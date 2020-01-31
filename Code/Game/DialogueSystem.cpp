#include "Engine/EngineCommon.hpp"

#include "Game/DialogueSystem.hpp"
#include "Game/GameCommon.hpp"

DialogueSystem::DialogueSystem()
{
	ClearLog();
	
	memset(m_inputBuf, 0, sizeof(m_inputBuf));

	//Store all valid commands into a list
	m_commands.push_back("HELP");
	m_commands.push_back("HISTORY");
	m_commands.push_back("CLEAR");

	Vec2 frame_resolution = g_gameConfigBlackboard.GetValue(
		"Screensize",
		Vec2(720.0f, 1080.0f)
	);

	m_gameResolution[0] = frame_resolution.x;
	m_gameResolution[1] = frame_resolution.y;
	m_dialogWindowSize[0] = frame_resolution.x;
	m_dialogWindowSize[1] = m_gameResolution[1] * 0.25f;
}


DialogueSystem::~DialogueSystem()
{
	ClearLog();
	
	for (int i = 0; i < m_history.Size; i++)
	{
		free(m_history[i]);
	}
}


void DialogueSystem::BeginFrame()
{
	g_imGUI->BeginFrame();
	ImGui::NewFrame();

	m_imguiError = ImGui::Begin(
		"Dialogue System",
		&m_show,
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoSavedSettings
	);

	
	ImGui::SetWindowSize(
		ImVec2(m_dialogWindowSize[0], m_dialogWindowSize[1]), 
		ImGuiCond_Always
	);

	ImGui::SetWindowPos(
		ImVec2(0.0f, m_gameResolution[1] - m_dialogWindowSize[1]),
		ImGuiCond_Always
	);

}


void DialogueSystem::Update(double delta_seconds)
{
	UNUSED(delta_seconds)

	if (!m_imguiError)
	{
		ImGui::End();
	}

	UpdateHistory();
	ImGui::Separator();
	UpdateInput();
}


void DialogueSystem::Render() const
{
	g_imGUI->Render();
}


void DialogueSystem::EndFrame() const
{
	g_imGUI->EndFrame();
}


void DialogueSystem::ClearLog()
{
	for (int idx = 0; idx < m_items.Size; idx++)
	{
		free(m_items[idx]);
	}
	
	m_items.clear();
}


void DialogueSystem::AddLog(const char* format, ...)
{
	char buffer[1024];
	
	va_list args;
	
	va_start(args, format);
	vsnprintf(buffer, IM_ARRAYSIZE(buffer), format, args);
	buffer[IM_ARRAYSIZE(buffer) - 1] = 0;
	va_end(args);
	
	m_items.push_back(StringDuplicate(buffer));
}


void DialogueSystem::UpdateHistory()
{
	//creating the history
	const float player_input_height_to_reserve = ImGui::GetStyle().ItemSpacing.y +
		ImGui::GetFrameHeightWithSpacing();

	ImGui::BeginChild(
		"DialogueHistory",
		ImVec2(0, -player_input_height_to_reserve),
		false,
		ImGuiWindowFlags_HorizontalScrollbar
	);

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1));

	for (int i = 0; i < m_items.Size; i++)
	{
		const char* item = m_items[i];

		// TODO: create a struct that stores the dialog history text.
		bool pop_color = false;
		if (strstr(item, "[error]")) //for error messages
		{
			ImGui::PushStyleColor(
				ImGuiCol_Text,
				ImVec4(1.0f, 0.4f, 0.4f, 1.0f)
			);

			pop_color = true;
		}
		else if (strncmp(item, "# ", 2) == 0) //for play back
		{
			ImGui::PushStyleColor(
				ImGuiCol_Text,
				ImVec4(1.0f, 0.8f, 0.6f, 1.0f)
			);

			pop_color = true;
		}
		
		ImGui::TextUnformatted(item); // for everything else
	
		if (pop_color)
		{
			ImGui::PopStyleColor();
		}
	}

	//Auto scroll to the bottom when the player hits enter
	if (m_scrollToBottom || (m_autoScrolling && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
	{
		ImGui::SetScrollHereY(1.0f);
	}

	m_scrollToBottom = false;

	ImGui::PopStyleVar();
	ImGui::EndChild();
}


void DialogueSystem::UpdateInput()
{
	bool reclaim_focus = false;
	
	if (ImGui::InputText(
			"Input",
			m_inputBuf,
			IM_ARRAYSIZE(m_inputBuf),
			ImGuiInputTextFlags_EnterReturnsTrue
			)
		)
	{
		char* s = m_inputBuf;
		StringTrim(s);
		
		if (s[0])
		{
			ExecCommand(s);
		}

		strcpy_s(s, sizeof s, "");
		reclaim_focus = true;
	}

	ImGui::SetItemDefaultFocus();

	//after pressing enter, keep the focus so we can keep typing
	if (reclaim_focus)
	{
		ImGui::SetKeyboardFocusHere(-1);
	}

	ImGui::End();
}


int DialogueSystem::StringCompare(const char* str1, const char* str2)
{
	int compare;
	
	while ((compare = toupper(*str2) - toupper(*str1)) == 0 && *str1)
	{
		str1++;
		str2++;
	}
	
	return compare;
}


int DialogueSystem::SringNCompare(const char* str1, const char* str2, int first_n_chars)
{
	int compare = 0;

	while (first_n_chars > 0 && (compare = toupper(*str2) - toupper(*str1)) == 0 && *str1)
	{
		str1++;
		str2++;
		first_n_chars--;
	}
	
	return compare;
}


char* DialogueSystem::StringDuplicate(const char* str)
{
	size_t len = strlen(str) + 1;
	void* buf = malloc(len);
	IM_ASSERT(buf);
	return static_cast<char*>(memcpy(buf, static_cast<const void*>(str), len));
}


void DialogueSystem::StringTrim(char* str)
{
	char* str_end = str + strlen(str);

	while (str_end > str && str_end[-1] == ' ')
	{
		str_end--;
	}
	
	*str_end = 0;
}


void DialogueSystem::ExecCommand(const char* command_line)
{
	AddLog("# %s\n", command_line);

	// Insert into history. First find match and delete it so it can be pushed to the back.
	// This isn't trying to be smart or optimal.
	
	m_historyPos = -1;
	for (int i = m_history.Size - 1; i >= 0; i--)
	{
		if (StringCompare(m_history[i], command_line) == 0)
		{
			free(m_history[i]);
			m_history.erase(m_history.begin() + i);
			break;
		}	
	}
	
	m_history.push_back(StringDuplicate(command_line));

	// Process command
	if (StringCompare(command_line, "CLEAR") == 0)
	{
		ClearLog();
	}
	else if (StringCompare(command_line, "HELP") == 0)
	{
		AddLog("Commands:");
		for (int i = 0; i < m_commands.Size; i++)
			AddLog("- %s", m_commands[i]);
	}
	else if (StringCompare(command_line, "HISTORY") == 0)
	{
		int first = m_history.Size - 10;
		for (int i = first > 0 ? first : 0; i < m_history.Size; i++)
		{
			AddLog("%3d: %s\n", i, m_history[i]);
		}
	}
	else
	{
		AddLog("Unknown command: '%s'\n", command_line);
	}

	// On commad input, we scroll to bottom even if AutoScroll==false
	m_scrollToBottom = true;
}

