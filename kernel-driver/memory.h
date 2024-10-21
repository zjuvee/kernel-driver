#pragma once

#include <ntdef.h>
#include <ntifs.h>
#include <intrin.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

    NTSTATUS ReadProcessMemory(ULONG pid, PVOID Address, PVOID AllocatedBuffer, SIZE_T size, SIZE_T* read);
    NTSTATUS WriteProcessMemory(ULONG pid, PVOID Address, PVOID AllocatedBuffer, SIZE_T size, SIZE_T* written);

#ifdef __cplusplus
}
#endif
