#include "Arthemida.h"

void __stdcall ART_LIB::ArtemisLibrary::HookScanner(ArtemisConfig* cfg)
{
	return; // � ����������
	if (cfg == nullptr) return;
	if (cfg->callback == nullptr) return;
	
	// ���������� � ������������ ��������� ��������
	if (cfg->ProtectedFunctionPatterns.empty()) return;
	for (auto PatternPair : cfg->ProtectedFunctionPatterns)
	{
		
	}

	while (true) 
	{
		

		Sleep(cfg->HookScanDelay);
	}
}