#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/EngineCommon.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/RawNoise.hpp"


RandomNumberGenerator::RandomNumberGenerator(const unsigned seed)
{
	m_seed = seed;
	m_position = 0;
}

int RandomNumberGenerator::GetRandomIntLessThan(const int max_not_inclusive)
{
	return ModPositive(static_cast<int>(Get1DNoiseUint(m_position++, m_seed)), max_not_inclusive);
}

int RandomNumberGenerator::GetRandomIntInRange(const int min_inclusive, const int max_inclusive)
{
	return Get1DNoiseUint(m_position++, m_seed) % (max_inclusive - min_inclusive + 1) + min_inclusive;
}

float RandomNumberGenerator::GetRandomFloatZeroToOne()
{
	return Get1DNoiseZeroToOne(m_position++, m_seed);
}

float RandomNumberGenerator::GetRandomFloatInRange(const float min_inclusive, const float max_inclusive)
{
	return min_inclusive + Get1DNoiseUint(m_position++, m_seed) / (static_cast<float>(INT_MAX / (max_inclusive -
		min_inclusive)));
}

Vec2 RandomNumberGenerator::GetRandomVec2InsideUnitCircle()
{
	const float angle = GetRandomFloatInRange(0.0f, MATH_PI * 2.0f);
	return Vec2::MakeFromPolarRadians(angle, 1.0f);
}

bool RandomNumberGenerator::PercentChance(const float fraction)
{
	return fraction >= GetRandomFloatZeroToOne();
}
