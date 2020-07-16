#pragma once
#include <Windows.h>
#include <map>
#include <string>
#include <cstdio>

using std::string;
using std::map;

#define DEBUG 1    //Turn on debug printing 1 - enable; 0 - disable

#define SETUP_ORIG_FUNC(nameFunc, shift) \
	nameFunc nameFunc##_orig = (nameFunc)((UINT64)GetModuleHandle(0) + shift);

#define GET_ADDR(shift) \
	(PVOID)((UINT64)GetModuleHandle(0) + shift)

#define ATTACH(nameFunc) \
	DetourAttach(&(PVOID&)(nameFunc##_orig), (PVOID)nameFunc##_hook)

#define h(nameFunc) nameFunc##_hook
#define o(nameFunc) nameFunc##_orig

void FillDictionary();
BOOL DictSearch(char*& text);
void CreateDebugConsole();

inline static void ChangeProtection(void* ptr, size_t size, DWORD& protection);
void ChangeBytesAtAddr(PVOID pFunc, char bytes[], size_t size = 0);
