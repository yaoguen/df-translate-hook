#pragma once
#include "detours.h"
#include "hook_helper.h"

typedef char* (__cdecl* strncpyP)(char* Dest, char* Source, size_t Count);

void AttachFunctions();
