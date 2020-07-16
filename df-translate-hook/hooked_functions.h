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
typedef char(__fastcall* standardstringentry)(char* str_, int maxlen, unsigned int flag, char* events);
typedef __int64(__fastcall* count)(char* this_, __int64* elem);
typedef __int64(__fastcall* resize)(char* this_, unsigned __int64 len);
typedef void(__fastcall* clear)(char* this_);
#endif // _M_IX86

// Structs
#ifdef _M_IX86
	struct string_ {					// Структура данных экземпляра string
		union {
			char buf[16];		// Либо массив символов
			char* ptr;			// Либо указатель на строку
		};
		unsigned int len;
		unsigned int capa;
		int pad;
	};
#elif defined _M_X64
	struct string_ {			// Структура данных экземпляра string
		union {
			char buf[16];		// Массив символов
			char* ptr;			// Указатель на строку
		};
		unsigned __int64 len;
		unsigned __int64 capa;
		__int64 pad;
	};
#endif // _M_IX86

void AttachFunctions();
void ReworkFunctions();
