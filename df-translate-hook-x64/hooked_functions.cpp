#include "pch.h"
#include "hooked_functions.h"
#include "keybindings.h"

// Set address function from df.exe

SETUP_ORIG_FUNC(strncpyP, 0xC780);
char* __cdecl h(strncpyP)(char* dest, char* source, size_t count)
{
	if (DictSearch(source)) {
		count = strlen(source);
	}
	return o(strncpyP)(dest, source, count);
}

SETUP_ORIG_FUNC(addst, 0x6EF010);
SETUP_ORIG_FUNC(addcoloredst, 0x6EEF50);  // Искать по строке "REC"
void __fastcall h(addcoloredst)(graphicst_* gps, char* str, char* colorstr)
{
	string_ string{};
	const unsigned int slen = strlen(str);
	string.len = slen;

	if (slen >= 16) {
		string.capa = slen;
		string.ptr = str;
	}
	else {
		string.capa = 15;
		memcpy(string.buf, str, 16);
	}

	gps->screenf = colorstr[0] & 7;
	gps->screenb = (colorstr[0] & 56) >> 3;
	gps->screenbright = (colorstr[0] & 64) >> 6;

	o(addst)(gps, &string, 0, 0);
}

SETUP_ORIG_FUNC(addcoloredst_inline, 0x977D99);		// Искать по строке "Text generation failed: "
void __fastcall h(addcoloredst_inline)(char* str, char* colorstr)
{
	graphicst_* gps = (graphicst_*)GET_ADDR(0x1271220);
	h(addcoloredst)(gps, str, colorstr);
}

SETUP_ORIG_FUNC(count, 0x71050);
SETUP_ORIG_FUNC(resize, 0x713A0);
SETUP_ORIG_FUNC(clear, 0x710E0);
SETUP_ORIG_FUNC(standardstringentry, 0x7CF640);
char h(standardstringentry)(string_* str, int maxlen, unsigned int flag, void* events)
{
	char* str_i = str->capa >= 16 ? str->ptr : str->buf;

	unsigned char entry = 1;
	unsigned char cont;
	__int64 count_arg;
	unsigned short int item;
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
	if (flag & STRINGENTRY_LETTERS)
	{
		count_arg = INTERFACEKEY_STRING_A168;
		if (o(count)(events, &count_arg)) entry = 168;//Ё
		count_arg = INTERFACEKEY_STRING_A184;
		if (o(count)(events, &count_arg)) entry = 184;//ё
		cont = (BYTE)'А'; // cyrillic A
		for (item = INTERFACEKEY_STRING_A192; item <= INTERFACEKEY_STRING_A255; item++)//все русские буквы
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
		for (item = INTERFACEKEY_STRING_A097; item <= INTERFACEKEY_STRING_A122; item++) // Маленькие английские буквы
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
		for (item = INTERFACEKEY_STRING_A065; item <= INTERFACEKEY_STRING_A090; item++) // Большие английские буквы
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
		cont = (BYTE)'0';
		for (item = INTERFACEKEY_STRING_A048; item <= INTERFACEKEY_STRING_A057; item++) // Цифры
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

	if (entry != 1)
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
				if (entry >= (BYTE)'а' && entry <= (BYTE)'я') {
					entry += (BYTE)'А' - (BYTE)'а';
				}
				if (entry == (BYTE)'ё') {
					entry += (BYTE)'Ё' - (BYTE)'ё';
				}
			}
			str_i[cursor] = entry;
		}
		o(clear)(events);

		return 1;
	}
	return 0;
}

SETUP_ORIG_FUNC(append, 0xC9F0);
__int64* __fastcall h(append)(void* src, char* text, size_t size) {
	if (DictSearch(text)) {
		size = strlen(text);
	}
	return o(append)(src, text, size);
}

void Capitalize(char& s)
{
	if (s >= 'a' && s <= 'z')
	{
		s -= 'a';
		s += 'A';
	}
	if ((BYTE)s >= (BYTE)'а' && (BYTE)s <= (BYTE)'я')
	{
		s -= (BYTE)'а';
		s += (BYTE)'А';
	}
	if ((BYTE)s == (BYTE)'ё')
	{
		s -= (BYTE)'ё';
		s += (BYTE)'Ё';
	}
}

void LowerCast(char& s) {
	if (s >= 'A' && s <= 'Z')
	{
		s -= 'A';
		s += 'a';
	}
	if ((BYTE)s >= (BYTE)'А' && (BYTE)s <= (BYTE)'Я')
	{
		s -= (BYTE)'А';
		s += (BYTE)'а';
	}
	if ((BYTE)s == (BYTE)'Ё')
	{
		s -= (BYTE)'Ё';
		s += (BYTE)'ё';
	}
}


SETUP_ORIG_FUNC(upper_case_string, 0x14B240);
void __fastcall h(upper_case_string)(string_* str_)
{
	char* str = str_->capa >= 16 ? str_->ptr : str_->buf;

	for (int s = 0; s < str_->len; s++) {
		//CAPITALIZE
		Capitalize(str[s]);

		switch (str[s]) {
		case (char)129:str[s] = (char)154; break;
		case (char)164:str[s] = (char)165; break;
		case (char)132:str[s] = (char)142; break;
		case (char)134:str[s] = (char)143; break;
		case (char)130:str[s] = (char)144; break;
		case (char)148:str[s] = (char)153; break;
		case (char)135:str[s] = (char)128; break;
		case (char)145:str[s] = (char)146; break;
		}
	}
}


SETUP_ORIG_FUNC(simplify_string, 0x14AE60);
void __fastcall h(simplify_string)(string_* str_)
{
	char* str = str_->capa >= 16 ? str_->ptr : str_->buf;
	for (int s = 0; s < str_->len; s++)
	{
		//CAPITALIZE
		LowerCast(str[s]);

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
void __fastcall h(lower_case_string)(string_* str_)
{
	char* str = str_->capa >= 16 ? str_->ptr : str_->buf;
	for (int s = 0; s < str_->len; s++)
	{
		// lovercast
		LowerCast(str[s]);

		switch (str[s])
		{
		case (char)154:str[s] = (char)129; break;
		case (char)165:str[s] = (char)164; break;
		case (char)142:str[s] = (char)132; break;
		case (char)143:str[s] = (char)134; break;
		case (char)144:str[s] = (char)130; break;
		case (char)153:str[s] = (char)148; break;
		case (char)128:str[s] = (char)135; break;
		case (char)146:str[s] = (char)145; break;
		}
	}
}


SETUP_ORIG_FUNC(capitalize_string_words, 0x14B430);
void __fastcall h(capitalize_string_words)(string_* str_)
{
	char* str = str_->capa >= 16 ? str_->ptr : str_->buf;
	for (int s = 0; s < str_->len; s++)
	{
		char conf = 0;
		if (s > 0)
		{
			if (str[s - 1] == ' ' ||
				str[s - 1] == '\"')conf = 1;
			if (str[s - 1] == '\'')
			{
				//DISCOUNT SINGLE QUOTE IF IT ISN'T PRECEDED BY SPACE, COMMA OR NOTHING
				if (s <= 0)conf = 1;
				else if (s >= 2)
				{
					if (str[s - 2] == ' ' ||
						str[s - 2] == ',')conf = 1;
				}
			}
		}
		if (s == 0 || conf)
		{
			//CAPITALIZE
			Capitalize(str[s]);

			switch (str[s])
			{
			case (char)129:str[s] = (char)154; break;
			case (char)164:str[s] = (char)165; break;
			case (char)132:str[s] = (char)142; break;
			case (char)134:str[s] = (char)143; break;
			case (char)130:str[s] = (char)144; break;
			case (char)148:str[s] = (char)153; break;
			case (char)135:str[s] = (char)128; break;
			case (char)145:str[s] = (char)146; break;
			}
		}
	}
}


SETUP_ORIG_FUNC(capitalize_string_first_word, 0x14B6B0);
void __fastcall h(capitalize_string_first_word)(string_* str_)
{
	char* str = str_->capa >= 16 ? str_->ptr : str_->buf;
	for (int s = 0; s < str_->len; s++)
	{
		char conf = 0;
		if (s > 0)
		{
			if (str[s - 1] == ' ' ||
				str[s - 1] == '\"')conf = 1;
			if (str[s - 1] == '\'')
			{
				//DISCOUNT SINGLE QUOTE IF IT ISN'T PRECEDED BY SPACE, COMMA OR NOTHING
				if (s <= 0)conf = 1;
				else if (s >= 2)
				{
					if (str[s - 2] == ' ' ||
						str[s - 2] == ',')conf = 1;
				}
			}
		}
		if (s == 0 || conf)
		{
			//CAPITALIZE
			if (str[s] >= 'a' && str[s] <= 'z')
			{
				str[s] -= 'a';
				str[s] += 'A';
				return;
			}
			if ((BYTE)str[s] >= (BYTE)'а' && (BYTE)str[s] <= (BYTE)'я')
			{
				str[s] -= (BYTE)'а';
				str[s] += (BYTE)'А';
				return;
			}
			if ((BYTE)str[s] == (BYTE)'ё')
			{
				str[s] -= (BYTE)'ё';
				str[s] += (BYTE)'Ё';
				return;
			}

			switch (str[s])
			{
			case (char)129:str[s] = (char)154; return;
			case (char)164:str[s] = (char)165; return;
			case (char)132:str[s] = (char)142; return;
			case (char)134:str[s] = (char)143; return;
			case (char)130:str[s] = (char)144; return;
			case (char)148:str[s] = (char)153; return;
			case (char)135:str[s] = (char)128; return;
			case (char)145:str[s] = (char)146; return;
			}
			if (str[s] != ' ' && str[s] != '\"')return;
		}
	}
}


SETUP_ORIG_FUNC_FNAME(TTF_RenderUNICODE_Blended, SDL_ttf.dll);
char* h(TTF_RenderUNICODE_Blended)(char* font, uint16_t* text, SDL_Color fg) {
	uint16_t* x = ChangeText(text);
	return o(TTF_RenderUNICODE_Blended)(font, x ? x : text, fg);
}

SETUP_ORIG_FUNC_FNAME(TTF_SizeUNICODE, SDL_ttf.dll);
int h(TTF_SizeUNICODE)(char* font, uint16_t* text, int* width, int* height) {
	uint16_t* x = ChangeText(text);
	return o(TTF_SizeUNICODE)(font, x ? x : text, width, height);
}


void AttachFunctions()
{
	ATTACH(strncpyP);
	ATTACH(addcoloredst);
	ATTACH(addcoloredst_inline);

	ATTACH(append);

	ATTACH(standardstringentry);
	printf("%p\n", o(standardstringentry));

	ATTACH(simplify_string);
	ATTACH(lower_case_string);
	ATTACH(upper_case_string);
	ATTACH(capitalize_string_words);
	ATTACH(capitalize_string_first_word);

	ATTACH(TTF_RenderUNICODE_Blended);
	printf("%p\n", o(TTF_RenderUNICODE_Blended));
	//ATTACH(TTF_SizeUNICODE);
	//printf("%p\n", o(TTF_SizeUNICODE));
}

void ReworkFunctions()
{
	//Fix inline addcoloredst 
	Sleep(10);
	PVOID addr;
	addr = GET_ADDR(0x977D91);
	char b1[] = { 0x51,					// push rcx
		0x48, 0x8B, 0xCE,				// mov rcx, rsi
		0x48, 0x8D, 0x55, 0x90, 0xE8 	// lea rdx, ss:[rbp-0x70]
	};
	ChangeBytesAtAddr(addr, b1, 9);

	addr = GET_ADDR(0x977D9E);
	char b2[] = { 0x59,					// pop rcx
		0xEB, 0x6F						// jmp $+0x6F
	};
	ChangeBytesAtAddr(addr, b2, 3);
}
