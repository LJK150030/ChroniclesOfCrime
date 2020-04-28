#include "Engine/EngineCommon.hpp"
#include "Engine/Renderer/ImGUISystem.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"

#include "Game/GameCommon.hpp"
#include "Game/DialogueSystem.hpp"
#include "Game/Game.hpp"
#include "Game/Scenario.hpp"
#include "ThirdParty/imGUI/imgui_internal.h"


DialogueSystem::DialogueSystem(Game* owner) : m_theGame(owner)
{
	ClearLog();

	m_items = std::vector<LogEntry*>();
	memset(m_inputBuf, 0, sizeof(m_inputBuf));
		

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
	
}


void DialogueSystem::Update(double delta_seconds)
{
	UNUSED(delta_seconds)

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
		ImVec2(0.0f, m_gameResolution[1] - m_dialogWindowSize[1] * 0.75f),
		ImGuiCond_Always
	);

	ImGui::SetWindowFontScale(FONT_SCALE);

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
}


void DialogueSystem::ClearLog()
{
	for (int idx = 0; idx < static_cast<int>(m_items.size()); idx++)
	{
		delete m_items[idx];
		m_items[idx] = nullptr;
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


void DialogueSystem::AddLog(LogType type, const String& log_message)
{
	LogEntry* log = new LogEntry();
	
	log->m_message = log_message;
	log->m_type = type;
	Vec4 color;
	switch(type)
	{
		case LOG_ERROR:
		{
			color = Vec4(242.0f / 255.0f, 156.0f / 255.0f, 156.0f / 255.0f, 1.0f);
			break;
		}
		case LOG_MESSAGE:
		{
			color = Vec4(242.0f / 255.0f, 228.0f / 255.0f, 154.0f / 255.0f, 1.0f);
			break;
		}
		case LOG_ECHO:
		{
			color = Vec4(242.0f / 255.0f, 181.0f / 255.0f, 154.0f / 255.0f, 1.0f);
			break;
		}
		case LOG_LOCATION:
		{
			color = Vec4(158.0f / 255.0f, 223.0f / 255.0f, 246.0f / 255.0f, 1.0f);
			break;
		}
		case LOG_CHARACTER:
		{
			color = Vec4(158.0f / 255.0f, 223.0f / 255.0f, 246.0f / 255.0f, 1.0f);
			break;
		}
		case LOG_ITEM:
		{
			color = Vec4(158.0f / 255.0f, 223.0f / 255.0f, 246.0f / 255.0f, 1.0f);
			break;
		}
		default:
		{
			color = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
			break;
		};
	}
	log->m_color = color;
	
	m_items.push_back(log);
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

	
// 	ImGui::DragFloat("font scale", &FONT_SCALE, 0.001f, 0.1f, 5.0f);
// 	ImGui::DragFloat("cursor pos", &CURSOR_POS, 1.0f, 1.0f, 2000.0f);
// 	ImGui::DragFloat("text wrap backup", &TEXT_WRAP_BACKUP_VAL, 1.0f, 1.0f, 2000.0f);
// 	ImGui::DragFloat("text wrap positi", &TEXT_WRAP_POS, 1.0f, 1.0f, 2000.0f);
// 	TEXT_WRAP_POS = 1575.0f - (CURSOR_POS * FONT_SCALE);
	
	
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1));

	// for loop through each "Card Dialogue Type"
	for (int i = 0; i < static_cast<int>(m_items.size()); i++)
	{
		const char* item = m_items[i]->m_message.c_str();
		ImVec4 color(m_items[i]->m_color.x, m_items[i]->m_color.y, m_items[i]->m_color.z, m_items[i]->m_color.w);
		ImGui::PushStyleColor(ImGuiCol_Text, color);

		if(m_items[i]->m_type != LOG_ECHO)
		{
			ImGui::SetCursorPosX(CURSOR_POS);
		}
		else
		{
			ImGui::SetCursorPosX(0.0f);
		}
		
		TextWrapped(item);
		ImGui::PopStyleColor();
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
	AddLog(LOG_ECHO,Stringf("# %s\n", command_line));

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
		AddLog(LOG_ERROR, Stringf("Unknown command: '%s'\n", command_line));
	}
	
	m_scrollToBottom = true;
}


void DialogueSystem::TextWrapped(const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	TextWrappedV(fmt, args);
	va_end(args);
}

void DialogueSystem::TextWrappedV(const char* fmt, const va_list args)
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	const bool need_backup = (window->DC.TextWrapPos < TEXT_WRAP_BACKUP_VAL);
	if (need_backup)
	{
		//ImGui::PushTextWrapPos(TEXT_WRAP_POS);
		ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + TEXT_WRAP_POS);
	}
	ImGui::TextV(fmt, args);
	if (need_backup)
	{
		ImGui::PopTextWrapPos();
	}
}
