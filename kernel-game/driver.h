#pragma once

#ifndef DRIVER_H
#define DRIVER_H

#ifdef _KERNEL_MODE
// kernel mode
#include <ntddk.h>

#define IOCTL_READ_MEMORY CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_WRITE_MEMORY CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_BUFFERED, FILE_ANY_ACCESS)

typedef struct _MEMORY_READ_REQUEST {
    int pid;
    PVOID address;
    SIZE_T size;
} MEMORY_READ_REQUEST, * PMEMORY_READ_REQUEST;

typedef struct _MEMORY_WRITE_REQUEST {
    int pid;
    PVOID address;
    PVOID buffer;
    SIZE_T size;
} MEMORY_WRITE_REQUEST, * PMEMORY_WRITE_REQUEST;

NTSTATUS MyDriverCreate(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS MyDriverClose(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS MyDriverDeviceControl(PDEVICE_OBJECT DeviceObject, PIRP Irp);
VOID MyDriverUnload(PDRIVER_OBJECT DriverObject);

#else
// usermode
#include <windows.h>

#define IOCTL_READ_MEMORY CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_WRITE_MEMORY CTL_CODE(FILE_DEVICE_UNKNOWN, 0x801, METHOD_BUFFERED, FILE_ANY_ACCESS)

typedef struct _MEMORY_READ_REQUEST {
    int pid;
    PVOID address;
    SIZE_T size;
} MEMORY_READ_REQUEST, * PMEMORY_READ_REQUEST;

typedef struct _MEMORY_WRITE_REQUEST {
    int pid;
    PVOID address;
    PVOID buffer;
    SIZE_T size;
} MEMORY_WRITE_REQUEST, * PMEMORY_WRITE_REQUEST;

#endif // _KERNEL_MODE

#endif // DRIVER_H
