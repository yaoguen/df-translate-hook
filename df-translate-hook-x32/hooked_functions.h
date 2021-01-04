#pragma once
#include "detours.h"
#include "hook_helper.h"
#include "changetext.h"

// Structs
struct string_ {					// Структура данных экземпляра string
	union {
		char buf[16];		// Либо массив символов
		char* ptr;			// Либо указатель на строку
		};
	unsigned int len;
	unsigned int capa;
	int pad;
	};

struct set_ {				// Структура данных экземпляра std::set
	struct set_* begin;
	void* ptr;
	struct set_* end;
	__int8 pad13;
	__int8 endP;			// Определяет конец set
};

struct graphicst_ {
	long screenx, screeny;
	char screenf, screenb;
	char screenbright;
};

struct SDL_Color {
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t unused;
};


typedef char* (__fastcall* strncpyP)(void* dest, DWORD EDX, char* source, int count);
typedef void(__fastcall* addcoloredst)(graphicst_* gps, DWORD EDX, char* str, const char* colorstr);
typedef void(__thiscall* addst)(graphicst_* gps, string_* str, unsigned __int8 just, int space);
typedef int* (__fastcall* append)(void* src, DWORD EDX, char* text, size_t size);

typedef int(__thiscall* count)(set_* this_, int* elem);
typedef void(__thiscall* clear_)(set_* begin, set_* end);
typedef void(__thiscall* resize_)(void* this_, unsigned int a2, char a3);
typedef char(__fastcall* standardstringentry)(string_* str_, int maxlen, unsigned int flag, char* events);

typedef void(__fastcall* upper_case_string)(string_* str_);
typedef void(__fastcall* simplify_string)(string_* str_);
typedef void(__fastcall* lower_case_string)(string_* str_);
typedef void(__fastcall* capitalize_string_words)(string_* str_);
typedef void(__fastcall* capitalize_string_first_word)(string_* str_);

typedef char* (*TTF_RenderUNICODE_Blended)(char* font, const uint16_t* text, SDL_Color fg);
typedef int(*TTF_SizeUNICODE)(char* font, uint16_t* text, int* w, int* h);

void AttachFunctions();
void ReworkFunctions();
