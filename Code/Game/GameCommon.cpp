#include "Game/GameCommon.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

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


char* StringDuplicate(const char* str)
{
	const size_t len = strlen(str) + 1;
	void* buf = malloc(len);
	
	ASSERT_OR_DIE(buf, "could not duplicate c-style string");
	
	return static_cast<char*>(memcpy(buf, static_cast<const void*>(str), len));
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
