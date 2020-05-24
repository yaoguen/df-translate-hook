#pragma once
#include <Windows.h>
#include <map>
#include <string>
#include <cstdio>

#define DEBUG 0    //Turn on debug printing 1 - enable; 0 - disable

#define SETUP_ORIG_FUNC(nameFunc, shift) \
	nameFunc nameFunc##_orig = (nameFunc)((UINT64)GetModuleHandle(0) + shift);

#define ATTACH(nameFunc) \
	DetourAttach(&(PVOID&)(nameFunc##_orig), (PVOID)nameFunc##_hook)

#define h(nameFunc) nameFunc##_hook
#define o(nameFunc) nameFunc##_orig

void FillDictionary();
BOOL DictSearch(char*& text);
void CreateDebugConsole();
