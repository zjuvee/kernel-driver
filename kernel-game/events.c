/*
#pragma warning (disable: 4047)

#include "events.h"
#include "message.h"
#include "process_info.h"

#ifndef PROCESS_INFO_H
#define PROCESS_INFO_H

#include <ntifs.h>

#define SystemProcessInformation 5

NTSTATUS ZwQuerySystemInformation(
    ULONG SystemInformationClass,
    PVOID SystemInformation,
    ULONG SystemInformationLength,
    PULONG ReturnLength
);

typedef struct _SYSTEM_PROCESS_INFORMATION {
    ULONG NextEntryOffset;
    ULONG NumberOfThreads;
    LARGE_INTEGER WorkingSetPrivateSize;
    ULONG HardFaultCount;
    ULONG NumberOfThreadsHighWatermark;
    ULONGLONG CycleTime;
    LARGE_INTEGER CreateTime;
    LARGE_INTEGER UserTime;
    LARGE_INTEGER KernelTime;
    UNICODE_STRING ImageName;
    ULONG BasePriority;
    HANDLE UniqueProcessId;
    HANDLE InheritedFromUniqueProcessId;
    ULONG HandleCount;
    ULONG SessionId;
    ULONG_PTR UniqueProcessKey;
    SIZE_T PeakVirtualSize;
    SIZE_T VirtualSize;
    ULONG PageFaultCount;
    SIZE_T PeakWorkingSetSize;
    SIZE_T WorkingSetSize;
    SIZE_T QuotaPeakPagedPoolUsage;
    SIZE_T QuotaPagedPoolUsage;
    SIZE_T QuotaPeakNonPagedPoolUsage;
    SIZE_T QuotaNonPagedPoolUsage;
    SIZE_T PagefileUsage;
    SIZE_T PeakPagefileUsage;
    SIZE_T PrivatePageCount;
    LARGE_INTEGER ReadOperationCount;
    LARGE_INTEGER WriteOperationCount;
    LARGE_INTEGER OtherOperationCount;
    LARGE_INTEGER ReadTransferCount;
    LARGE_INTEGER WriteTransferCount;
    LARGE_INTEGER OtherTransferCount;
} SYSTEM_PROCESS_INFORMATION, * PSYSTEM_PROCESS_INFORMATION;

#endif

VOID lsass()
{
    NTSTATUS status;
    ULONG bufferSize = 1024 * 1024;
    PVOID buffer = ExAllocatePoolWithTag(NonPagedPool, bufferSize, 'proc');

    if (!buffer)
        return;

    status = ZwQuerySystemInformation(SystemProcessInformation, buffer, bufferSize, NULL);

    if (NT_SUCCESS(status)) {
        PSYSTEM_PROCESS_INFORMATION processInfo = (PSYSTEM_PROCESS_INFORMATION)buffer;

        while (processInfo->NextEntryOffset) {
            if (processInfo->ImageName.Buffer) {
                if (wcsstr(processInfo->ImageName.Buffer, L"lsass.exe")) {
                    DebugMessage("LSASS.exe found! PID: %d \n", processInfo->UniqueProcessId);
                    break;
                }
            }
            processInfo = (PSYSTEM_PROCESS_INFORMATION)((PUCHAR)processInfo + processInfo->NextEntryOffset);
        }
    }

    ExFreePool(buffer);
}
*/