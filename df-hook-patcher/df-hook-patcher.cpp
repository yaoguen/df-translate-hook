﻿#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <filesystem>

#include "detours.h"

#ifdef _M_IX86
#pragma comment (lib, "lib.X86/detours.lib")
#elif _M_X64
#pragma comment (lib, "lib.X64/detours.lib")
#endif // _M_IX86

static BOOL CALLBACK ExportCallback(PVOID pContext, ULONG nOrdinal, LPCSTR pszSymbol, PVOID pbTarget) {
	if (nOrdinal == 1)
		*((BOOL*)pContext) = TRUE;

	return TRUE;
}

BOOL DetourDoesDllExportOrdinal1(const TCHAR* pszDllPath) {
	HMODULE hDll = LoadLibraryEx(pszDllPath, NULL, DONT_RESOLVE_DLL_REFERENCES);
	BOOL validFlag = FALSE;

	if (hDll != 0)
		DetourEnumerateExports(hDll, &validFlag, ExportCallback);
	FreeLibrary(hDll);

	return validFlag;
}

static BOOL CALLBACK AddBywayCallback(PVOID pContext, LPCSTR pszFile, LPCSTR* ppszOutFile) {
	static DWORD nByways = 0;
	char* dllpath = (char*)pContext;

	if (pContext == NULL)
		nByways++;
	else if (--nByways == 0)
		*ppszOutFile = dllpath;

	return TRUE;
}

void PatchingBytesInEXE(std::string exepath, DWORD offset, BYTE bytes[], DWORD size) {
	OVERLAPPED O_F;
	DWORD size_write;

	HANDLE  hFile = CreateFile(exepath.c_str(),
		GENERIC_WRITE, FILE_SHARE_READ,
		NULL, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile == INVALID_HANDLE_VALUE) {
		printf("Couldn't open input file: %s, error: %d\n", exepath.c_str(), GetLastError());
		return;
	}

	O_F.OffsetHigh = 0;
	O_F.Offset = offset;
	O_F.hEvent = 0;
	WriteFile(hFile, bytes, size, &size_write, &O_F);
	printf("Write bytes: %d\n", size_write);
	CloseHandle(hFile);
}

bool CreateExeWithDllImport(std::string& exepath, std::string& dllpath) {
	if (!DetourDoesDllExportOrdinal1(dllpath.c_str()))
		return false;

	PDETOUR_BINARY pBinary = NULL;

	std::string tmp_path = exepath + "_tmp.exe";

	std::filesystem::copy_file(exepath, tmp_path);

	HANDLE hOld = CreateFile(tmp_path.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	HANDLE hNew = CreateFile(exepath.c_str(), GENERIC_WRITE | GENERIC_READ, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);

	if (hOld == INVALID_HANDLE_VALUE) {
		printf("Couldn't open input file: %s, error: %d\n", exepath.c_str(), GetLastError());
		return false;
	}

	if ((pBinary = DetourBinaryOpen(hOld)) == NULL) {
		printf("DetourBinaryOpen failed: %d\n", GetLastError());
		return false;
	}
	DetourBinaryResetImports(pBinary);

	if (!DetourBinaryEditImports(pBinary, NULL, AddBywayCallback, NULL, NULL, NULL)) {
		printf("DetourBinaryEditImports failed: %d\n", GetLastError());
		return false;
	}

	if (!DetourBinaryEditImports(pBinary, (PVOID)dllpath.c_str(), AddBywayCallback, NULL, NULL, NULL)) {
		printf("DetourBinaryEditImports failed: %d\n", GetLastError());
		return false;
	}

	if (!DetourBinaryWrite(pBinary, hNew)) {
		printf("DetourBinaryWrite failed: %d\n", GetLastError());
		return false;
	}

	DetourBinaryClose(pBinary);
	CloseHandle(hOld);
	CloseHandle(hNew);

	std::filesystem::remove(tmp_path);

	return true;
}

int main() {
	std::string exepath = "Dwarf Fortress.exe";
	std::string dllpath = "df-translate-hook.dll";

	FILE* fp;

	printf("Exe Patcher\n");

	fp = fopen(exepath.c_str(), "rb");
	if (fp == NULL) {
		printf("Could not open Exe %s?\n", exepath.c_str());
		system("pause");
		return 0;
	}
	fclose(fp);

	fp = fopen(dllpath.c_str(), "rb");
	if (fp == NULL) {
		printf("Could not open Dll %s?\n", dllpath.c_str());
		system("pause");
		return 0;
	}
	fclose(fp);



//	// Replacing "jmp rcx" with "jmp 0x14032d9e4"
//	// to bypass string length checking when reading index(hardcore) file
//#ifdef _M_IX86
//	PatchingBytesInEXE(exepath, 0x002b0b6d, new BYTE[7]{ 0xE9, 0x15, 0x03, 0x00, 0x00, 0x90, 0x90 }, 7);
//#elif _M_X64
//	PatchingBytesInEXE(exepath, 0x0032ca93, new BYTE[6]{ 0xE9, 0x4C, 0x03, 0x00, 0x00, 0x90 }, 6);
//#endif // _M_IX86
//
//
//	// Replacing "jz 0x140360e65" with "jmp	0x140360e65"
//	// to switch a special encryption for an index file to a regular one
//#ifdef _M_IX86
//	PatchingBytesInEXE(exepath, 0x002da2df, new BYTE[1]{ 0xEB }, 1);
//#elif _M_X64
//	PatchingBytesInEXE(exepath, 0x003601f7, new BYTE[1]{ 0xEB }, 1);
//#endif // _M_IX86

	// Patching unicode table
	BYTE bytes[] = { 0x5e, 0x04, 0x00, 0x00, 0xfa, 0x00, 0x00, 0x00, 0xf1, 0x00, 0x00, 0x00, 0x90, 0x04,
		 0x00, 0x00, 0xaa, 0x00, 0x00, 0x00, 0xba, 0x00, 0x00, 0x00, 0x01, 0x04, 0x00, 0x00, 0x10, 0x23,
		 0x00, 0x00, 0x04, 0x04, 0x00, 0x00, 0xbd, 0x00, 0x00, 0x00, 0xbc, 0x00, 0x00, 0x00, 0xa1, 0x00,
		 0x00, 0x00, 0xab, 0x00, 0x00, 0x00, 0xbb, 0x00, 0x00, 0x00, 0x91, 0x25, 0x00, 0x00, 0x92, 0x25,
		 0x00, 0x00, 0x06, 0x04, 0x00, 0x00, 0x56, 0x04, 0x00, 0x00, 0x91, 0x04, 0x00, 0x00, 0x61, 0x25,
		 0x00, 0x00, 0x62, 0x25, 0x00, 0x00, 0x56, 0x25, 0x00, 0x00, 0x51, 0x04, 0x00, 0x00, 0x63, 0x25,
		 0x00, 0x00, 0x54, 0x04, 0x00, 0x00, 0x57, 0x25, 0x00, 0x00, 0x5d, 0x25, 0x00, 0x00, 0x5c, 0x25,
		 0x00, 0x00, 0x5b, 0x25, 0x00, 0x00, 0x57, 0x04, 0x00, 0x00, 0x10, 0x04, 0x00, 0x00, 0x11, 0x04,
		 0x00, 0x00, 0x12, 0x04, 0x00, 0x00, 0x13, 0x04, 0x00, 0x00, 0x14, 0x04, 0x00, 0x00, 0x15, 0x04,
		 0x00, 0x00, 0x16, 0x04, 0x00, 0x00, 0x17, 0x04, 0x00, 0x00, 0x18, 0x04, 0x00, 0x00, 0x19, 0x04,
		 0x00, 0x00, 0x1a, 0x04, 0x00, 0x00, 0x1b, 0x04, 0x00, 0x00, 0x1c, 0x04, 0x00, 0x00, 0x1d, 0x04,
		 0x00, 0x00, 0x1e, 0x04, 0x00, 0x00, 0x1f, 0x04, 0x00, 0x00, 0x20, 0x04, 0x00, 0x00, 0x21, 0x04,
		 0x00, 0x00, 0x22, 0x04, 0x00, 0x00, 0x23, 0x04, 0x00, 0x00, 0x24, 0x04, 0x00, 0x00, 0x25, 0x04,
		 0x00, 0x00, 0x26, 0x04, 0x00, 0x00, 0x27, 0x04, 0x00, 0x00, 0x28, 0x04, 0x00, 0x00, 0x29, 0x04,
		 0x00, 0x00, 0x2a, 0x04, 0x00, 0x00, 0x2b, 0x04, 0x00, 0x00, 0x2c, 0x04, 0x00, 0x00, 0x2d, 0x04,
		 0x00, 0x00, 0x2e, 0x04, 0x00, 0x00, 0x2f, 0x04, 0x00, 0x00, 0x30, 0x04, 0x00, 0x00, 0x31, 0x04,
		 0x00, 0x00, 0x32, 0x04, 0x00, 0x00, 0x33, 0x04, 0x00, 0x00, 0x34, 0x04, 0x00, 0x00, 0x35, 0x04,
		 0x00, 0x00, 0x36, 0x04, 0x00, 0x00, 0x37, 0x04, 0x00, 0x00, 0x38, 0x04, 0x00, 0x00, 0x39, 0x04,
		 0x00, 0x00, 0x3a, 0x04, 0x00, 0x00, 0x3b, 0x04, 0x00, 0x00, 0x3c, 0x04, 0x00, 0x00, 0x3d, 0x04,
		 0x00, 0x00, 0x3e, 0x04, 0x00, 0x00, 0x3f, 0x04, 0x00, 0x00, 0x40, 0x04, 0x00, 0x00, 0x41, 0x04,
		 0x00, 0x00, 0x42, 0x04, 0x00, 0x00, 0x43, 0x04, 0x00, 0x00, 0x44, 0x04, 0x00, 0x00, 0x45, 0x04,
		 0x00, 0x00, 0x46, 0x04, 0x00, 0x00, 0x47, 0x04, 0x00, 0x00, 0x48, 0x04, 0x00, 0x00, 0x49, 0x04,
		 0x00, 0x00, 0x4a, 0x04, 0x00, 0x00, 0x4b, 0x04, 0x00, 0x00, 0x4c, 0x04, 0x00, 0x00, 0x4d, 0x04,
		 0x00, 0x00, 0x4e, 0x04, 0x00, 0x00, 0x4f, 0x04 };

#ifdef _M_IX86
	PatchingBytesInEXE(exepath, 0x00c35d50, bytes, sizeof(bytes));
#elif _M_X64
	PatchingBytesInEXE(exepath, 0x00e786e8, bytes, sizeof(bytes));
#endif // _M_IX86

	CreateExeWithDllImport(exepath, dllpath);
	printf("Success!\n");
	system("pause");
}