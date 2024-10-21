#include <ntifs.h>
#include "memory.h"

extern "C" {
	NTKERNELAPI NTSTATUS IoCreateDriver(PUNICODE_STRING DriverName, PDRIVER_INITIALIZE InitializationFunction);
	NTKERNELAPI NTSTATUS MmCopyVirtualMemory(PEPROCESS SourceProcess, PVOID SourceAddress, PEPROCESS TargetProcess, PVOID TargetAddress, SIZE_T BufferSize, KPROCESSOR_MODE PreviousMode, PSIZE_T ReturnSize);
}

constexpr ULONG init_code = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x775, METHOD_BUFFERED, FILE_SPECIAL_ACCESS);
constexpr ULONG read_code = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x776, METHOD_BUFFERED, FILE_SPECIAL_ACCESS);
constexpr ULONG write_code = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x777, METHOD_BUFFERED, FILE_SPECIAL_ACCESS);

struct info_t {
	HANDLE target_pid = 0;
	void* target_address = 0x0;
	void* buffer_address = 0x0;
	SIZE_T size = 0;
	SIZE_T return_size = 0;
};

NTSTATUS ctl_io(PDEVICE_OBJECT device_obj, PIRP irp) {
	UNREFERENCED_PARAMETER(device_obj);

	static PEPROCESS s_target_process;

	irp->IoStatus.Information = sizeof(info_t);
	auto stack = IoGetCurrentIrpStackLocation(irp);
	auto buffer = (info_t*)irp->AssociatedIrp.SystemBuffer;

	if (stack) {
		if (buffer && sizeof(*buffer) >= sizeof(info_t)) {
			const auto ctl_code = stack->Parameters.DeviceIoControl.IoControlCode;

			if (ctl_code == init_code) {
				PsLookupProcessByProcessId(buffer->target_pid, &s_target_process);
			}
			else if (ctl_code == read_code) {
				SIZE_T bytesRead = 0;
				NTSTATUS status = ReadProcessMemory((int)buffer->target_pid,
					buffer->target_address,
					buffer->buffer_address,
					buffer->size,
					&bytesRead);
				buffer->return_size = bytesRead;
				return status;
			}
			else if (ctl_code == write_code) {
				SIZE_T bytesWritten = 0;
				NTSTATUS status = WriteProcessMemory((int)buffer->target_pid,
					buffer->target_address,
					buffer->buffer_address,
					buffer->size,
					&bytesWritten);
				buffer->return_size = bytesWritten;
				return status;		
			}
		}
	}

	IoCompleteRequest(irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

NTSTATUS unsupported_io(PDEVICE_OBJECT device_obj, PIRP irp) {
	UNREFERENCED_PARAMETER(device_obj);

	irp->IoStatus.Status = STATUS_NOT_SUPPORTED;
	IoCompleteRequest(irp, IO_NO_INCREMENT);
	return irp->IoStatus.Status;
}

NTSTATUS create_io(PDEVICE_OBJECT device_obj, PIRP irp) {
	UNREFERENCED_PARAMETER(device_obj);

	IoCompleteRequest(irp, IO_NO_INCREMENT);
	return irp->IoStatus.Status;
}

NTSTATUS close_io(PDEVICE_OBJECT device_obj, PIRP irp) {
	UNREFERENCED_PARAMETER(device_obj);

	IoCompleteRequest(irp, IO_NO_INCREMENT);
	return irp->IoStatus.Status;
}

NTSTATUS real_main(PDRIVER_OBJECT driver_obj, PUNICODE_STRING registery_path) {
	UNREFERENCED_PARAMETER(registery_path);

	UNICODE_STRING dev_name, sym_link;
	PDEVICE_OBJECT dev_obj;

	RtlInitUnicodeString(&dev_name, L"\\Device\\amogus");
	auto status = IoCreateDevice(driver_obj, 0, &dev_name, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &dev_obj);
	if (status != STATUS_SUCCESS) return status;

	RtlInitUnicodeString(&sym_link, L"\\DosDevices\\amogus");
	status = IoCreateSymbolicLink(&sym_link, &dev_name);
	if (status != STATUS_SUCCESS) return status;

	SetFlag(dev_obj->Flags, DO_BUFFERED_IO);

	for (int t = 0; t <= IRP_MJ_MAXIMUM_FUNCTION; t++)
		driver_obj->MajorFunction[t] = unsupported_io;

	driver_obj->MajorFunction[IRP_MJ_CREATE] = create_io;
	driver_obj->MajorFunction[IRP_MJ_CLOSE] = close_io;
	driver_obj->MajorFunction[IRP_MJ_DEVICE_CONTROL] = ctl_io;
	driver_obj->DriverUnload = NULL;

	ClearFlag(dev_obj->Flags, DO_DEVICE_INITIALIZING);
	return status;
}

extern "C" NTSTATUS DriverEntry(PDRIVER_OBJECT driver_obj, PUNICODE_STRING registery_path) {
	UNREFERENCED_PARAMETER(driver_obj);
	UNREFERENCED_PARAMETER(registery_path);

	UNICODE_STRING  drv_name;
	RtlInitUnicodeString(&drv_name, L"\\Driver\\amogus");
	IoCreateDriver(&drv_name, &real_main);

	return STATUS_SUCCESS;
}