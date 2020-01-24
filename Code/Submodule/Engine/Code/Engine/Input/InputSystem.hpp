#pragma once
#include"Engine/Input/XboxController.hpp"

// Engine input system; owns 4 XboxController instances (and, later, mouse & keyboard)
// An InputSystem instance is owned (created, managed, destroyed) by the App
// InputSystem::BeginFrame() Updates each of the system’s 4 XboxControllers.

constexpr int MAX_XBOX_CONTROLLERS = 4;

//Key codes (platform-agnostic)
extern const unsigned char KEY_ESC;
extern const unsigned char KEY_ENTER;
extern const unsigned char KEY_SPACEBAR;
extern const unsigned char KEY_BACKSPACE;
extern const unsigned char KEY_UP_ARROW;
extern const unsigned char KEY_LEFT_ARROW;
extern const unsigned char KEY_DOWN_ARROW;
extern const unsigned char KEY_RIGHT_ARROW;
extern const unsigned char KEY_F1;
extern const unsigned char KEY_F2;
extern const unsigned char KEY_F3;
extern const unsigned char KEY_F4;
extern const unsigned char KEY_F5;
extern const unsigned char KEY_F6;
extern const unsigned char KEY_F7;
extern const unsigned char KEY_F8;
extern const unsigned char KEY_F9;
extern const unsigned char KEY_F10;
extern const unsigned char KEY_F11;
extern const unsigned char KEY_F12;

class InputSystem
{
public:
	InputSystem();
	~InputSystem();

	void Startup();
	void BeginFrame();
	void EndFrame();
	void ShutDown();

	const XboxController& GetXboxController(int controller_id) const;

private:
	XboxController m_xboxController[MAX_XBOX_CONTROLLERS];
};
