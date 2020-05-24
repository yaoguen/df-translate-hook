#include "pch.h"
#include "hooked_functions.h"

// Set address function from df.exe
#if defined _M_IX86
	SETUP_ORIG_FUNC(strncpyP, 0xa130);
#elif defined _M_X64
	SETUP_ORIG_FUNC(strncpyP, 0xC780);
#endif // _M_IX86


#if defined _M_IX86
char* __fastcall h(strncpyP)(void* Dest, DWORD EDX, char* Source, int Count)
{
	if (DictSearch(Source)) {
		Count = strlen(Source);
		printf("%s\n", Source);
	}
	
	return o(strncpyP)(Dest, EDX, Source, Count);
}
#elif defined _M_X64
char* __cdecl h(strncpyP)(char* Dest, char* Source, int Count)
{
	if (DictSearch(Source)) {
		Count = strlen(Source);
		printf("%s\n", Source);
	}
	return o(strncpyP)(Dest, Source, Count);
}
#endif // _M_IX86


void AttachFunctions()
{
	ATTACH(strncpyP);
}
