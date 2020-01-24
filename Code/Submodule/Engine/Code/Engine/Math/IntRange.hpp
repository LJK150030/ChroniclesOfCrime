#pragma once
#include <string>

struct IntRange
{
public:
	IntRange(): m_min(0), m_max(0)
	{
	};
	explicit IntRange(int min, int max);
	explicit IntRange(int minmax);

	std::string GetAsString() const;
	int GetNumberFormRange() const;


	void SetFromText(const char* text); // Parses “4~6” or “ -.3 ~ 0.05 ” to min to max


public:
	int m_min;
	int m_max;
};
