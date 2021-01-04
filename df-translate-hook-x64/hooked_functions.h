#pragma once
#include "detours.h"
#include "hook_helper.h"
#include "changetext.h"

// Structs
struct string_ {			// Структура данных экземпляра string
	union {
		char buf[16];		// Массив символов
		char* ptr;			// Указатель на строку
	};
	unsigned __int64 len;
	unsigned __int64 capa;
	__int64 pad;
};

struct SDL_Color {
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t unused;
};

struct graphicst_ {
	long screenx, screeny;
	char screenf, screenb;
	char screenbright;
};

typedef char* (__cdecl* strncpyP)(char* dest, char* source, size_t count);

typedef void(__fastcall* addcoloredst_inline)(char* str, char* colorstr);
typedef void(__fastcall* addcoloredst)(graphicst_* gps, char* str, char* colorstr);
typedef void(__fastcall* addst)(graphicst_* gps, string_* str, unsigned __int8 just, int space);

typedef char(__fastcall* standardstringentry)(string_* str_, int maxlen, unsigned int flag, char* events);
typedef __int64(__fastcall* count)(void* this_, __int64* elem);
typedef __int64(__fastcall* resize)(char* this_, unsigned __int64 len);
typedef void(__fastcall* clear)(void* this_);

typedef __int64* (__fastcall* append)(void* src, char* a2, size_t size);

typedef void(__fastcall* upper_case_string)(string_* str_);
typedef void(__fastcall* simplify_string)(string_* str_);
typedef void(__fastcall* lower_case_string)(string_* str_);
typedef void(__fastcall* capitalize_string_words)(string_* str_);
typedef void(__fastcall* capitalize_string_first_word)(string_* str_);

typedef char* (*TTF_RenderUNICODE_Blended)(char* font, const uint16_t* text, SDL_Color fg);
typedef int(*TTF_SizeUNICODE)(char* font, uint16_t* text, int* w, int* h);

void AttachFunctions();
void ReworkFunctions();
