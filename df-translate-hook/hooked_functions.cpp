#include "pch.h"
#include "hooked_functions.h"
#include "keybindings.h"

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
		string_ string;
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
		string_ string;
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

	SETUP_ORIG_FUNC(count, 0x71050);
	SETUP_ORIG_FUNC(resize, 0x713A0);
	SETUP_ORIG_FUNC(clear, 0x710E0);
	SETUP_ORIG_FUNC(standardstringentry, 0x7CF0C0);
	char h(standardstringentry)(char* str_, int maxlen, unsigned int flag, char * events)
	{
		string_* str;
		str = (string_*)str_;

		unsigned char entry = 255;
		unsigned char cont;
		__int64 count_arg;
		unsigned short int item;
		if (flag & STRINGENTRY_LETTERS)
		{
			count_arg = INTERFACEKEY_STRING_A168;
			if (o(count)(events, &count_arg)) entry = 168;//Ё
			count_arg = INTERFACEKEY_STRING_A184;
			if (o(count)(events, &count_arg)) entry = 184;//ё
			cont = 'А'; // cyrillic A
			for (item = INTERFACEKEY_STRING_A192; item <= INTERFACEKEY_STRING_A255; item++)//все русские буквы
			{
				count_arg = item;
				if (o(count)(events, &count_arg)) entry = cont;
				cont++;
			}
			cont = 'a';
			for (item = INTERFACEKEY_STRING_A097; item <= INTERFACEKEY_STRING_A122; item++) // Маленькие английские буквы
			{
				count_arg = item;
				if (o(count)(events, &count_arg)) entry = cont;
				cont++;
			}
			cont = 'A';
			for (item = INTERFACEKEY_STRING_A065; item <= INTERFACEKEY_STRING_A090; item++) // Большие английские буквы
			{
				count_arg = item;
				if (o(count)(events, &count_arg)) entry = cont;
				cont++;
			}
		}
		if (flag & STRINGENTRY_SPACE)
		{
			count_arg = INTERFACEKEY_STRING_A032;
			if (o(count)(events, &count_arg))entry = ' ';
		}
		count_arg = INTERFACEKEY_STRING_A000;
		if (o(count)(events, &count_arg)) entry = '\x0';
		if (flag & STRINGENTRY_NUMBERS)
		{
			cont = 0;
			for (item = INTERFACEKEY_STRING_A048; item <= INTERFACEKEY_STRING_A057; item++) // Цифры
			{
				count_arg = item;
				if (o(count)(events, &count_arg))entry = cont;
				cont++;
			}
		}
		if (flag & STRINGENTRY_SYMBOLS)
		{
			cont = 0;
			for (item = INTERFACEKEY_STRING_A000; item <= INTERFACEKEY_STRING_A255; item++)
			{
				count_arg = item;
				if (o(count)(events, &count_arg)) entry = cont;
				cont++;
			}
		}

		if (entry != 255)
		{
			if (entry == '\x0')
			{
				if (str->len > 0) o(resize)(str_, str->len - 1);
			}
			else
			{
				int cursor = str->len;
				if (cursor >= maxlen) cursor = maxlen - 1;
				if (cursor < 0) cursor = 0;

				if (str->len < (__int64)cursor + 1) o(resize)(str_, (__int64)cursor + 1);

				if (flag & STRINGENTRY_CAPS)
				{
					if (entry >= 'a' && entry <= 'z') entry += 'A' - 'a';
					if (entry >= 'а' && entry <= 'я') entry += 'А' - 'а';
					if (entry == 'ё') 				  entry += 'Ё' - 'ё';
				}

				if (str->capa >= 16) {
					str->ptr[cursor] = entry;
				}
				else {
					str->buf[cursor] = entry;
				}
				
			}

			o(clear)(events);

			return 1;
		}

		return 0;
	}


#endif // _M_IX86

void AttachFunctions()
{
	ATTACH(strncpyP);
	ATTACH(addcoloredst);

#ifdef _M_X64
	ATTACH(addcoloredst_inline);
	ATTACH(standardstringentry);
	printf("%p", o(standardstringentry));
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
