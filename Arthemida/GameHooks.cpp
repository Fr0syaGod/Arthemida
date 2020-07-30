#include "GameHooks.h"
GameHooks::ArtemisConfig* GameHooks::cfg = nullptr;
GameHooks::ptrAddEventHandler GameHooks::callAddEventHandler = nullptr;
GameHooks::callGetCustomData GameHooks::ptrGetCustomData = (GameHooks::callGetCustomData)0x0;
GameHooks::callSetCustomData GameHooks::ptrSetCustomData = (GameHooks::callSetCustomData)0x0;
GameHooks::ptrCheckUTF8BOMAndUpdate GameHooks::callCheckUTF8BOMAndUpdate = (GameHooks::ptrCheckUTF8BOMAndUpdate)0x0;
GameHooks::ptrTriggerServerEvent GameHooks::callTriggerServerEvent = (GameHooks::ptrTriggerServerEvent)0x0;
GameHooks::GameHooks()
{
#ifdef ARTEMIS_DEBUG
    DeleteFileA(ARTEMIS_LOG);
    Utils::LogInFile(ARTEMIS_LOG, "Artemis Library loaded!\n");
#endif
}
void GameHooks::CheckIfReturnIsLegit(const char* function_name, PVOID return_address)
{
	vector<string> allowedModules = { "client.dll", "multiplayer_sa.dll", "game_sa.dll", 
	"core.dll", "gta_sa.exe", "proxy_sa.exe", "lua5.1c.dll", "pcre3.dll" };
    string moduleName = Utils::GetNameOfModuledAddressSpace(return_address, Utils::GenerateModuleNamesList());
	if (!Utils::IsVecContain2(allowedModules, moduleName) && !Utils::IsVecContain(cfg->ExcludedMethods, return_address))
	{
        typedef DWORD(__stdcall* LPFN_GetMappedFileNameA)(HANDLE hProcess, LPVOID lpv, LPCSTR lpFilename, DWORD nSize);
        LPFN_GetMappedFileNameA g_GetMappedFileNameA = nullptr; HMODULE hPsapi = LoadLibraryA("psapi.dll");
        g_GetMappedFileNameA = (LPFN_GetMappedFileNameA)GetProcAddress(hPsapi, "GetMappedFileNameA");
        char MappedName[256]; memset(MappedName, 0, sizeof(MappedName));
        g_GetMappedFileNameA(GetCurrentProcess(), (PVOID)return_address, MappedName, sizeof(MappedName));
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        MEMORY_BASIC_INFORMATION mme{ 0 }; ARTEMIS_DATA data; // ���������� �������� ��������� ��������
        VirtualQueryEx(GetCurrentProcess(), return_address, &mme, 5); // ��������� ��������� ���������� �� ������� ������
        data.baseAddr = (LPVOID)return_address; // ������ �������� ������ ������� ������
        data.MemoryRights = mme.AllocationProtect; // ������ ���� ������� � ������� ������
        data.regionSize = mme.RegionSize; // ������ ������� ������� ������
        data.type = DetectionType::ART_RETURN_ADDRESS; // ����������� ���� �������
        data.dllName = moduleName; data.dllPath = MappedName; // ������������ ������ � ���� � ����
        cfg->callback(&data); cfg->ExcludedMethods.push_back(return_address); // �������� ������� �������� � ��������� ������������ � ����-����
#ifdef ARTEMIS_DEBUG
		Utils::LogInFile(ARTEMIS_LOG, "Returned from %s function to 0x%X in to module %s\n", function_name, return_address, moduleName.c_str());
#endif
	}
}
void* __fastcall GameHooks::GetCustomData(CClientEntity* ECX, void* EDX, const char* szName, bool bInheritData, bool* pbIsSynced)
{
    GameHooks::CheckIfReturnIsLegit(__FUNCTION__, _ReturnAddress());
    return ptrGetCustomData(ECX, szName, bInheritData, pbIsSynced);
}
void __fastcall GameHooks::SetCustomData(CClientEntity* ECX, void* EDX, const char* szName, void* Variable, bool bSynchronized)
{
    GameHooks::CheckIfReturnIsLegit(__FUNCTION__, _ReturnAddress());
    ptrSetCustomData(ECX, szName, Variable, bSynchronized);
}
bool __cdecl GameHooks::AddEventHandler(CLuaMain* LuaMain, const char* szName, CClientEntity* Entity,
const CLuaFunctionRef* iLuaFunction, bool bPropagated, DWORD eventPriority, float fPriorityMod)
{
    GameHooks::CheckIfReturnIsLegit(__FUNCTION__, _ReturnAddress());
    return callAddEventHandler(LuaMain, szName, Entity, iLuaFunction, bPropagated, eventPriority, fPriorityMod);
}
bool __cdecl GameHooks::CheckUTF8BOMAndUpdate(char** pcpOutBuffer, unsigned int* puiOutSize)
{
    GameHooks::CheckIfReturnIsLegit(__FUNCTION__, _ReturnAddress());
    return callCheckUTF8BOMAndUpdate(pcpOutBuffer, puiOutSize);
}
bool __cdecl GameHooks::TriggerServerEvent(const char* szName, CClientEntity* CallWithEntity, void* Arguments)
{
    GameHooks::CheckIfReturnIsLegit(__FUNCTION__, _ReturnAddress());
    return callTriggerServerEvent(szName, CallWithEntity, Arguments);
}