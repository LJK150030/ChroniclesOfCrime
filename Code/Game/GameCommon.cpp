#include "Game/GameCommon.hpp"

EventSystem* g_theDialogueEventSystem = nullptr;

int StringCompare(const char* str1, const char* str2)
{
	int compare;

	while ((compare = toupper(*str2) - toupper(*str1)) == 0 && *str1)
	{
		str1++;
		str2++;
	}

	return compare;
}


int StringNCompare(const char* str1, const char* str2, int first_n_chars)
{
	int compare = 0;

	while (first_n_chars > 0 && (compare = toupper(*str2) - toupper(*str1)) == 0 && *str1)
	{
		str1++;
		str2++;
		first_n_chars--;
	}

	return compare;
}


void StringTrim(char* str)
{
	char* str_end = str + strlen(str);

	while (str_end > str && str_end[-1] == ' ')
	{
		str_end--;
	}

	*str_end = 0;
}
