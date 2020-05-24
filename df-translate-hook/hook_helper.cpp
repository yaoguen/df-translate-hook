#include "pch.h"
#include "hook_helper.h"

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
				else if (!strncmp(&sym, "�", 1)) { break; }
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
