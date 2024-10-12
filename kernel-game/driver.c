#include "driver.h"
//#include "memory.h"

NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath)
{
    UNREFERENCED_PARAMETER(RegistryPath);

    PDEVICE_OBJECT DeviceObject = NULL;
    NTSTATUS status;
    UNICODE_STRING deviceName = RTL_CONSTANT_STRING(L"\\Device\\kerneldriver");
    UNICODE_STRING symLink = RTL_CONSTANT_STRING(L"\\DosDevices\\kerneldriver");

    status = IoCreateDevice(DriverObject, 0, &deviceName, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &DeviceObject);
    if (!NT_SUCCESS(status)) {
        return status;
    }
   
    status = IoCreateSymbolicLink(&symLink, &deviceName);
    if (!NT_SUCCESS(status)) {
        IoDeleteDevice(DeviceObject);
        return status;
    }

    DriverObject->MajorFunction[IRP_MJ_CREATE] = MyDriverCreate;
    DriverObject->MajorFunction[IRP_MJ_CLOSE] = MyDriverClose;
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = MyDriverDeviceControl;
    DriverObject->DriverUnload = MyDriverUnload;

    return STATUS_SUCCESS;
}

VOID MyDriverUnload(PDRIVER_OBJECT DriverObject)
{
    UNICODE_STRING symLink = RTL_CONSTANT_STRING(L"\\DosDevices\\kerneldriver");

    IoDeleteSymbolicLink(&symLink);
    IoDeleteDevice(DriverObject->DeviceObject);
}
