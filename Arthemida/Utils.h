﻿#pragma once
#include <Windows.h>
#include <stdio.h>
#include <algorithm>
#include <string>
#include <map>
#include "CRC32.h"

// Выстраивает и возвращает список базовых адресов загруженных в процесс модулей и их размера
static std::map<LPVOID, DWORD> __stdcall BuildModuledMemoryMap()
{
	std::map<LPVOID, DWORD> memoryMap; HMODULE hMods[1024]; DWORD cbNeeded;
	typedef BOOL(__stdcall* PtrEnumProcessModules)(HANDLE hProcess, HMODULE* lphModule, DWORD cb, LPDWORD lpcbNeeded);
	PtrEnumProcessModules EnumProcModules = (PtrEnumProcessModules)
	GetProcAddress(LoadLibraryA("psapi.dll"), "EnumProcessModules");
	EnumProcModules(GetCurrentProcess(), hMods, sizeof(hMods), &cbNeeded);
	typedef BOOL(__stdcall* GetMdlInfoP)(HANDLE hProcess, HMODULE hModule, LPMODULEINFO lpmodinfo, DWORD cb);
	GetMdlInfoP GetMdlInfo = (GetMdlInfoP)GetProcAddress(LoadLibraryA("psapi.dll"), "GetModuleInformation");
	for (unsigned int i = 0; i < (cbNeeded / sizeof(HMODULE)); i++)
	{
		MODULEINFO modinfo; GetMdlInfo(GetCurrentProcess(), hMods[i], &modinfo, sizeof(modinfo));
		memoryMap.insert(memoryMap.begin(), std::pair<LPVOID, DWORD>(modinfo.lpBaseOfDll, modinfo.SizeOfImage));
	}
	return memoryMap;
}
// 
static bool __stdcall IsMemoryInModuledRange(LPVOID base)
{
	std::map<LPVOID, DWORD> memory = BuildModuledMemoryMap();
	for (const auto& it : memory)
	{
		if (base >= it.first && base <= (LPVOID)((DWORD_PTR)it.first + it.second)) return true;
	}
	return false;
}
// Генерация CRC32 хеша файла
DWORD GenerateCRC32(const std::string filePath)
{
	if (filePath.empty()) return 0x0;
	auto getFileSize = [](FILE* file) -> long
	{
		long lCurPos, lEndPos;
		lCurPos = ftell(file);
		fseek(file, 0, 2);
		lEndPos = ftell(file);
		fseek(file, lCurPos, 0);
		return lEndPos;
	};
	FILE* hFile = fopen(filePath.c_str(), "rb");
	if (hFile == nullptr) return 0x0;
	BYTE* fileBuf; long fileSize;
	fileSize = getFileSize(hFile);
	fileBuf = new BYTE[fileSize];
	fread(fileBuf, fileSize, 1, hFile);
	fclose(hFile); DWORD crc = CRC::Calculate(fileBuf, fileSize, CRC::CRC_32());
	delete[] fileBuf; return crc;
}
template<typename First, typename Second>
bool SearchForSingleMapMatch(const std::map<First, Second>& map, const First key)
{
	for (auto it : map)
	{
		if (it.first == key) return true;
	}
	return false;
}
template <typename T>
const bool Contains(std::vector<T>& Vec, const T& Element)
{
	if (std::find(Vec.begin(), Vec.end(), Element) != Vec.end()) return true;
	return false;
}
std::string SearchForSingleMapMatchAndRet(const std::map<PVOID, const char*>& map, const PVOID key)
{
	for (auto it : map)
	{
		if (it.first == key) return it.second;
	}
	return "EMPTY";
}
// Поиск субстринга без case-sensevity
bool findStringIC(const std::string& strHaystack, const std::string& strNeedle)
{
	auto it = std::search(strHaystack.begin(), strHaystack.end(),
	strNeedle.begin(), strNeedle.end(),
	[](char ch1, char ch2) { return std::toupper(ch1) == std::toupper(ch2); });
	return (it != strHaystack.end());
}
// 
bool SearchForSingleMultiMapMatch2(const std::multimap<DWORD, std::string>& map, DWORD first, std::string second, bool firstOrSecond)
{
	for (auto it : map)
	{
		if (findStringIC(it.second, second) && firstOrSecond) return true;
		if (it.first == first && !firstOrSecond) return true;
	}
	return false;
}
//
char* strdel(char* s, size_t offset, size_t count)
{
	size_t len = strlen(s);
	if (offset > len) return s;
	if ((offset + count) > len) count = len - offset;
	strcpy(s + offset, s + offset + count);
	return s;
}
std::string GetDllName(char* szDllNameTmp)
{
	char szDllName[300]; memset(szDllName, 0, sizeof(szDllName));
	strcpy(szDllName, szDllNameTmp);
	char fname[256]; char* ipt = strrchr(szDllName, '\\');
	memset(fname, 0, sizeof(fname));
	strdel(szDllName, 0, (ipt - szDllName + 1));
	strncpy(fname, szDllName, strlen(szDllName));
	std::string ProcName(fname);
	return ProcName;
}
bool IsVecContain(const std::vector<PVOID>& source, PVOID element)
{
	for (const auto it : source)
	{
		if (it == element) return true;
	}
	return false;
}
bool CheckCRC32(HMODULE mdl, std::multimap<DWORD, std::string> &ModuleSnapshot)
{
	if (mdl == nullptr) return false;
	CHAR szFileName[MAX_PATH + 1]; GetModuleFileNameA(mdl, szFileName, MAX_PATH + 1);
	DWORD CRC32 = GenerateCRC32(szFileName); std::string DllName = GetDllName(szFileName);
	if (SearchForSingleMultiMapMatch2(ModuleSnapshot, 0x0, DllName, true) && 
	SearchForSingleMultiMapMatch2(ModuleSnapshot, CRC32, "", false))
	{
		std::multimap<std::string, DWORD> tmpModuleSnapshot;
		for (const auto& it : ModuleSnapshot)
		{
			tmpModuleSnapshot.insert(tmpModuleSnapshot.begin(), std::pair<std::string, DWORD>(it.second, it.first));
		}
		if (tmpModuleSnapshot.count(DllName) == 0x1) return true;
	}
	else
	{
		ModuleSnapshot.insert(ModuleSnapshot.begin(), std::pair<DWORD, std::string>(CRC32, DllName));
		return true;
	}
	return false;
}