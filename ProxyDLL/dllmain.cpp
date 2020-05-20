// dllmain.cpp : Defines the entry point for the DLL application.
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <thread>

void Init(LPVOID pvNothing) {
	MessageBoxA(NULL, "Proxy DLL injected", "Proxy dxgi.dll", MB_OK);
	while (true) { }
}

#pragma region DLL
struct dxgi_dll {
	HMODULE dll;
	FARPROC OrignalApplyCompatResolutionQuirking;
	FARPROC OrignalCompatString;
	FARPROC OrignalCompatValue;
	FARPROC OrignalCreateDXGIFactory;
	FARPROC OrignalCreateDXGIFactory1;
	FARPROC OrignalCreateDXGIFactory2;
	FARPROC OrignalDXGID3D10CreateDevice;
	FARPROC OrignalDXGID3D10CreateLayeredDevice;
	FARPROC OrignalDXGID3D10GetLayeredDeviceSize;
	FARPROC OrignalDXGID3D10RegisterLayers;
	FARPROC OrignalDXGIDeclareAdapterRemovalSupport;
	FARPROC OrignalDXGIDumpJournal;
	FARPROC OrignalDXGIGetDebugInterface1;
	FARPROC OrignalDXGIReportAdapterConfiguration;
	FARPROC OrignalPIXBeginCapture;
	FARPROC OrignalPIXEndCapture;
	FARPROC OrignalPIXGetCaptureState;
	FARPROC OrignalSetAppCompatStringPointer;
	FARPROC OrignalUpdateHMDEmulationStatus;
} dxgi;

__declspec(naked) void FakeApplyCompatResolutionQuirking() { _asm { jmp[dxgi.OrignalApplyCompatResolutionQuirking] } }
__declspec(naked) void FakeCompatString() { _asm { jmp[dxgi.OrignalCompatString] } }
__declspec(naked) void FakeCompatValue() { _asm { jmp[dxgi.OrignalCompatValue] } }
__declspec(naked) void FakeCreateDXGIFactory() { _asm { jmp[dxgi.OrignalCreateDXGIFactory] } }
__declspec(naked) void FakeCreateDXGIFactory1() { _asm { jmp[dxgi.OrignalCreateDXGIFactory1] } }
__declspec(naked) void FakeCreateDXGIFactory2() { _asm { jmp[dxgi.OrignalCreateDXGIFactory2] } }
__declspec(naked) void FakeDXGID3D10CreateDevice() { _asm { jmp[dxgi.OrignalDXGID3D10CreateDevice] } }
__declspec(naked) void FakeDXGID3D10CreateLayeredDevice() { _asm { jmp[dxgi.OrignalDXGID3D10CreateLayeredDevice] } }
__declspec(naked) void FakeDXGID3D10GetLayeredDeviceSize() { _asm { jmp[dxgi.OrignalDXGID3D10GetLayeredDeviceSize] } }
__declspec(naked) void FakeDXGID3D10RegisterLayers() { _asm { jmp[dxgi.OrignalDXGID3D10RegisterLayers] } }
__declspec(naked) void FakeDXGIDeclareAdapterRemovalSupport() { _asm { jmp[dxgi.OrignalDXGIDeclareAdapterRemovalSupport] } }
__declspec(naked) void FakeDXGIDumpJournal() { _asm { jmp[dxgi.OrignalDXGIDumpJournal] } }
__declspec(naked) void FakeDXGIGetDebugInterface1() { _asm { jmp[dxgi.OrignalDXGIGetDebugInterface1] } }
__declspec(naked) void FakeDXGIReportAdapterConfiguration() { _asm { jmp[dxgi.OrignalDXGIReportAdapterConfiguration] } }
__declspec(naked) void FakePIXBeginCapture() { _asm { jmp[dxgi.OrignalPIXBeginCapture] } }
__declspec(naked) void FakePIXEndCapture() { _asm { jmp[dxgi.OrignalPIXEndCapture] } }
__declspec(naked) void FakePIXGetCaptureState() { _asm { jmp[dxgi.OrignalPIXGetCaptureState] } }
__declspec(naked) void FakeSetAppCompatStringPointer() { _asm { jmp[dxgi.OrignalSetAppCompatStringPointer] } }
__declspec(naked) void FakeUpdateHMDEmulationStatus() { _asm { jmp[dxgi.OrignalUpdateHMDEmulationStatus] } }
#pragma endregion

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	char path[MAX_PATH];
    switch (ul_reason_for_call)
    {
	case DLL_PROCESS_ATTACH: {
		CopyMemory(path + GetSystemDirectoryA(path, MAX_PATH - 10), "\\dxgi.dll", 11);
		dxgi.dll = LoadLibraryA(path);
		if (!dxgi.dll)
		{
			ExitProcess(0);
		}
#pragma region DLL
		dxgi.OrignalApplyCompatResolutionQuirking = GetProcAddress(dxgi.dll, "ApplyCompatResolutionQuirking");
		dxgi.OrignalCompatString = GetProcAddress(dxgi.dll, "CompatString");
		dxgi.OrignalCompatValue = GetProcAddress(dxgi.dll, "CompatValue");
		dxgi.OrignalCreateDXGIFactory = GetProcAddress(dxgi.dll, "CreateDXGIFactory");
		dxgi.OrignalCreateDXGIFactory1 = GetProcAddress(dxgi.dll, "CreateDXGIFactory1");
		dxgi.OrignalCreateDXGIFactory2 = GetProcAddress(dxgi.dll, "CreateDXGIFactory2");
		dxgi.OrignalDXGID3D10CreateDevice = GetProcAddress(dxgi.dll, "DXGID3D10CreateDevice");
		dxgi.OrignalDXGID3D10CreateLayeredDevice = GetProcAddress(dxgi.dll, "DXGID3D10CreateLayeredDevice");
		dxgi.OrignalDXGID3D10GetLayeredDeviceSize = GetProcAddress(dxgi.dll, "DXGID3D10GetLayeredDeviceSize");
		dxgi.OrignalDXGID3D10RegisterLayers = GetProcAddress(dxgi.dll, "DXGID3D10RegisterLayers");
		dxgi.OrignalDXGIDeclareAdapterRemovalSupport = GetProcAddress(dxgi.dll, "DXGIDeclareAdapterRemovalSupport");
		dxgi.OrignalDXGIDumpJournal = GetProcAddress(dxgi.dll, "DXGIDumpJournal");
		dxgi.OrignalDXGIGetDebugInterface1 = GetProcAddress(dxgi.dll, "DXGIGetDebugInterface1");
		dxgi.OrignalDXGIReportAdapterConfiguration = GetProcAddress(dxgi.dll, "DXGIReportAdapterConfiguration");
		dxgi.OrignalPIXBeginCapture = GetProcAddress(dxgi.dll, "PIXBeginCapture");
		dxgi.OrignalPIXEndCapture = GetProcAddress(dxgi.dll, "PIXEndCapture");
		dxgi.OrignalPIXGetCaptureState = GetProcAddress(dxgi.dll, "PIXGetCaptureState");
		dxgi.OrignalSetAppCompatStringPointer = GetProcAddress(dxgi.dll, "SetAppCompatStringPointer");
		dxgi.OrignalUpdateHMDEmulationStatus = GetProcAddress(dxgi.dll, "UpdateHMDEmulationStatus");
#pragma endregion
		DisableThreadLibraryCalls(hModule);
		std::thread initThread(Init, lpReserved);
		initThread.detach();
		break;
	}
	case DLL_PROCESS_DETACH: {
		FreeLibraryAndExitThread(hModule, EXIT_SUCCESS);
	}
	default:
		break;
    }
    return TRUE;
}

