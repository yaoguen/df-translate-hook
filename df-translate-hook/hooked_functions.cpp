#include "pch.h"
#include "hooked_functions.h"

// Set address function from df.exe
#if defined _M_IX86
SETUP_ORIG_FUNC(strncpyP, 0xC780);  //FIXME: set actual address function in x32 process
#elif defined _M_X64
SETUP_ORIG_FUNC(strncpyP, 0xC780);
#endif // _M_X64

char* __cdecl h(strncpyP)(char* Dest, char* Source, int Count)
{
	if (DictSearch(Source)) {
		Count = strlen(Source);
	}

	return o(strncpyP)(Dest, Source, Count);
}


void AttachFunctions()
{
	ATTACH(strncpyP);
}
