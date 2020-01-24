#pragma once
#include <string>

struct FloatRange
{
public:
	FloatRange(): m_min(0.0f), m_max(0.0f)
	{
	};
	explicit FloatRange(float min, float max);
	explicit FloatRange(float min_max);

	std::string GetAsString() const;
	float GetNumber() const;

	void SetFromText(const char* text); // Parses “4~6” or “ -.3 ~ 0.05 ” to min to max


public:
	float m_min;
	float m_max;
};
