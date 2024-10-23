#include "driver.h"

#include <iostream>
#include <Windows.h>
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

int main() {

	auto driver = new driver_manager("\\\\.\\amogus", "cs2.exe");

	auto client = get_module("client.dll", driver->pid);
	auto client_base = (uintptr_t)client.modBaseAddr;

	while (true) {
		uintptr_t LocalPlayer = driver->RPM<uintptr_t>(client_base + dwLocalPlayer);
		
		auto CameraService = driver->RPM<uintptr_t>(LocalPlayer + m_pCameraServices);

		auto flags = driver->RPM<uint32_t>(LocalPlayer + m_fFlags);
		auto fov = driver->RPM<uint32_t>(CameraService + m_iFOV);

		cout << flags << endl;

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