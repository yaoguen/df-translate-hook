#include "pch.h"
#include "hooked_functions.h"
#include "keybindings.h"

// Set address function from df.exe
SETUP_ORIG_FUNC(strncpyP, 0xa130);
char* __fastcall h(strncpyP)(void* dest, const DWORD EDX, char* source, int count)
{
	if (DictSearch(source)) {
		count = strlen(source);
	}
	
	return o(strncpyP)(dest, EDX, source, count);
}


SETUP_ORIG_FUNC(addcoloredst, 0x5E5BB0);
SETUP_ORIG_FUNC(addst, 0x5E5C30);
void __fastcall h(addcoloredst)(graphicst_* gps, DWORD EDX, char* str, const char* colorstr)
{
	string_ string{};
	const size_t slen = strlen(str);
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


SETUP_ORIG_FUNC(clear_, 0x5A770);
void clear(set_* events) {
	o(clear_)(events, (set_*)events->begin->ptr);
	events->begin->ptr = events->begin;
	events->begin->begin = events->begin;
	events->begin->end = events->begin;
	events->ptr = nullptr;
}


SETUP_ORIG_FUNC(resize_, 0x3F6A0);
void resize(string_* str, int size, char let) {
	if (size > str->len) {
		o(resize_)(str, size - str->len, 0);
	}
	else {
		str->len = size;
		if (str->capa >= 16) {
			str->ptr[size] = 0;
		}
		else {
			str->buf[size] = 0;
		}
	}
}


// WARNING!!! If you change the standardstringentry function,
// you must correct the offset addresses in ReworkFunctions.
SETUP_ORIG_FUNC(count, 0x5A680);
SETUP_ORIG_FUNC(standardstringentry, 0x698AF0);
char __fastcall h(standardstringentry)(string_* str, int maxlen, unsigned int flag, set_* events)
{
	char* str_i = str->capa >= 16 ? str->ptr : str->buf;

	unsigned char entry = 1;
	unsigned char cont;
	int count_arg;
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
			if (str->len > 0) resize(str, str->len - 1, 0);
		}
		else
		{
			int cursor = str->len;
			if (cursor >= maxlen) cursor = maxlen - 1;
			if (cursor < 0) cursor = 0;

			if (str->len < cursor + 1) resize(str, cursor + 1, 0);

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

		clear(events);
		return 1;
	}
	return 0;
}


SETUP_ORIG_FUNC(append, 0xA340);
int* __fastcall h(append)(void* src, DWORD EDX, char* text, size_t size) {
	if (DictSearch(text)) {
		size = strlen(text);
	}
	return o(append)(src, EDX, text, size);
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


SETUP_ORIG_FUNC(upper_case_string, 0x11DC60);
void __fastcall h(upper_case_string)(string_* str_)
{
	__asm { push ecx }

	char* str = str_->capa >= 16 ? str_->ptr : str_->buf;

	int32_t s;
	for (s = 0;s < str_->len;s++) {
		//CAPITALIZE
		Capitalize(str[s]);

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

	__asm { pop ecx }
}


SETUP_ORIG_FUNC(simplify_string, 0x11D940);
void __fastcall h(simplify_string)(string_* str_)
{
	__asm { push ecx }

	char* str = str_->capa >= 16 ? str_->ptr : str_->buf;
	int32_t s;
	for (s = 0;s < str_->len;s++)
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

	__asm { pop ecx }
}


SETUP_ORIG_FUNC(lower_case_string, 0x11DAD0);
void __fastcall h(lower_case_string)(string_* str_)
{
	__asm { push ecx }

	char* str = str_->capa >= 16 ? str_->ptr : str_->buf;
	int32_t s;
	for (s = 0;s < str_->len;s++)
	{
		// lovercast
		LowerCast(str[s]);

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

	__asm { pop ecx }
}


SETUP_ORIG_FUNC(capitalize_string_words, 0x11DDE0);
void __fastcall h(capitalize_string_words)(string_* str_)
{
	__asm { push ecx }

	char* str = str_->capa >= 16 ? str_->ptr : str_->buf;
	char conf;
	int32_t s;
	for (s = 0;s < str_->len;s++)
	{
		conf = 0;
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

	__asm { pop ecx }
}


SETUP_ORIG_FUNC(capitalize_string_first_word, 0x11DFE0);
void __fastcall h(capitalize_string_first_word)(string_* str_)
{
	char* str = str_->capa >= 16 ? str_->ptr : str_->buf;
	for (int32_t s = 0;s < str_->len;s++)
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
			case (char)129:str[s] = (char)154;return;
			case (char)164:str[s] = (char)165;return;
			case (char)132:str[s] = (char)142;return;
			case (char)134:str[s] = (char)143;return;
			case (char)130:str[s] = (char)144;return;
			case (char)148:str[s] = (char)153;return;
			case (char)135:str[s] = (char)128;return;
			case (char)145:str[s] = (char)146;return;
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
	ATTACH(append);

	ATTACH(standardstringentry);

	ATTACH(simplify_string);
	ATTACH(lower_case_string);
	ATTACH(upper_case_string);
	ATTACH(capitalize_string_words);
	ATTACH(capitalize_string_first_word);

	ATTACH(TTF_RenderUNICODE_Blended);
	//ATTACH(TTF_SizeUNICODE);
	//printf("%p\n", o(TTF_SizeUNICODE));
}

void ReworkFunctions()
{
	// Fix return for standardstringentry function
	Sleep(10);
	char buf[] = { 0xC3,	0x90, 0x90 };	// ret
	// WARNING!!! If you change the standardstringentry function, you must correct the offset addresses.
	ChangeBytesAtAddr((char*)h(standardstringentry)+0x36D, buf, 3);
}
