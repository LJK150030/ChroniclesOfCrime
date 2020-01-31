#pragma once
#include "Engine/Renderer/ImGUISystem.hpp"

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
	void		UpdateHistory();
	void		UpdateInput();
	
	void		ClearLog();
	void		ExecCommand(const char* command_line);
	
	int			StringCompare(const char* str1, const char* str2);
	int			SringNCompare(const char* str1, const char* str2, int first_n_chars);
	char*		StringDuplicate(const char* str);
	void		StringTrim(char* str);

private:
	char					m_inputBuf[256];
	ImVector<char*>			m_items;
	ImVector<const char*>	m_commands;
	ImVector<char*>			m_history;
	int						m_historyPos = -1;	// -1: new line, 0, 1, 2,..History.Size-1 browsing history.
	bool					m_autoScrolling = true;
	bool					m_scrollToBottom = false;
	
	bool					m_show = true;
	bool					m_imguiError = false;
	float					m_gameResolution[2] = {720.0f, 1080.0f};
	float					m_dialogWindowSize[2] = { 800.0f, 200.0f };
}; 
