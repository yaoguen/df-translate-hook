#include "pch.h"
#include "hooked_functions.h"

// Set address function from df.exe

#if defined _M_IX86
	SETUP_ORIG_FUNC(strncpyP, 0xa130);
	char* __fastcall h(strncpyP)(void* Dest, DWORD EDX, char* Source, int Count)
	{
		if (DictSearch(Source)) {
			Count = strlen(Source);
		}
	
		return o(strncpyP)(Dest, EDX, Source, Count);
	}

	SETUP_ORIG_FUNC(addcoloredst, 0x5E5620);
	SETUP_ORIG_FUNC(addst, 0x5e56a0);
	void __fastcall h(addcoloredst)(char* gps, DWORD EDX, char* str, const char* colorstr)
	{
		struct {					// Структура данных string
			union {
				char buf[16];		// Либо массив символов
				char* ptr;			// Либо указатель на строку
			};
			unsigned int len;
			unsigned int capa;
			int pad;
		} string;

		unsigned int slen = strlen(str);
		string.len = slen;

		if (slen >= 16) {
			string.capa = slen;
			string.ptr = str;
		}
		else {
			string.capa = 15;
			memcpy(string.buf, str, 16);
		}

		gps[8] = (colorstr[0] & 7);
		gps[9] = ((colorstr[0] & 56)) >> 3;
		gps[10] = ((colorstr[0] & 64)) >> 6;

		o(addst)(gps, (char*)&string, 0, 0);
	}
#elif defined _M_X64
	SETUP_ORIG_FUNC(strncpyP, 0xC780);
	char* __cdecl h(strncpyP)(char* Dest, char* Source, int Count)
	{
		if (DictSearch(Source)) {
			Count = strlen(Source);
		}
		return o(strncpyP)(Dest, Source, Count);
	}

	SETUP_ORIG_FUNC(addst, 0x6EEB50);
	SETUP_ORIG_FUNC(addcoloredst, 0x6EEA90);  // Искать по строке "REC"
	void __fastcall h(addcoloredst)(char* gps, char* str, char* colorstr)
	{
		struct {					// Структура данных string
			union {
				char buf[16];		// Либо массив символов
				char* ptr;			// Либо указатель на строку
			};
			unsigned __int64 len;
			unsigned __int64 capa;
			__int64 pad;
		} string;

		unsigned int slen = strlen(str);
		string.len = slen;

		if (slen >= 16) {
			string.capa = slen;
			string.ptr = str;
		}
		else {
			string.capa = 15;
			memcpy(string.buf, str, 16);
		}

		gps[8] = (colorstr[0] & 7);
		gps[9] = ((colorstr[0] & 56)) >> 3;
		gps[10] = ((colorstr[0] & 64)) >> 6;

		o(addst)(gps, (char*)&string, 0, 0);
	}

	SETUP_ORIG_FUNC(addcoloredst_inline, 0x976F99);		// Искать по строке "Text generation failed: "
	void __fastcall h(addcoloredst_inline)(char* str, char* colorstr)
	{
		char* gps = (char*)GET_ADDR(0x126F220);
		h(addcoloredst)(gps, str, colorstr);
	}

#endif // _M_IX86

void AttachFunctions()
{
	ATTACH(strncpyP);
	ATTACH(addcoloredst);

#ifdef _M_X64
	ATTACH(addcoloredst_inline);
#endif // _M_X64
}

void ReworkFunctions()
{
	//Fix inline addcoloredst 
#ifdef _M_X64
	Sleep(10);
	PVOID addr;
	addr = GET_ADDR(0x976F91);
	char b1[] = { 0x51,					// push rcx
		0x48, 0x8B, 0xCE,				// mov rcx, rsi
		0x48, 0x8D, 0x55, 0x90, 0xE8 	// lea rdx, ss:[rbp-0x70]
	};
	ChangeBytesAtAddr(addr, b1, 9);

	addr = GET_ADDR(0x976F9E);
	char b2[] = { 0x59,					// pop rcx
		0xEB, 0x6F						// jmp $+0x6F
	};
	ChangeBytesAtAddr(addr, b2, 3);
#endif // _M_X64
}
