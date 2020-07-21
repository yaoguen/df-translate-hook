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
	char* __cdecl h(strncpyP)(char* Dest, char* Source, size_t Count)
	{
		if (DictSearch(Source)) {
			Count = strlen(Source);
		}
		return o(strncpyP)(Dest, Source, Count);
	}

	SETUP_ORIG_FUNC(addst, 0x6EEB50);
	SETUP_ORIG_FUNC(addcoloredst, 0x6EEA90);  // ������ �� ������ "REC"
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

	SETUP_ORIG_FUNC(addcoloredst_inline, 0x976F99);		// ������ �� ������ "Text generation failed: "
	void __fastcall h(addcoloredst_inline)(char* str, char* colorstr)
	{
		char* gps = (char*)GET_ADDR(0x126F220);
		h(addcoloredst)(gps, str, colorstr);
	}

	SETUP_ORIG_FUNC(count, 0x71050);
	SETUP_ORIG_FUNC(resize, 0x713A0);
	SETUP_ORIG_FUNC(clear, 0x710E0);
	SETUP_ORIG_FUNC(standardstringentry, 0x7CF0C0);
	char h(standardstringentry)(string_* str, int maxlen, unsigned int flag, char * events)
	{
		char* str_i = str->capa >= 16 ? str->ptr : str->buf;

		unsigned char entry = 255;
		unsigned char cont;
		__int64 count_arg;
		unsigned short int item;
		if (flag & STRINGENTRY_LETTERS)
		{
			count_arg = INTERFACEKEY_STRING_A168;
			if (o(count)(events, &count_arg)) entry = 168;//�
			count_arg = INTERFACEKEY_STRING_A184;
			if (o(count)(events, &count_arg)) entry = 184;//�
			cont = (BYTE)'�'; // cyrillic A
			for (item = INTERFACEKEY_STRING_A192; item <= INTERFACEKEY_STRING_A255+5; item++)//��� ������� �����
			{
				count_arg = item;
				if (o(count)(events, &count_arg))
				{
					entry = cont;
					break;
				}
				cont++;
			}
			cont = 'a';
			for (item = INTERFACEKEY_STRING_A097; item <= INTERFACEKEY_STRING_A122; item++) // ��������� ���������� �����
			{
				count_arg = item;
				if (o(count)(events, &count_arg))
				{
					entry = cont;
					break;
				}
				cont++;
			}
			cont = 'A';
			for (item = INTERFACEKEY_STRING_A065; item <= INTERFACEKEY_STRING_A090; item++) // ������� ���������� �����
			{
				count_arg = item;
				if (o(count)(events, &count_arg)) {
					entry = cont;
					break;
				}
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
			for (item = INTERFACEKEY_STRING_A048; item <= INTERFACEKEY_STRING_A057; item++) // �����
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
				if (o(count)(events, &count_arg))
				{
					entry = cont;
					break;
				}
				cont++;
			}
		}

		if (entry != 255)
		{
			if (entry == '\x0')
			{
				if (str->len > 0) o(resize)((char*)str, str->len - 1);
			}
			else
			{
				int cursor = str->len;
				if (cursor >= maxlen) cursor = maxlen - 1;
				if (cursor < 0) cursor = 0;

				if (str->len < (__int64)cursor + 1) o(resize)((char*)str, (__int64)cursor + 1);

				if (flag & STRINGENTRY_CAPS)
				{
					if (entry >= 'a' && entry <= 'z') {
						entry += 'A' - 'a';
					}
					/*if (entry >= (BYTE)'�' && entry <= (BYTE)'�') {
						entry += (BYTE)'�' - (BYTE)'�';
					}
					if (entry == (BYTE)'�') {
						entry += (BYTE)'�' - (BYTE)'�';
					}*/
				}
				str_i[cursor] = entry;
			}
			o(clear)(events);

			return 1;
		}
		return 0;
	}

	SETUP_ORIG_FUNC(upper_case_string, 0x14B240);
	void h(upper_case_string)(string_* str_)
	{
		char* str = str_->capa >= 16 ? str_->ptr : str_->buf;

		int32_t s;
		for (s = 0;s < str_->len;s++) {
			//CAPITALIZE
			if (str[s] >= 'a' && str[s] <= 'z') {
				str[s] -= 'a' - 'A';
			}
			if (str[s] >= (BYTE)'�' && str[s] <= (BYTE)'�') {
				str[s] -= (BYTE)'a' - (BYTE)'�';
			}
			if (str[s] == (BYTE)'�') {
				str[s] -= (BYTE)'�' - (BYTE)'�';
			}

			switch (str[s]) {
				case (char)129:str[s] = (char)154;break;
				case (char)164:str[s] = (char)165;break;
				case (char)132:str[s] = (char)142;break;
				case (char)134:str[s] = (char)143;break;
				case (char)130:str[s] = (char)144;break;
				case (char)148:str[s] = (char)153;break;
				case (char)135:str[s] = (char)128;break;
				case (char)145:str[s] = (char)146;break;
			}
		}
	}

	SETUP_ORIG_FUNC(simplify_string, 0x14AE60);
	void h(simplify_string)(string_* str_)
	{
		char* str = str_->capa >= 16 ? str_->ptr : str_->buf;
		int32_t s;
		for (s = 0;s < str_->len;s++)
		{
			//CAPITALIZE
			if (str[s] >= 'A' && str[s] <= 'Z')
			{
				str[s] -= 'A';
				str[s] += 'a';
			}
			if ((BYTE)str[s] >= (BYTE)'�' && (BYTE)str[s] <= (BYTE)'�')
			{
				str[s] -= (BYTE)'�';
				str[s] += (BYTE)'�';
			}
			if ((BYTE)str[s] == (BYTE)'�')
			{
				str[s] -= (BYTE)'�';
				str[s] += (BYTE)'�';
			}

			switch (str[s])
			{
				case (char)129:
				case (char)150:
				case (char)151:
				case (char)154:
				case (char)163:
					str[s] = 'u';
					break;
				case (char)152:
					str[s] = 'y';
					break;
				case (char)164:
				case (char)165:
					str[s] = 'n';
					break;
				case (char)131:
				case (char)132:
				case (char)133:
				case (char)134:
				case (char)142:
				case (char)143:
				case (char)145:
				case (char)146:
				case (char)160:
					str[s] = 'a';
					break;
				case (char)130:
				case (char)136:
				case (char)137:
				case (char)138:
				case (char)144:
					str[s] = 'e';
					break;
				case (char)139:
				case (char)140:
				case (char)141:
				case (char)161:
					str[s] = 'i';
					break;
				case (char)147:
				case (char)148:
				case (char)149:
				case (char)153:
				case (char)162:
					str[s] = 'o';
					break;
				case (char)128:
				case (char)135:
					str[s] = 'c';
					break;
			}
		}
	}
	
	SETUP_ORIG_FUNC(lower_case_string, 0x14B050);
	void h(lower_case_string)(string_* str_)
	{
		char* str = str_->capa >= 16 ? str_->ptr : str_->buf;
		int32_t s;
		for (s = 0;s < str_->len;s++)
		{
			//CAPITALIZE
			if (str[s] >= 'A' && str[s] <= 'Z')
			{
				str[s] -= 'A';
				str[s] += 'a';
			}
			if ((BYTE)str[s] >= (BYTE)'�' && (BYTE)str[s] <= (BYTE)'�')
			{
				str[s] -= (BYTE)'�';
				str[s] += (BYTE)'�';
			}
			if ((BYTE)str[s] == (BYTE)'�')
			{
				str[s] -= (BYTE)'�';
				str[s] += (BYTE)'�';
			}

			switch (str[s])
			{
			case (char)154:str[s] = (char)129;break;
			case (char)165:str[s] = (char)164;break;
			case (char)142:str[s] = (char)132;break;
			case (char)143:str[s] = (char)134;break;
			case (char)144:str[s] = (char)130;break;
			case (char)153:str[s] = (char)148;break;
			case (char)128:str[s] = (char)135;break;
			case (char)146:str[s] = (char)145;break;
			}
		}
	}

	SETUP_ORIG_FUNC(append, 0xC9F0);
	__int64* __fastcall h(append)(void* Src, char* text, size_t Size) {
		if (DictSearch(text)) {
			Size = strlen(text);
		}
		return o(append)(Src, text, Size);
	}

#endif // _M_IX86

void AttachFunctions()
{
	ATTACH(strncpyP);
	printf("%p\n", o(strncpyP));
	ATTACH(addcoloredst);
	printf("%p\n", o(addcoloredst));


#ifdef _M_X64
	ATTACH(addcoloredst_inline);
	ATTACH(standardstringentry);
	printf("%p\n", o(standardstringentry));

	ATTACH(upper_case_string);
	printf("%p\n", o(upper_case_string));

	printf("Hi %p\n", ((UINT64)GetModuleHandle(0) + 0x457D10));
	printf("Hi %p\n", ((UINT64)GetModuleHandle(0) + 0x613E60));

	ATTACH(simplify_string);
	printf("%p\n", o(simplify_string));
	ATTACH(lower_case_string);

	ATTACH(append);
	printf("%p\n", o(append));

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
