#include "Engine/EngineCommon.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/FloatRange.hpp"

FloatRange::FloatRange(const float min, const float max): m_min(min), m_max(max)
{
}

FloatRange::FloatRange(const float min_max)
{
	m_min = min_max;
	m_max = min_max;
}

std::string FloatRange::GetAsString() const
{
	return Stringf("%g~%g", m_min, m_max);
}

float FloatRange::GetNumber() const
{
	return g_randomNumberGenerator.GetRandomFloatInRange(m_min, m_max);
}

void FloatRange::SetFromText(const char* text)
{
	std::vector<std::string> splitWords = SplitStringOnDelimiter(text, '~');

	ASSERT_OR_DIE(splitWords.size() == 1 || splitWords.size() == 2,
		Stringf("Expecting a FloatRange from the following line, %s", text));

	m_min = static_cast<float>(atof(splitWords[0].c_str()));

	if (splitWords.size() == 1)
		m_max = static_cast<float>(atof(splitWords[0].c_str()));
	else
		m_max = static_cast<float>(atof(splitWords[1].c_str()));
}
