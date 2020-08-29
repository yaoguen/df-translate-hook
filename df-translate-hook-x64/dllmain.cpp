// dllmain.cpp : Определяет точку входа для приложения DLL.
#include "pch.h"
#include "hooked_functions.h"

#pragma comment (lib, "lib/detours.lib")


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
#if DEBUG
		CreateDebugConsole();
#endif // DEBUG
		FillDictionary();

		DetourRestoreAfterWith();
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());

		AttachFunctions();
		
		DetourTransactionCommit();
		ReworkFunctions();
	}
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

