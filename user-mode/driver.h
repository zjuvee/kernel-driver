#pragma once
#include <Windows.h>
#include <iostream>
#include <TlHelp32.h>
#include <tchar.h>

constexpr DWORD init_code = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x775, METHOD_BUFFERED, FILE_SPECIAL_ACCESS);
constexpr DWORD read_code = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x776, METHOD_BUFFERED, FILE_SPECIAL_ACCESS);
constexpr DWORD write_code = CTL_CODE(FILE_DEVICE_UNKNOWN, 0x777, METHOD_BUFFERED, FILE_SPECIAL_ACCESS);

DWORD get_pid(const char* processName)
{
    if (processName == NULL)
        return 0;
    DWORD pid = 0;
    DWORD threadCount = 0;

    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32 pe;

    pe.dwSize = sizeof(PROCESSENTRY32);
    Process32First(hSnap, &pe);
    while (Process32Next(hSnap, &pe)) {
        if (_tcsicmp(pe.szExeFile, processName) == 0) {
            if ((int)pe.cntThreads > threadCount) {
                threadCount = pe.cntThreads;

                pid = pe.th32ProcessID;

            }
        }
    }
    return pid;
}

auto get_module(const char* module_name, DWORD process_id) {
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, process_id);
    if (hSnap != INVALID_HANDLE_VALUE) {
        MODULEENTRY32 modEntry;
        modEntry.dwSize = sizeof(modEntry);
        if (Module32First(hSnap, &modEntry)) {
            do {
                if (!strcmp(modEntry.szModule, module_name)) {
                    CloseHandle(hSnap);
                    return modEntry;
                }
            } while (Module32Next(hSnap, &modEntry));
        }
    }
}

class driver_manager {
    HANDLE m_driver_handle = nullptr; 

    struct info_t {
        UINT64 target_pid = 0;
        UINT64 target_address = 0x0;
        UINT64 buffer_address = 0x0;
        UINT64 size = 0;
        UINT64 return_size = 0;
    };

public:
    DWORD pid;

    driver_manager(const char* driver_name, const char* pName) {
        m_driver_handle = CreateFileA(driver_name, GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr); //get a handle to our driver
        if (m_driver_handle == INVALID_HANDLE_VALUE) {
            std::cout << "error opening driver handle" << std::endl;
        }

        pid = get_pid(pName);

        attach_to_process();
    }

    void attach_to_process() {
        info_t io_info;

        io_info.target_pid = pid;
        DeviceIoControl(m_driver_handle, init_code, &io_info, sizeof(io_info), &io_info, sizeof(io_info), nullptr, nullptr);
    }

    template<typename T> T RPM(const UINT64 address) {
        info_t io_info;
        T read_data;

        io_info.target_pid = pid;
        io_info.target_address = address;
        io_info.buffer_address = reinterpret_cast<UINT64>(&read_data);
        io_info.size = sizeof(T);

        NTSTATUS status = DeviceIoControl(m_driver_handle, read_code, &io_info, sizeof(io_info), &io_info, sizeof(io_info), nullptr, nullptr);
        if (!status) {
            throw std::runtime_error("DeviceIoControl read call failed");
        }

        return read_data;
    }

    template<typename T> bool WPM(const UINT64 address, const T buffer) {
        info_t io_info;

        io_info.target_pid = pid;
        io_info.target_address = address;
        io_info.buffer_address = (UINT64)&buffer;
        io_info.size = sizeof(T);

        NTSTATUS status = DeviceIoControl(m_driver_handle, write_code, &io_info, sizeof(io_info), &io_info, sizeof(io_info), nullptr, nullptr);
        if (!status) {
            throw std::runtime_error("DeviceIoControl write call failed");
        }

        return io_info.return_size == sizeof(T);
    }
};