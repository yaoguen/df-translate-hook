#include "pch.h"
#include "hook_helper.h"

std::map<std::string, std::string> dictionary;

void FillDictionary()
{
	FILE* ptrFile;
	fopen_s(&ptrFile, "df_rus.csv", "r");
	if (ptrFile != NULL)
	{
		std::string key = "";
		std::string arg = "";
		bool isKey = true;
		do
		{
			char sym = fgetc(ptrFile);
			if (isKey)
			{
				if (!strncmp(&sym, "\t", 1))
				{
					isKey = false;
					arg.clear();
					continue;
				}
				else if (!strncmp(&sym, "ÿ", 1)) { break; }
				else { key.append(&sym, 1); }
			}
			else
			{
				if (!strncmp(&sym, "\n", 1))
				{
					dictionary.insert(make_pair(key, arg));
					isKey = true;
					key.clear();
					arg.clear();
					continue;
				}
				else { arg.append(&sym, 1); }
			}
		} while (true);

		fclose(ptrFile);
	}
}

BOOL DictSearch(char* text)
{
	if (dictionary.find(text) == dictionary.end()) {
		return 0;
	} else {
		text = (char*)(dictionary.at(text)).c_str();
	}
	return 1;
}

void CreateDebugConsole()
{
	FILE* fp = 0;
	AllocConsole();
	SetConsoleTitle(L"DF debugging console");

	system("mode con cols=40 lines=20");
	freopen_s(&fp, "CONOUT$", "w", stdout);
	SetConsoleOutputCP(1251);
	COORD newSize = { 40,9999 };
	SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), newSize);
}

