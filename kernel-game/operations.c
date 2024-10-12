#include "driver.h"
#include "message.h"

NTSTATUS ReadProcessMemory(int pid, PVOID Address, PVOID AllocatedBuffer, SIZE_T size, SIZE_T* read);
NTSTATUS WriteProcessMemory(int pid, PVOID Address, PVOID AllocatedBuffer, SIZE_T size, SIZE_T* written);

//  IOCTL
NTSTATUS MyDriverDeviceControl(PDEVICE_OBJECT DeviceObject, PIRP Irp) {
    PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);
    NTSTATUS status = STATUS_SUCCESS;
    ULONG inBufferSize = stack->Parameters.DeviceIoControl.InputBufferLength;
    ULONG outBufferSize = stack->Parameters.DeviceIoControl.OutputBufferLength;
    PVOID inputBuffer = Irp->AssociatedIrp.SystemBuffer;
    PVOID outputBuffer = Irp->AssociatedIrp.SystemBuffer;

    switch (stack->Parameters.DeviceIoControl.IoControlCode) {
    case IOCTL_READ_MEMORY: {
        if (inBufferSize < sizeof(MEMORY_READ_REQUEST)) {
            status = STATUS_BUFFER_TOO_SMALL;
            break;
        }
        MEMORY_READ_REQUEST* readRequest = (MEMORY_READ_REQUEST*)inputBuffer;
        SIZE_T bytesRead = 0;
        status = ReadProcessMemory(readRequest->pid, readRequest->address, outputBuffer, readRequest->size, &bytesRead);
        Irp->IoStatus.Information = bytesRead;
        break;
    }

    case IOCTL_WRITE_MEMORY: {
        if (inBufferSize < sizeof(MEMORY_WRITE_REQUEST)) {
            status = STATUS_BUFFER_TOO_SMALL;
            break;
        }
        MEMORY_WRITE_REQUEST* writeRequest = (MEMORY_WRITE_REQUEST*)inputBuffer;
        SIZE_T bytesWritten = 0;
        status = WriteProcessMemory(writeRequest->pid, writeRequest->address, writeRequest->buffer, writeRequest->size, &bytesWritten);
        Irp->IoStatus.Information = bytesWritten;
        break;
    }

    default:
        status = STATUS_INVALID_DEVICE_REQUEST;
        break;
    }

    Irp->IoStatus.Status = status;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return status;
}


// create and close the driver
NTSTATUS MyDriverCreate(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
    UNREFERENCED_PARAMETER(DeviceObject);
    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return STATUS_SUCCESS;
}

NTSTATUS MyDriverClose(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
    UNREFERENCED_PARAMETER(DeviceObject);
    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return STATUS_SUCCESS;
}


