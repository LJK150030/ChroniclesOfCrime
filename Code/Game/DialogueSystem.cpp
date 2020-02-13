#include "Engine/EngineCommon.hpp"
#include "Engine/Renderer/ImGUISystem.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"


#include "Game/DialogueSystem.hpp"
#include "Game/GameCommon.hpp"

//TODO: hacky solution to get the Scenario
#include "Game/App.hpp"
#include "Game/Game.hpp"
#include "Game/Scenario.hpp"
#include "Engine/Core/StringUtils.hpp"

DialogueSystem::DialogueSystem(Game* owner) : m_theGame(owner)
{
	ClearLog();
	
	memset(m_inputBuf, 0, sizeof(m_inputBuf));

	//Store all valid commands into a list

	m_commands.push_back("HELP");
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
	ImGui::End();
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

void DialogueSystem::AddCardTypeCommand(CardType type, const char* command)
{
	m_commands.push_back(command);

	char* stored_command;

	switch(type)
	{
	case CARD_LOCATION:
		{
			stored_command = m_locationCommand;
			break;
		}
	case CARD_CHARACTER:
		{
			stored_command = m_characterCommand;
			break;
		}
	case CARD_ITEM:
		{
			stored_command = m_itemCommand;
			break;
		}
	default:
		{
			ASSERT_OR_DIE(false, "This card type command has not been added")
			break;
		}
	}
	
	memcpy(stored_command, command, strlen(command) + 2);
	stored_command[strlen(command)] = ' ';
	stored_command[strlen(command) + 1] = '\0';
}


void DialogueSystem::AddLog(const String& log)
{
	m_items.push_back(StringDuplicate(log.c_str()));
}


void DialogueSystem::UpdateHistory()
{
	const float player_input_height_to_reserve = ImGui::GetStyle().ItemSpacing.y +
		ImGui::GetFrameHeightWithSpacing();

	ImGui::BeginChild(
		"DialogueHistory",
		ImVec2(0, -player_input_height_to_reserve),
		false,
		ImGuiWindowFlags_HorizontalScrollbar
	);

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1));

	// for loop through each "Card Dialogue Type"
	for (int i = 0; i < static_cast<int>(m_items.size()); i++)
	{
		const char* item = m_items[i];
		bool pop_color = false;

		// TODO: create a struct that represent a card type's dialog
		if (StringNCompare(item, "# ", 2) == 0) //play back messages
		{
			ImGui::PushStyleColor(
				ImGuiCol_Text,
				ImVec4(0.9490196078431373f, 0.8000000000000000f, 0.7137254901960784f, 1.0f)
			);

			pop_color = true;
		}
		else if (StringNCompare(item, g_locationHeader, 5) == 0) // location dialogue struct
		{
			ImGui::PushStyleColor(
				ImGuiCol_Text,
				ImVec4(0.0470588235294118f, 0.9098039215686275f, 0.8941176470588235f, 1.0f)
			);

			pop_color = true;
		}
		else if (StringNCompare(item, g_characterHeader, 5) == 0) // character reply dialogue struct
		{
			ImGui::PushStyleColor(
				ImGuiCol_Text,
				ImVec4(0.9215686274509804f, 0.5098039215686275f, 0.0000000000000000f, 1.0f)
			);

			pop_color = true;
		}
		else if (StringNCompare(item, g_itemHeader, 5) == 0) // item dialogue struct
		{
			ImGui::PushStyleColor(
				ImGuiCol_Text,
				ImVec4(0.9098039215686275f, 0.2078431372549020f, 0.0470588235294118f, 1.0f)
			);

			pop_color = true;
		}
		
		ImGui::TextUnformatted(item); 
	
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
			ExecuteCommand(s);
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
}


void DialogueSystem::ExecuteCommand(const char* command_line)
{
	AddLog(Stringf("# %s\n", command_line));

	EventArgs args;
	std::vector<std::string> event_and_args = SplitStringOnDelimiter(command_line, ' ');
	int num_args = static_cast<int>(event_and_args.size());
	int num_functions_called = 0;
	
	const std::string event_name = StringToLower(event_and_args[0]);

	if(num_args == 1)
	{
		num_functions_called  = g_theDialogueEventSystem->FireEvent(event_name, args);
	}
	else
	{
		String sub_string = "";

		for(int str_idx = 1; str_idx < num_args; ++str_idx)
		{
			if(str_idx != num_args - 1)
			{
				sub_string += event_and_args[str_idx] + " ";	
			}
			else
			{
				sub_string += event_and_args[str_idx];
			}
		}

		//Setting up 
		args.SetValue("card", sub_string); //comparing just the first word
		args.SetValue("scenario", m_theGame->GetCurrentScenario());
		args.SetValue("dialoguesystem", this);
		num_functions_called = g_theDialogueEventSystem->FireEvent(event_name, args);
	}

	if (num_functions_called == 0)
	{
		AddLog(Stringf("Unknown command: '%s'\n", command_line));
	}
	
	m_scrollToBottom = true;
}


char* DialogueSystem::StringDuplicate(const char* str)
{
	const size_t len = strlen(str) + 1;
	void* buf = malloc(len);

	ASSERT_OR_DIE(buf, "could not duplicate c-style string");

	return static_cast<char*>(memcpy(buf, static_cast<const void*>(str), len));
}

