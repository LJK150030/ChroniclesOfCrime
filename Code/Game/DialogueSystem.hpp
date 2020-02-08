#pragma once
#include "Game/GameCommon.hpp"
#include "Game/GameNames.hpp"

//Wrapper class for ImGui, that makes a console window
class DialogueSystem
{
public:
	DialogueSystem();
	~DialogueSystem();

	void	BeginFrame();
	void	Update(double delta_seconds);
	void	Render() const;
	void	EndFrame() const;
	void	AddLog(const char* format, ...);

private:
	void	UpdateHistory();
	void	UpdateInput();
	
	void	ClearLog();
	void	AddCardTypeCommand(CardType type,  const char* command);
	void	ExecCommand(const char* command_line);
	char*	StringDuplicate(const char* str);

private:
	char						m_inputBuf[MAX_INPUT];
	std::vector<char*>			m_items;
	std::vector<const char*>	m_commands;
	bool						m_autoScrolling = true;
	bool						m_scrollToBottom = false;
	
	bool	m_show = true;
	bool	m_imguiError = false;
	float	m_gameResolution[2] = {720.0f, 1080.0f};
	float	m_dialogWindowSize[2] = { 800.0f, 200.0f };

	char	m_locationCommand[MAX_COMMAND_LENGTH];
// 	char	m_evidenceCommand[MAX_COMMAND_LENGTH];
// 	char	m_contactCommand[MAX_COMMAND_LENGTH];
// 	char	m_characterCommand[MAX_COMMAND_LENGTH];
// 	char	m_itemCommand[MAX_COMMAND_LENGTH];
}; 
