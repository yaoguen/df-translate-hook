#pragma once
#include <Windows.h>
#include "hook_helper.h"

typedef char* (__cdecl* strncpyP)(char* Dest, char* Source, size_t Count);

// Set address function from df.exe
#if defined _M_IX86
	SETUP_ORIG_FUNC(strncpyP, 0xC780);  //FIXME: set actual address function in x32 process
#elif defined _M_X64
	SETUP_ORIG_FUNC(strncpyP, 0xC780);
#endif // _M_X64

