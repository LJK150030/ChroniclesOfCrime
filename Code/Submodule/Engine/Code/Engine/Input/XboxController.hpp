#pragma once
#include "KeyButtonState.hpp"
#include "AnalogJoystick.hpp"

// Each controller owns 2 AnalogJoystick, 2 triggers, 14 KeyButtonStates, and an ID (0-3).

// XboxController::Update() should check if the controller is connected; if not, reset the joysticks, buttons, triggers, etc.
// If controller is connected, each XboxController should update the state of its 14 buttons, 2 triggers, and 2 joysticks.
// Each trigger should store its current position (as a float in [0,1]).

enum XboxButtonID
{
	XBOX_BUTTON_ID_INVALID = -1,

	//These are "simple" (literal/physical) binary buttons
	XBOX_BUTTON_ID_A,
	XBOX_BUTTON_ID_B,
	XBOX_BUTTON_ID_X,
	XBOX_BUTTON_ID_Y,
	XBOX_BUTTON_ID_BACK,
	XBOX_BUTTON_ID_START,
	XBOX_BUTTON_ID_LEFT_SHOULDER,
	XBOX_BUTTON_ID_RIGHT_SHOULDER,
	XBOX_BUTTON_ID_LEFT_THUMB,
	XBOX_BUTTON_ID_RIGHT_THUMB,
	XBOX_BUTTON_ID_DPAD_RIGHT,
	XBOX_BUTTON_ID_DPAD_UP,
	XBOX_BUTTON_ID_DPAD_LEFT,
	XBOX_BUTTON_ID_DPAD_DOWN,

	NUM_XBOX_BUTTONS
};

class XboxController
{
	friend class InputSystem;

public:
	explicit XboxController(int controller_id);
	~XboxController();

	bool IsConnected() const { return m_isConnected; }
	int GetControllerID() const { return m_controllerID; }
	const AnalogJoyStick& GetLeftJoyStick() const { return m_leftJoystick; }
	const AnalogJoyStick& GetRightJoyStick() const { return m_rightJoystick; }
	float GetLeftTrigger() const { return m_leftTriggerFraction; }
	float GetRightTrigger() const { return m_rightTriggerFraction; }
	const KeyButtonState& GetButtonState(XboxButtonID button_id) const;

private:
	void Update();
	void Rest();
	void UpdateTrigger(float& trigger_value, unsigned char raw_value);
	void UpdateJoyStick(AnalogJoyStick& joy_stick, short raw_x, short raw_y);
	void UpdateButton(XboxButtonID button_id, unsigned short button_flags, unsigned short button_flag);

public:
	const int m_controllerID = -1;
	bool m_isConnected = false;
	KeyButtonState m_buttonStates[NUM_XBOX_BUTTONS];
	AnalogJoyStick m_leftJoystick = AnalogJoyStick(0.3f, 0.9f);
	AnalogJoyStick m_rightJoystick = AnalogJoyStick(0.3f, 0.9f);
	float m_leftTriggerFraction = 0.0f;
	float m_rightTriggerFraction = 0.0f;
};
