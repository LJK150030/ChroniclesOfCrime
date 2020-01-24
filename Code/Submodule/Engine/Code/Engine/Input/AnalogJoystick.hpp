#pragma once
#include"Engine/Math/Vec2.hpp"

// Each analog stick owns data and logic for joystick positioning & dead-zone corrections.
// Each joystick should store its raw and deadzone-corrected position in [-1,1] normalized Cartesian (x,y) coordinates.
class AnalogJoyStick
{
	friend class XboxController;

public:
	explicit AnalogJoyStick(float inner_dead_zone_fraction, float outer_dead_zone_fraction);

	Vec2 GetPosition() const { return m_correctedPosition; }
	float GetMagnitude() const { return m_correctedMagnitude; }
	float GetAngleDegrees() const { return m_correctedDegrees; }
	Vec2 GetRawPosition() const { return m_rawPosition; }
	float GetInnerDeadZoneFraction() const { return m_innerDeadZoneFraction; }
	float GetOuterDeadZoneFraction() const { return m_outerDeadZoneFraction; }
	void Reset();

private:
	void UpdatePosition(float raw_normalized_x, float raw_normalized_y);

private:
	const float m_innerDeadZoneFraction;
	const float m_outerDeadZoneFraction;
	Vec2 m_rawPosition = Vec2(0.0f, 0.0f);
	Vec2 m_correctedPosition = Vec2(0.0f, 0.0f);
	float m_correctedDegrees = 0.0f;
	float m_correctedMagnitude = 0.0f;
};
