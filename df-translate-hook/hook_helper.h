#pragma once

#define SETUP_ORIG_FUNC(nameFunc, shift) \
	nameFunc nameFunc##_orig = (nameFunc)((UINT64)GetModuleHandle(0) + shift);

#define ATTACH(nameFunc) \
	DetourAttach(&(PVOID&)(nameFunc##_orig), (PVOID)nameFunc##_hook)

#define h(nameFunc) nameFunc##_hook
#define o(nameFunc) nameFunc##_orig
