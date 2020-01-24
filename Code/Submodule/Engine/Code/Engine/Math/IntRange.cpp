#include "Engine/EngineCommon.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/IntRange.hpp"

IntRange::IntRange(const int min, const int max): m_min(min), m_max(max)
{
}

IntRange::IntRange(int minmax): m_min(minmax), m_max(minmax)
{
}

std::string IntRange::GetAsString() const
{
	return Stringf("%i~%i", m_min, m_max);
}

int IntRange::GetNumberFormRange() const
{
	return g_randomNumberGenerator.GetRandomIntInRange(m_min, m_max);
}

void IntRange::SetFromText(const char* text)
{
	std::vector<std::string> splitWords = SplitStringOnDelimiter(text, '~');

	ASSERT_OR_DIE(splitWords.size() == 1 || splitWords.size() == 2,
		Stringf("Expecting a IntRange from the following line, %s", text));

	m_min = atoi(splitWords[0].c_str());

	if (splitWords.size() == 1)
		m_max = atoi(splitWords[0].c_str());
	else
		m_max = atoi(splitWords[1].c_str());
}
