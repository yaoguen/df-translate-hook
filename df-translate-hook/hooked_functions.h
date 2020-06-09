#pragma once
#include "detours.h"
#include "hook_helper.h"

#ifdef _M_IX86
typedef char* (__fastcall* strncpyP)(void* Dest, DWORD EDX, char* Source, int Count);
typedef void(__fastcall* addcoloredst)(char* gps, DWORD EDX, char* str, const char* colorstr);
typedef void(__thiscall* addst)(char* gps, char* str, unsigned __int8 just, int space);
#elif defined _M_X64
typedef char* (__cdecl* strncpyP)(char* Dest, char* Source, size_t Count);
typedef void(__fastcall* addcoloredst_inline)(char* str, char* colorstr);
typedef void(__fastcall* addcoloredst)(char* gps, char* str, char* colorstr);
typedef void(__fastcall* addst)(char* gps, char* str, unsigned __int8 just, int space);
#endif // _M_IX86

void AttachFunctions();
void ReworkFunctions();
