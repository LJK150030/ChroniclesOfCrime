#pragma once

// Represents a physical (or virtual) controller button (or, later on, keyboard key).
// Each button stores its current and previous state (two bools).

class KeyButtonState
{
public:
	void UpdateStatus(bool is_now_pressed);
	bool IsPressed() const { return m_isPressed; }
	bool WasJustPressed() const;
	void Rest();

private:
	bool m_isPressed = false;
	bool m_wasPressedLastFrame = false;
};
