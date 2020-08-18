#pragma once
#include "detours.h"
#include "hook_helper.h"

// Structs
#ifdef _M_IX86
struct string_ {					// ��������� ������ ���������� string
	union {
		char buf[16];		// ���� ������ ��������
		char* ptr;			// ���� ��������� �� ������
		};
	unsigned int len;
	unsigned int capa;
	int pad;
	};

struct set_ {				// ��������� ������ ���������� std::set
	struct set_* begin;
	void* ptr;
	struct set_* end;
	__int8 pad13;
	__int8 endP;			// ���������� ����� set
};
#elif defined _M_X64
struct string_ {			// ��������� ������ ���������� string
	union {
		char buf[16];		// ������ ��������
		char* ptr;			// ��������� �� ������
	};
	unsigned __int64 len;
	unsigned __int64 capa;
	__int64 pad;
};
#endif // _M_IX86

#ifdef _M_IX86
typedef char* (__fastcall* strncpyP)(void* Dest, DWORD EDX, char* Source, int Count);
typedef void(__fastcall* addcoloredst)(char* gps, DWORD EDX, char* str, const char* colorstr);
typedef void(__thiscall* addst)(char* gps, char* str, unsigned __int8 just, int space);
typedef int* (__fastcall* append)(void* Src, DWORD EDX, char* text, size_t Size);

typedef int(__thiscall* count)(set_* this_, int* elem);
typedef void(__thiscall* clear_)(set_* begin, set_* end);
typedef void(__thiscall* resize_)(void* this_, unsigned int a2, char a3);
typedef char(__fastcall* standardstringentry)(string_* str_, int maxlen, unsigned int flag, char* events);
#elif defined _M_X64
typedef char* (__cdecl* strncpyP)(char* Dest, char* Source, size_t Count);

typedef void(__fastcall* addcoloredst_inline)(char* str, char* colorstr);
typedef void(__fastcall* addcoloredst)(char* gps, char* str, char* colorstr);
typedef void(__fastcall* addst)(char* gps, char* str, unsigned __int8 just, int space);

typedef char(__fastcall* standardstringentry)(string_* str_, int maxlen, unsigned int flag, char* events);
typedef __int64(__fastcall* count)(void* this_, __int64* elem);
typedef __int64(__fastcall* resize)(char* this_, unsigned __int64 len);
typedef void(__fastcall* clear)(void* this_);

typedef __int64* (__fastcall* append)(void* Src, char* a2, size_t Size);
#endif // _M_IX86

typedef void(__fastcall* upper_case_string)(string_* str_);
typedef void(__fastcall* simplify_string)(string_* str_);
typedef void(__fastcall* lower_case_string)(string_* str_);
typedef void(__fastcall* capitalize_string_words)(string_* str_);
typedef void(__fastcall* capitalize_string_first_word)(string_* str_);

void AttachFunctions();
void ReworkFunctions();
