#pragma once
#include "Game/GameCommon.hpp"
#include "Game/GameNames.hpp"

class Game;

struct LogEntry
{
	String		m_message = "";
	LogType		m_type = LOG_ERROR;
	Vec4		m_color;
};


//Wrapper class for ImGui, that makes a console window
class DialogueSystem
{
public:
	DialogueSystem(Game* owner);
	~DialogueSystem();

	void	BeginFrame();
	void	Update(double delta_seconds);
	void	Render() const;
	void	EndFrame() const;
	void	AddLog(LogType type, const String& log_message);
	void	ClearLog();

private:
	void	UpdateHistory();
	void	UpdateInput();
	
	void	AddCardTypeCommand(CardType type,  const char* command);
	void	ExecuteCommand(const char* command_line);

private:
	// Owner
	Game*						m_theGame = nullptr;
	
	char						m_inputBuf[MAX_INPUT];
	std::vector<LogEntry*>		m_items;
	std::vector<const char*>	m_commands;
	bool						m_autoScrolling = true;
	bool						m_scrollToBottom = false;
	
	bool	m_show = true;
	bool	m_imguiError = false;
	float	m_gameResolution[2] = {720.0f, 1080.0f};
	float	m_dialogWindowSize[2] = { 800.0f, 200.0f };

	char	m_locationCommand[MAX_COMMAND_LENGTH];
 	char	m_characterCommand[MAX_COMMAND_LENGTH];
 	char	m_itemCommand[MAX_COMMAND_LENGTH];
}; 
