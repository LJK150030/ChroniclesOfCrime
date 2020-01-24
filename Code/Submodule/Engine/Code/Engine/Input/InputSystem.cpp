#include "Engine/Input/InputSystem.hpp"
#include "Engine/Input/XboxController.hpp"

InputSystem* g_theInput = nullptr;


InputSystem::InputSystem(): m_xboxController{XboxController(0), XboxController(1), XboxController(2), XboxController(3)}
{
}

InputSystem::~InputSystem() = default;

void InputSystem::Startup()
{
}

void InputSystem::BeginFrame()
{
	for (int i = 0; i < MAX_XBOX_CONTROLLERS; i++)
		m_xboxController[i].Update();
}

void InputSystem::EndFrame()
{
}

void InputSystem::ShutDown()
{
	for (int i = 0; i < MAX_XBOX_CONTROLLERS; i++)
		m_xboxController[i].Rest();
}

const XboxController& InputSystem::GetXboxController(const int controller_id) const
{
	return m_xboxController[controller_id];
}
