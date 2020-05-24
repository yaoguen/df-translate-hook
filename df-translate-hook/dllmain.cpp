// dllmain.cpp : Определяет точку входа для приложения DLL.
#include "pch.h"
#include "detours.h"

#ifdef _M_IX86
#pragma comment (lib, "lib.X86/detours.lib")
#elif _M_X64
#pragma comment (lib, "lib.X64/detours.lib")
#endif // _M_IX86



// Fix export ordinal #1
extern "C" __declspec(dllexport)VOID NullExport(VOID)
{
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
	{
		DetourRestoreAfterWith();
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());

		DetourTransactionCommit();
	}
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

