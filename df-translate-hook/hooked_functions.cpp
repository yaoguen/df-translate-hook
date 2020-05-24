#include "pch.h"
#include "hooked_functions.h"

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
