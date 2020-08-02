#include "Arthemida.h"

typedef struct
{
	bool installed;
	ART_LIB::ArtemisLibrary::ArtemisCallback callback;
	std::map<PVOID, const char*> ForbiddenApcList;
} APC_FILTER, * PAPC_FILTER;
APC_FILTER flt;

extern "C" void __stdcall KiApcStub(); // �������� ����� ASM, �������� ApcHandler

// ���������� APC
extern "C" void __stdcall HandleApc(PVOID ApcRoutine, PVOID Argument, PCONTEXT Context)
{
	auto IsRoutineForbidden = [](PVOID Routine) -> bool
	{
		if (Utils::SearchForSingleMapMatch<PVOID, const char*>(flt.ForbiddenApcList, Routine)) return true;
		return false;
	};
	if (IsRoutineForbidden(Argument))
	{
		ART_LIB::ArtemisLibrary::ARTEMIS_DATA ApcInfo;
		char ForbiddenName[45]; memset(ForbiddenName, 0, sizeof(ForbiddenName));
		strcpy(ForbiddenName, Utils::SearchForSingleMapMatchAndRet(flt.ForbiddenApcList, Argument).c_str());
		ApcInfo.ApcInfo = std::make_tuple(Argument, Context, ForbiddenName);
		ApcInfo.type = ART_LIB::ArtemisLibrary::DetectionType::ART_APC_INJECTION;
		flt.callback(&ApcInfo);
	}
}

#ifdef _WIN64
extern "C" void __stdcall ApcHandler(PCONTEXT Context)
{
	HandleApc(reinterpret_cast<PVOID>(Context->P4Home), reinterpret_cast<PVOID>(Context->P1Home), Context);
}
#else
extern "C" void __stdcall ApcHandler(PVOID ApcRoutine, PVOID Arg, PCONTEXT Context)
{
	HandleApc(ApcRoutine, Arg, Context);
}
#endif

extern "C" void(__stdcall * OriginalApcDispatcher)(PVOID NormalRoutine, PVOID SysArg1, PVOID SysArg2, CONTEXT Context) = nullptr;
using ApcDispatcherPtr = void(__stdcall*)(PVOID NormalRoutine, PVOID SysArg1, PVOID SysArg2, CONTEXT Context);

// ����� ��� ������������� APC ����������� (���������� ���������� ������ ������� APC � ��������� ������������)
bool __stdcall ART_LIB::ArtemisLibrary::InstallApcDispatcher(ArtemisConfig* cfg)
{
	if (cfg == nullptr || cfg->callback == nullptr) return false; // ������ ����� ������ ����������
	if (flt.installed) return true; // ������ �� ��������� ��������� �����������
	flt.callback = cfg->callback; // �������� ��������� �� ������� �.� ��� ���������� �������
	OriginalApcDispatcher = (ApcDispatcherPtr)GetProcAddress(GetModuleHandleA("ntdll.dll"), "KiUserApcDispatcher");
	if (OriginalApcDispatcher == nullptr) return false;
	auto MakeForbiddenList = []() -> std::map<PVOID, const char*>
	{
		std::map<PVOID, const char*> forbidden;
		forbidden.insert(std::pair<PVOID, const char*>((PVOID)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA"), "LoadLibraryA"));
		forbidden.insert(std::pair<PVOID, const char*>((PVOID)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryW"), "LoadLibraryW"));
		forbidden.insert(std::pair<PVOID, const char*>((PVOID)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryExA"), "LoadLibraryExA"));
		forbidden.insert(std::pair<PVOID, const char*>((PVOID)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryExW"), "LoadLibraryExW"));
		forbidden.insert(std::pair<PVOID, const char*>((PVOID)GetProcAddress(GetModuleHandleA("kernelbase.dll"), "LoadLibraryA"), "LoadLibraryA"));
		forbidden.insert(std::pair<PVOID, const char*>((PVOID)GetProcAddress(GetModuleHandleA("kernelbase.dll"), "LoadLibraryW"), "LoadLibraryW"));
		forbidden.insert(std::pair<PVOID, const char*>((PVOID)GetProcAddress(GetModuleHandleA("kernelbase.dll"), "LoadLibraryExA"), "LoadLibraryExA"));
		forbidden.insert(std::pair<PVOID, const char*>((PVOID)GetProcAddress(GetModuleHandleA("kernelbase.dll"), "LoadLibraryExW"), "LoadLibraryExW"));
		forbidden.insert(std::pair<PVOID, const char*>((PVOID)GetProcAddress(GetModuleHandleA("ntdll.dll"), "LdrLoadDll"), "LdrLoadDll"));
		return forbidden;
	};
	flt.ForbiddenApcList = MakeForbiddenList(); // ��������� ��������� ������ ����������� APC
	if (MH_CreateHook(OriginalApcDispatcher, KiApcStub, reinterpret_cast<PVOID*>(&OriginalApcDispatcher)) == MH_OK) flt.installed = true;
	else
	{
		flt.installed = false; // ������ ���� ��� APC ���������� �� ��� ������� ��-�� ������
		return false; // ����������� ��� ��������� ������ � ��������� ���������
	}
	return true; // ���� ����� ��� APC ���������� ��� ������� ����������
}

bool __stdcall ART_LIB::ArtemisLibrary::DeleteApcDispatcher()
{
	if (OriginalApcDispatcher != nullptr) // ������ APC ����������� ���� �� ����������
	{
		MH_DisableHook(OriginalApcDispatcher);
		flt.installed = false; // ����� ����� �� "��������" ��� ����������� ��������� ���������
		return true;
	}
	return false;
}