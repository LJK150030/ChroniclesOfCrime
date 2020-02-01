#include "Engine/EngineCommon.hpp"
#include "Engine/Renderer/ImGUISystem.hpp"


#include "Game/DialogueSystem.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Evidence.hpp"

DialogueSystem::DialogueSystem()
{
	ClearLog();
	
	memset(m_inputBuf, 0, sizeof(m_inputBuf));

	//Store all valid commands into a list
	m_commands.push_back("HELP");
	m_commands.push_back("CLEAR");
	m_commands.push_back("VIEW");

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
	for (int idx = 0; idx < static_cast<int>(m_items.size()); idx++)
	{
		free(m_items[idx]);
	}
	
	m_items.clear();
}


void DialogueSystem::AddLog(const char* messageFormat, ...)
{
	const int message_max_length = 1024;
	char message_literal[message_max_length];
	va_list variable_argument_list;
	va_start(variable_argument_list, messageFormat);
	vsnprintf_s(message_literal, message_max_length, _TRUNCATE, messageFormat, variable_argument_list);
	va_end(variable_argument_list);
	message_literal[message_max_length - 1] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)
	
	m_items.push_back(StringDuplicate(message_literal));
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

	for (int i = 0; i < static_cast<int>(m_items.size()); i++)
	{
		const char* item = m_items[i];

		// TODO: create a struct that stores the dialog history text.
		bool pop_color = false;
		if (StringNCompare(item, "# ", 2) == 0) //for error messages
		{
			ImGui::PushStyleColor(
				ImGuiCol_Text,
				ImVec4(1.0f, 0.8f, 0.0f, 1.0f)
			);

			pop_color = true;
		}
		else if (StringNCompare(item, "Found ", 5) == 0) //for play back
		{
			ImGui::PushStyleColor(
				ImGuiCol_Text,
				ImVec4(0.18431372549f, 0.85098039215f, 0.0f, 1.0f)
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


void DialogueSystem::ExecCommand(const char* command_line)
{
	AddLog("# %s\n", command_line);

	// Process command
	// TODO: clean this up with map
	if (StringCompare(command_line, "CLEAR") == 0)
	{
		ClearLog();
	}
	else if (StringCompare(command_line, "HELP") == 0)
	{
		AddLog("Commands:");
		for (int i = 0; i < static_cast<int>(m_commands.size()); i++)
		{
			AddLog("- %s", m_commands[i]);			
		}
	}
	else if (StringNCompare(command_line, "VIEW ", 5) == 0)
	{
		// want everything after the "VIEW "
		const int command_length = 5;
		const int input_buff_length = strlen(command_line);
		const int sub_input_length = input_buff_length - command_length + 1;
		
		char sub_input_buff[MAX_INPUT];
		memcpy(sub_input_buff, &command_line[command_length], sub_input_length);
		sub_input_buff[sub_input_length - 1] = '\0';

		AddLog("%s", Evidence::InvestigateItem(sub_input_buff).c_str());
	}
	else
	{
		AddLog("Unknown command: '%s'\n", command_line);
	}

	// On commad input, we scroll to bottom even if AutoScroll==false
	m_scrollToBottom = true;
}

