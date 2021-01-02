#include "pch.h"
#include "hook_helper.h"
#include "csv_reader.h"

map<string, string> dictionary;

void FillDictionary()
{
	readData("df_rus.csv", dictionary);
}

BOOL DictSearch(char*& text)
{
	if (dictionary.find(text) == dictionary.end()) {
		return 0;
	}
	text = (char*)dictionary.at(text).c_str();
	return 1;
}

void CreateDebugConsole()
{
	FILE* fp = nullptr;
	AllocConsole();
	SetConsoleTitle("DF debugging console");

	system("mode con cols=40 lines=20");
	freopen_s(&fp, "CONOUT$", "w", stdout);
	SetConsoleOutputCP(1251);
	const COORD newSize = { 40,9999 };
	SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), newSize);
}

inline void ChangeProtection(void* ptr, size_t size, DWORD& protection)
{
	VirtualProtect(ptr, size, protection, &protection);
}

void ChangeBytesAtAddr(PVOID addr, char bytes[], size_t size)
{
	if (size == 0) {
		size = sizeof(bytes)/sizeof(char);
	}

	if ((PBYTE)addr == (PBYTE)0xFFFFFFFE)
	{
		MessageBoxA(nullptr, "Адрес не действительный!", "Критическая ошибка!", MB_OK);
		ExitProcess(1);
	}
	
	DWORD protection = PAGE_EXECUTE_READWRITE;
	ChangeProtection(addr, size, protection);
	memcpy(addr, bytes, size);
	ChangeProtection(addr, size, protection);
}

