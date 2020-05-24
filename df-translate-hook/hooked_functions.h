#pragma once
#include "detours.h"
#include "hook_helper.h"

#ifdef _M_IX86
typedef char* (__fastcall* strncpyP)(void* Dest, DWORD EDX, char* Source, int Count);
#elif defined _M_X64
typedef char* (__cdecl* strncpyP)(char* Dest, char* Source, size_t Count);
#endif // _M_IX86

void AttachFunctions();
