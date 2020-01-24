#include "Engine/Input/KeyButtonState.hpp"

void KeyButtonState::UpdateStatus(const bool is_now_pressed)
{
	m_wasPressedLastFrame = m_isPressed;
	m_isPressed = is_now_pressed;
}

bool KeyButtonState::WasJustPressed() const
{
	return (!m_wasPressedLastFrame && m_isPressed);
}

void KeyButtonState::Rest()
{
	m_wasPressedLastFrame = false;
	m_isPressed = false;
}
