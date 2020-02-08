#include "Game/GameCommon.hpp"


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


char ToLowercase(const char c)
{
	if (c >= 'A' && c <= 'Z')
	{
		return static_cast<char>(c + 32);
	}

	return c;
}


String StringToLower(String str)
{
	const int num_chars = static_cast<int>(str.length());
	for (int c_idx = 0; c_idx < num_chars; ++c_idx)
	{
		str[c_idx] = ToLowercase(str[c_idx]);
	}

	return str;
}
