#include "Engine/Input/AnalogJoystick.hpp"
#include "Engine/Math/MathUtils.hpp"

AnalogJoyStick::AnalogJoyStick(const float inner_dead_zone_fraction, const float outer_dead_zone_fraction):
	m_innerDeadZoneFraction(inner_dead_zone_fraction), m_outerDeadZoneFraction(outer_dead_zone_fraction)
{
}

void AnalogJoyStick::Reset()
{
	m_rawPosition = Vec2(0.0f, 0.0f);
	m_correctedPosition = Vec2(0.0f, 0.0f);
	m_correctedDegrees = 0.0f;
	m_correctedMagnitude = 0.0f;
}

void AnalogJoyStick::UpdatePosition(const float raw_normalized_x, const float raw_normalized_y)
{
	m_rawPosition = Vec2(raw_normalized_x, raw_normalized_y);

	const float raw_magnitude = m_rawPosition.GetLength();

	if (raw_magnitude < m_innerDeadZoneFraction)
	{
		m_correctedPosition = Vec2(0.0f, 0.0f);
		m_correctedMagnitude = 0.0f;
	}
	else
	{
		const float corrected_magnitude = RangeMapLinearFloat(raw_magnitude, m_innerDeadZoneFraction,
		                                                      m_outerDeadZoneFraction, 0.f, 1.f);

		m_correctedPosition = Vec2(raw_normalized_x, raw_normalized_y);
		m_correctedPosition.SetLength(corrected_magnitude);
		m_correctedPosition.ClampLength(1.0f);
		m_correctedDegrees = m_correctedPosition.GetAngleDegrees();
		m_correctedMagnitude = corrected_magnitude;
	}
}
