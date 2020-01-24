#include "Engine/Input/XboxController.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include <Windows.h>
#include <Xinput.h>
#include "Engine/Math/MathUtils.hpp"
#pragma comment( lib, "xinput9_1_0" ) // Xinput 1_4 doesn't work in Windows 7; use version 9_1_0 explicitly for broadest compatibility

XboxController::XboxController(const int controller_id): m_controllerID(controller_id)
{
}

XboxController::~XboxController() = default;

const KeyButtonState& XboxController::GetButtonState(const XboxButtonID button_id) const
{
	return m_buttonStates[button_id];
}

void XboxController::Update()
{
	XINPUT_STATE xbox_controller_state;
	memset(&xbox_controller_state, 0, sizeof(xbox_controller_state));
	const DWORD error_status = XInputGetState(m_controllerID, &xbox_controller_state);

	if (error_status == ERROR_SUCCESS)
	{
		m_isConnected = true;
		//	bLeftTrigger = xboxControllerState.Gamepad.bLeftTrigger						gets the unsigned char (0 to 255)
		//	bRightTrigger = xboxControllerState.Gamepad.bRightTrigger					gets the unsigned char (0 to 255)
		UpdateTrigger(m_leftTriggerFraction, xbox_controller_state.Gamepad.bLeftTrigger);
		UpdateTrigger(m_rightTriggerFraction, xbox_controller_state.Gamepad.bRightTrigger);


		//	sThumbLX = xboxControllerState.Gamepad.sThumbLX								gets the short (-32768 to 32767)
		//	sThumbLY = xboxControllerState.Gamepad.sThumbLY								gets the short (-32768 to 32767)
		UpdateJoyStick(m_leftJoystick, xbox_controller_state.Gamepad.sThumbLX, xbox_controller_state.Gamepad.sThumbLY);
		UpdateJoyStick(m_rightJoystick, xbox_controller_state.Gamepad.sThumbRX, xbox_controller_state.Gamepad.sThumbRY);

		//	wButtons = xboxControllerState.Gamepad.wButtons								gets the unsigned short (0 to 1111 1111 1111 1111)
		//	isButtonADown = xboxControllerState.Gamepad.wButtons & XINPUT_GAMEPAD_A		does a bit and operation on wButton and saved bit flag
		const unsigned short w_buttons = xbox_controller_state.Gamepad.wButtons;
		UpdateButton(XBOX_BUTTON_ID_A, w_buttons, XINPUT_GAMEPAD_A);
		UpdateButton(XBOX_BUTTON_ID_B, w_buttons, XINPUT_GAMEPAD_B);
		UpdateButton(XBOX_BUTTON_ID_X, w_buttons, XINPUT_GAMEPAD_X);
		UpdateButton(XBOX_BUTTON_ID_Y, w_buttons, XINPUT_GAMEPAD_Y);
		UpdateButton(XBOX_BUTTON_ID_BACK, w_buttons, XINPUT_GAMEPAD_BACK);
		UpdateButton(XBOX_BUTTON_ID_START, w_buttons, XINPUT_GAMEPAD_START);
		UpdateButton(XBOX_BUTTON_ID_LEFT_SHOULDER, w_buttons, XINPUT_GAMEPAD_LEFT_SHOULDER);
		UpdateButton(XBOX_BUTTON_ID_RIGHT_SHOULDER, w_buttons, XINPUT_GAMEPAD_RIGHT_SHOULDER);
		UpdateButton(XBOX_BUTTON_ID_LEFT_THUMB, w_buttons, XINPUT_GAMEPAD_LEFT_THUMB);
		UpdateButton(XBOX_BUTTON_ID_RIGHT_THUMB, w_buttons, XINPUT_GAMEPAD_RIGHT_THUMB);
		UpdateButton(XBOX_BUTTON_ID_DPAD_RIGHT, w_buttons, XINPUT_GAMEPAD_DPAD_RIGHT);
		UpdateButton(XBOX_BUTTON_ID_DPAD_UP, w_buttons, XINPUT_GAMEPAD_DPAD_UP);
		UpdateButton(XBOX_BUTTON_ID_DPAD_LEFT, w_buttons, XINPUT_GAMEPAD_DPAD_LEFT);
		UpdateButton(XBOX_BUTTON_ID_DPAD_DOWN, w_buttons, XINPUT_GAMEPAD_DPAD_DOWN);
	}
	else if (error_status == ERROR_DEVICE_NOT_CONNECTED)
	{
		Rest();
	}
	else // an unforeseen error
	{
		RecoverableWarning(__FILE__, __FUNCTION__, __LINE__, Stringf(
			                   "Xbox controller #%i reports error code %u (0x%08x).\n", m_controllerID,
			                   error_status, error_status));
	}
}

void XboxController::Rest()
{
	m_isConnected = false;

	for (int i = 0; i < NUM_XBOX_BUTTONS; i++)
	{
		m_buttonStates[i].Rest();
	}

	m_leftJoystick.Reset();
	m_rightJoystick.Reset();
	m_leftTriggerFraction = 0.0f;
	m_rightTriggerFraction = 0.0f;
}

void XboxController::UpdateTrigger(float& trigger_value, const unsigned char raw_value)
{
	trigger_value = RangeMapLinearFloat(raw_value, 0.f, 255.f, 0.0f, 1.0f);
}

void XboxController::UpdateJoyStick(AnalogJoyStick& joy_stick, const short raw_x, const short raw_y)
{
	const float raw_normalized_x = RangeMapLinearFloat(raw_x, -32768.f, 32767.f, -1.0f, 1.0f);
	const float raw_normalized_y = RangeMapLinearFloat(raw_y, -32768.f, 32767.f, -1.0f, 1.0f);

	joy_stick.UpdatePosition(raw_normalized_x, raw_normalized_y);
}

void XboxController::UpdateButton(const XboxButtonID button_id, const unsigned short button_flags,
                                  const unsigned short button_flag)
{
	const bool is_now_pressed = IsBitFlagSet(button_flags, button_flag);
	m_buttonStates[button_id].UpdateStatus(is_now_pressed);
}
