#include <cstdio>
#include <stdio.h>
#include <windows.h>
#include "detours.h"

#pragma comment (lib, "detours.lib")

int main()
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	BOOL   bStatus;

	// Initialize si&pi fill with 0
	memset(&si, 0, sizeof(si));
	memset(&pi, 0, sizeof(pi));
	si.cb = sizeof(si);

	const char* cEXEPatch = "Dwarf Fortress.exe";
	const char* cDllPath = "df-translate-hook.dll";

	// Spawn a DLL injected process
	bStatus = DetourCreateProcessWithDllEx(
		cEXEPatch,		// application name 
		NULL,			// full command line + arguments 
		NULL,			// process attributes
		NULL,			// thread attributes
		FALSE,			// inherit handles
		CREATE_DEFAULT_ERROR_MODE,				// creation flags
		NULL,			// environment
		NULL,			// current directory
		&si,			// startup info
		&pi,			// process info
		cDllPath,		// path to dll to inject
		NULL);			// use standard CreateProcess API 

	// Show the result of DetourCreateProcessWithDll
	if (bStatus) {
		printf("Launched DF PID %d!\n", pi.dwProcessId);
	}
	else {
		printf("Failed to launch...\n");
	}

	return 0;
}