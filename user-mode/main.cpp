#include "driver.h"

#include <iostream>
#include <TlHelp32.h>
#include <Windows.h>
#include <cstdint>
#include <thread>

constexpr uintptr_t dwLocalPlayer = 0x1831AE8;
constexpr uintptr_t m_pCameraServices = 0x11E0;
constexpr uintptr_t dwForceJump = 0x182AAF0;
constexpr uintptr_t m_fFlags = 0x3EC;
constexpr uintptr_t m_iFOV = 0x210;

const uint32_t STANDING = 65665;
const uint32_t CROUCHING = 65667;
const uint32_t PLUS_JUMP = 65537;
const uint32_t MINUS_JUMP = 256;

using namespace std;

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

int main() {
	auto hwnd = FindWindowA(NULL, "Counter-Strike 2");
	DWORD process_id; GetWindowThreadProcessId(hwnd, &process_id);
	auto client = get_module("client.dll", process_id);
	auto client_base = (uintptr_t)client.modBaseAddr;

	auto driver = new driver_manager("\\\\.\\amogus", process_id);

	while (true) {

		auto LocalPlayer = driver->RPM<uintptr_t>(client_base + dwLocalPlayer);
		auto CameraService = driver->RPM<uintptr_t>(LocalPlayer + m_pCameraServices);
		
		auto flags = driver->RPM<uint32_t>(LocalPlayer + m_fFlags);
		auto fov = driver->RPM<uint32_t>(CameraService + m_iFOV);

		cout << fov << endl;

		if (GetAsyncKeyState(0x20) & 0x8000)
		{
			if (flags == STANDING || flags == CROUCHING)
			{
				this_thread::sleep_for(chrono::milliseconds(10));
				driver->WPM<uint32_t>(client_base + dwForceJump, PLUS_JUMP);
				//driver->WPM<uint32_t>(CameraService + m_iFOV, 100);
			}
			else
			{
				driver->WPM<uint32_t>(client_base + dwForceJump, MINUS_JUMP);
			}
		}
		this_thread::sleep_for(chrono::milliseconds(10));
	}
}