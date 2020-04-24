#include "Game/App.hpp"
#include "Game/Game.hpp"
#include "Game/DialogueSystem.hpp"
#include "Game/Scenario.hpp"
#include "Game/DialogueSystem.hpp"
#include "Game/Location.hpp"
#include "Game/Character.hpp"
#include "Game/Item.hpp"
#include "Game/Incident.hpp"
#include "Game/Trigger.hpp"
#include "Game/Condition.hpp"
#include "Game/Action.hpp"
#include "Game/VictoryCondition.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Renderer/ImGUISystem.hpp"

// Debugging ------------------------------------------------------------


STATIC bool DumpLocations(EventArgs& args)
{
	UNUSED(args);

	Scenario*	current_scenario = g_theApp->GetTheGame()->GetCurrentScenario();
	const LocationList* locations = current_scenario->GetLocationList();
	const int num_locs = static_cast<int>(locations->size());

	g_theDevConsole->PrintString(Rgba::GREEN, "Locations:");
	for (int loc_idx = 0; loc_idx < num_locs; ++loc_idx)
	{
		const String loc_name = locations->at(loc_idx).GetName();
		String line = loc_name;

		StringList nn_list = locations->at(loc_idx).GetListOfNicknames();
		const int num_nn = static_cast<int>(nn_list.size());
		for (int nn_idx = 0; nn_idx < num_nn; ++nn_idx)
		{
			line += Stringf(" '%s'", nn_list[nn_idx].c_str());
		}

		g_theDevConsole->PrintString(Rgba::GREEN, Stringf("\t %s", line.c_str()));

	}

	return true;
}


STATIC bool DumpCharacter(EventArgs& args)
{
	UNUSED(args);

	Scenario*	current_scenario = g_theApp->GetTheGame()->GetCurrentScenario();
	const CharacterList* characters = current_scenario->GetCharacterList();
	const int num_char = static_cast<int>(characters->size());

	g_theDevConsole->PrintString(Rgba::GREEN, "Characters:");
	for (int char_idx = 0; char_idx < num_char; ++char_idx)
	{
		const String char_name = characters->at(char_idx).GetName();
		String line = char_name;

		StringList nn_list = characters->at(char_idx).GetListOfNicknames();
		const int num_nn = static_cast<int>(nn_list.size());
		for (int nn_idx = 0; nn_idx < num_nn; ++nn_idx)
		{
			line += Stringf(" '%s'", nn_list[nn_idx].c_str());
		}

		g_theDevConsole->PrintString(Rgba::GREEN, Stringf("\t %s", line.c_str()));

	}

	return true;
}


STATIC bool DumpItems(EventArgs& args)
{
	UNUSED(args);

	Scenario*	current_scenario = g_theApp->GetTheGame()->GetCurrentScenario();
	const ItemList* items = current_scenario->GetItemList();
	const int num_items = static_cast<int>(items->size());

	g_theDevConsole->PrintString(Rgba::GREEN, "Items:");
	for (int item_idx = 0; item_idx < num_items; ++item_idx)
	{
		String item_name = items->at(item_idx).GetName();
		String line = item_name.c_str();

		StringList nn_list = items->at(item_idx).GetListOfNicknames();
		const int num_nn = static_cast<int>(nn_list.size());
		for (int nn_idx = 0; nn_idx < num_nn; ++nn_idx)
		{
			line += Stringf(" '%s'", nn_list[nn_idx].c_str());
		}

		g_theDevConsole->PrintString(Rgba::GREEN, Stringf("\t %s", line.c_str()));
	}

	return true;
}


STATIC bool DumpIncident(EventArgs& args)
{
	UNUSED(args);

	Scenario*	current_scenario = g_theApp->GetTheGame()->GetCurrentScenario();
	const IncidentList* incidences = current_scenario->GetIncidentList();
	const int num_incidences = static_cast<int>(incidences->size());

	g_theDevConsole->PrintString(Rgba::GREEN, "Incidences:");
	for (int incident_idx = 0; incident_idx < num_incidences; ++incident_idx)
	{
		String event_name = incidences->at(incident_idx).GetName();
		IncidentType event_type = incidences->at(incident_idx).GetType();
		bool is_enabled = incidences->at(incident_idx).IsIncidentEnabled();

		//print incident
		String line = event_name + " is a ";

		switch (event_type)
		{
		case INCIDENT_OCCUR_ONCE:
		{
			line += "OneShot event";
			break;
		}
		case INCIDENT_OCCUR_MULTIPLE:
		{
			line += "Multiple event";
			break;
		}
		default:
		{
			break;
		}
		}

		line += " that is currently ";

		if (is_enabled)
		{
			line += "enabled.";
		}
		else
		{
			line += "disabled.";
		}

		const TriggerList* triggers = incidences->at(incident_idx).GetTriggerList();
		const int num_triggers = static_cast<int>(triggers->size());
		for (int trigger_idx = 0; trigger_idx < num_triggers; ++trigger_idx)
		{
			String new_line = "\n                              ";
			String trigger_name = triggers->at(trigger_idx)->GetName();

			const ConditionList* conditions = triggers->at(trigger_idx)->GetConditionList();
			const ActionList* actions = triggers->at(trigger_idx)->GetActionList();

			const int num_conditions = static_cast<int>(conditions->size());
			const int num_actions = static_cast<int>(actions->size());

			new_line += Stringf("%s with %i conditions and %i actions", trigger_name.c_str(), num_conditions, num_actions);

			for (int condition_idx = 0; condition_idx < num_conditions; ++condition_idx)
			{
				String new_new_line = "\n                                 ";
				String condition_string = conditions->at(condition_idx)->GetAsString();
				new_new_line += condition_string;
				new_line += new_new_line;
			}

			for (int action_idx = 0; action_idx < num_actions; ++action_idx)
			{
				String new_new_line = "\n                                 ";
				String action_string = actions->at(action_idx)->GetAsString();
				new_new_line += action_string;
				new_line += new_new_line;
			}

			line += new_line;
		}

		g_theDevConsole->PrintString(Rgba::GREEN, Stringf("\t %s", line.c_str()));
	}

	return true;
}


// Game Actions ---------------------------------------------------------


STATIC bool TravelToLocation(EventArgs& args)
{
	// args will be card = "item name"
	Scenario* current_scenario = g_theApp->GetTheGame()->GetCurrentScenario();
	DialogueSystem* ds = g_theApp->GetTheGame()->GetDialogueSystem();

	Location* cur_loc = current_scenario->GetCurrentLocation();
	cur_loc->SetInvestigation(false);
	
	//find the location in case the player uses a nick name
	LookupItr loc_itr;
	String log = "\t";
	String loc_name = args.GetValue("card", String("something and nothing"));
	const bool is_in_list = current_scenario->IsLocationInLookupTable(loc_itr, String(loc_name));

	if (is_in_list)
	{
		Location* new_loc = current_scenario->GetLocationFromList(loc_itr->second);
		const bool valid_transition = new_loc->GetLocationDescription(log);

		if (valid_transition)
		{
			current_scenario->SetLocation(new_loc);
			current_scenario->SetInterest(nullptr);

			if(new_loc->GetLocationState().m_addGameTime)
			{
				current_scenario->AddGameTime(current_scenario->GetLocChangeTime(), 0);
			}
		}
		else
		{
			current_scenario->AddGameTime(current_scenario->GetWastingTime(), 0);
		}
	}
	else
	{
		log += g_closedLocationMessage;
		current_scenario->AddGameTime(current_scenario->GetWastingTime(), 0);
	}

	ds->AddLog(LOG_LOCATION, log);
	current_scenario->TestIncidents();
	current_scenario->TestVictoryConditions();

	return true;
}


STATIC bool AskLocationForCharacter(EventArgs& args)
{
	String		char_name = args.GetValue("card", String("something and nothing"));
	Scenario*	current_scenario = g_theApp->GetTheGame()->GetCurrentScenario();
	DialogueSystem* ds = g_theApp->GetTheGame()->GetDialogueSystem();

	//find the character in case the player is uses a nick name
	LookupItr char_itr;
	const bool is_char_in_list = current_scenario->IsCharacterInLookupTable(char_itr, String(char_name));

	String log = "\t";
	if (is_char_in_list)
	{
		Location* cur_loc = current_scenario->GetCurrentLocation();
		Character* char_subject = current_scenario->GetCharacterFromList(char_itr->second);

		const bool is_subject_here = cur_loc->IntroduceCharacter(log, char_subject);

		if (is_subject_here)
		{
			current_scenario->SetInterest(char_subject);

			if(char_subject->GetCharacterState().m_addGameTime)
			{
				current_scenario->AddGameTime(current_scenario->GetInterrogateChangeTime(), 0);
			}
		}
		else
		{
			current_scenario->AddGameTime(current_scenario->GetWastingTime(), 0);
		}
	}
	else
	{
		log += current_scenario->GetUnknownCharacter();
		current_scenario->AddGameTime(current_scenario->GetWastingTime(), 0);
	}

	ds->AddLog(LOG_CHARACTER, log);
	current_scenario->TestIncidents();
	current_scenario->TestVictoryConditions();

	return false;
}


STATIC bool AskLocationForItem(EventArgs& args)
{
	String		char_name = args.GetValue("card", String("something and nothing"));
	Scenario*	current_scenario = g_theApp->GetTheGame()->GetCurrentScenario();
	DialogueSystem* ds = g_theApp->GetTheGame()->GetDialogueSystem();

	//find the item in case the player is uses a nickname
	LookupItr item_itr;
	const bool is_item_in_list = current_scenario->IsItemInLookupTable(item_itr, String(char_name));

	String log = "\t";
	if (is_item_in_list)
	{
		Location* cur_loc = current_scenario->GetCurrentLocation();
		Item* item_subject = current_scenario->GetItemFromList(item_itr->second);

		const bool is_subject_here = cur_loc->IntroduceItem(log, item_subject);

		if (is_subject_here)
		{
			current_scenario->SetInterest(item_subject);

			if(item_subject->GetItemState().m_addGameTime)
			{
				current_scenario->AddGameTime(current_scenario->GetExamineItemChangeTime(), 0);
			}
		}
		else
		{
			current_scenario->AddGameTime(current_scenario->GetWastingTime(), 0);
		}
	}
	else
	{
		log += current_scenario->GetUnknownItem();
		current_scenario->AddGameTime(current_scenario->GetWastingTime(), 0);
	}

	ds->AddLog(LOG_ITEM, log);
	current_scenario->TestIncidents();
	current_scenario->TestVictoryConditions();


	return false;
}


STATIC bool InterrogateCharacter(EventArgs& args)
{
	// args will be card = "character name"
	String		name = args.GetValue("card", String("something and nothing"));
	Scenario*	current_scenario = g_theApp->GetTheGame()->GetCurrentScenario();
	DialogueSystem* ds = g_theApp->GetTheGame()->GetDialogueSystem();

	String log = "\t";

	Card* current_interest = current_scenario->GetCurrentInterest();

	if(current_interest == nullptr)
	{
		log += "> Cannot ask this now. Make sure to Talk to a character, then ask them a question.";
		current_scenario->AddGameTime(current_scenario->GetWastingTime(), 0);
	}
	else
	{
		const CardType interest_card_type = current_interest->GetCardType();

		if (interest_card_type != CARD_CHARACTER)
		{
			log += "> Cannot ask this now. Make sure to Talk to a character, then ask them a question.";
			current_scenario->AddGameTime(current_scenario->GetWastingTime(), 0);
		}
		else
		{
			LookupItr interrogatee_itr;
			current_scenario->IsCharacterInLookupTable(interrogatee_itr, current_interest->GetName());
			Character* interrogatee = current_scenario->GetCharacterFromList(interrogatee_itr->second);
			CharacterState interrogatee_state = interrogatee->GetCharacterState();
			
			if(interrogatee_state.m_contextMode == CONTEXT_NONE)
			{
				log += "> Cannot Interrogate this character right now.";
				current_scenario->AddGameTime(current_scenario->GetWastingTime(), 0);
			}
			else
			{
				Location* cur_loc = current_scenario->GetCurrentLocation();

				LookupItr card_itr;
				const bool is_char = current_scenario->IsCharacterInLookupTable(card_itr, String(name));

				if(is_char)
				{
					Character* about_char = current_scenario->GetCharacterFromList(card_itr->second);
					interrogatee->AskAboutCharacter(log, cur_loc, about_char);

					if(interrogatee->GetCharacterState().m_addGameTime)
					{
						current_scenario->AddGameTime(current_scenario->GetInterrogateChangeTime(), 0);
					}
				}
				else 
				{
					const bool is_item = current_scenario->IsItemInLookupTable(card_itr, String(name));

					if(is_item)
					{
						Item* about_item = current_scenario->GetItemFromList(card_itr->second);
						interrogatee->AskAboutItem(log, cur_loc, about_item);

						if (interrogatee->GetCharacterState().m_addGameTime)
						{
							current_scenario->AddGameTime(current_scenario->GetInterrogateChangeTime(), 0);
						}
					}
					else
					{
						log += "> Asking about an unknown item or card.";
						current_scenario->AddGameTime(current_scenario->GetWastingTime(), 0);
					}
				}
			}
		}
	}

	ds->AddLog(LOG_CHARACTER, log);
	current_scenario->TestIncidents();
	current_scenario->TestVictoryConditions();

	return true;
}


STATIC bool SayGoodbyToCharacter(EventArgs& args)
{
	String		name = args.GetValue("card", String("something and nothing"));
	Scenario*	current_scenario = g_theApp->GetTheGame()->GetCurrentScenario();

	current_scenario->SetInterest(nullptr);

	return true;
}


// this was assuming that the items acted like characters and locations, in that we can
// combine an item with something else to get more information
// STATIC bool InvestigateItem(EventArgs& args)
// {
// 	// args will be card = "item name"
// 	String name = args.GetValue("card", String("something and nothing"));
// 	Scenario*	current_scenario = g_theApp->GetTheGame()->GetCurrentScenario();
// 	DialogueSystem* ds = g_theApp->GetTheGame()->GetDialogueSystem();
// 
// 	LookupItr item_itr;
// 	String log = "\t";
// 	const bool is_in_list = current_scenario->IsItemInLookupTable(item_itr, String(name));
// 
// 	if (is_in_list)
// 	{
// 		current_scenario->GetItemFromList(item_itr->second)->SetDiscovery(true);
// 		log += current_scenario->GetItemFromList(item_itr->second)->GetDescription();
// 	}
// 	else
// 	{
// 		log += current_scenario->GetUnknownItem();
// 	}
// 
// 	ds->AddLog(LOG_ITEM, log);
// 	current_scenario->TestIncidents();
// 
// 	return true;
// }



STATIC bool InvestigateRoom(EventArgs& args)
{
	UNUSED(args);
	Scenario*	current_scenario = g_theApp->GetTheGame()->GetCurrentScenario();
	DialogueSystem* ds = g_theApp->GetTheGame()->GetDialogueSystem();

	Location* cur_location = current_scenario->GetCurrentLocation();

	if(cur_location->CanInvestigateLocation())
	{
		cur_location->SetInvestigation(true);
	}
	else
	{
		ds->AddLog(LOG_MESSAGE, "> Cannot Investigate this location");
	}

	return true;
}


STATIC bool LeaveRoom(EventArgs& args)
{
	UNUSED(args);
	Scenario*	current_scenario = g_theApp->GetTheGame()->GetCurrentScenario();
	DialogueSystem* ds = g_theApp->GetTheGame()->GetDialogueSystem();

	Location* cur_location = current_scenario->GetCurrentLocation();

	if (cur_location->CanInvestigateLocation() && cur_location->IsPlayerInvestigatingRoom())
	{
		cur_location->SetInvestigation(false);
	}
	else
	{
		ds->AddLog(LOG_MESSAGE, "> You're not investigating a room right now.");
	}

	return true;
}


STATIC bool SolveScenario(EventArgs& args)
{
	UNUSED(args);
	Scenario*	current_scenario = g_theApp->GetTheGame()->GetCurrentScenario();
	DialogueSystem* ds = g_theApp->GetTheGame()->GetDialogueSystem();

	Location* cur_location = current_scenario->GetCurrentLocation();

	if(cur_location->CanSolveCaseHere())
	{
		if(current_scenario->AreAllVictoryConditionsMet())
		{
			ds->AddLog(LOG_MESSAGE, "\t > " + current_scenario->GetCongratulations());
			ds->AddLog(LOG_MESSAGE, "\t > " + current_scenario->GetSolution());
			ds->AddLog(LOG_MESSAGE, "\t > Press any button to close the game...");
			current_scenario->ScenarioSolved();
		}
		else
		{
			ds->AddLog(LOG_MESSAGE, "> " + current_scenario->GetContinueInvestigation());
		}
	}
	else
	{
		ds->AddLog(LOG_MESSAGE, "> You cannot solve the case here, head back to HQ.");
	}


	return true;
}


STATIC bool ClearCommandDs(EventArgs& args)
{
	UNUSED(args);
	DialogueSystem* ds = g_theApp->GetTheGame()->GetDialogueSystem();
	ds->ClearLog();

	return true;
}


STATIC bool HelpCommandDs(EventArgs& args)
{
	UNUSED(args);
	DialogueSystem* ds = g_theApp->GetTheGame()->GetDialogueSystem();
	StringList event_names;
	g_theDialogueEventSystem->GetSubscribedEventsList(event_names);

	String log = "Valid Commands: \n";
	int num_events = static_cast<int>(event_names.size());
	for (int name_id = 0; name_id < num_events; ++name_id)
	{
		log += "\t" + StringToUpper(event_names[name_id]) + "\n";
	}

	ds->AddLog(LOG_MESSAGE, log);

	return true;
}


Scenario::Scenario(Game* the_game) : m_theGame(the_game) { }
Scenario::~Scenario() = default;


void Scenario::Startup()
{
	g_theEventSystem->SubscribeEventCallbackFunction("dump_locs", DumpLocations);
	g_theEventSystem->SubscribeEventCallbackFunction("dump_chars", DumpCharacter);
	g_theEventSystem->SubscribeEventCallbackFunction("dump_items", DumpItems);
	g_theEventSystem->SubscribeEventCallbackFunction("dump_events", DumpIncident);


	g_theDialogueEventSystem = new EventSystem();

	// when the player is interacting with a location
	g_theDialogueEventSystem->SubscribeEventCallbackFunction("goto", TravelToLocation);
	g_theDialogueEventSystem->SubscribeEventCallbackFunction("talk", AskLocationForCharacter);
	g_theDialogueEventSystem->SubscribeEventCallbackFunction("view", AskLocationForItem);

	// when the player is interacting with a character
	g_theDialogueEventSystem->SubscribeEventCallbackFunction("ask", InterrogateCharacter);
	g_theDialogueEventSystem->SubscribeEventCallbackFunction("goodbye", SayGoodbyToCharacter);

	// Investigating a room
	g_theDialogueEventSystem->SubscribeEventCallbackFunction("investigate", InvestigateRoom);
	g_theDialogueEventSystem->SubscribeEventCallbackFunction("leave", LeaveRoom);

	g_theDialogueEventSystem->SubscribeEventCallbackFunction("solve", SolveScenario);


	// when the player is interacting with an item
	//g_theDialogueEventSystem->SubscribeEventCallbackFunction("link", InvestigateItem);
	//g_theDialogueEventSystem->SubscribeEventCallbackFunction("shelve", InvestigateItem);

	// Dialogue System helper functions	
	g_theDialogueEventSystem->SubscribeEventCallbackFunction("clear", ClearCommandDs);
	g_theDialogueEventSystem->SubscribeEventCallbackFunction("help", HelpCommandDs);


	Vec2 frame_resolution = g_gameConfigBlackboard.GetValue(
		"Screensize",
		Vec2(720.0f, 1080.0f)
	);

	m_gameResolution[0] = frame_resolution.x;
	m_gameResolution[1] = frame_resolution.y;
	m_dialogWindowSize[0] = m_gameResolution[0] * 0.25f;
	m_dialogWindowSize[1] = m_gameResolution[1] * 0.25f;

	TestIncidents();
}


void Scenario::Shutdown()
{
	delete g_theDialogueEventSystem;
	g_theDialogueEventSystem = nullptr;
}


void Scenario::Update(const double delta_seconds)
{
	UNUSED(delta_seconds);

	m_imguiError = ImGui::Begin(
		"Game State",
		&m_show,
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
		ImVec2(0.0f, 0.0f),
		ImGuiCond_Always
	);

	//render game state
	String current_scenario = "Scenario: " + m_name;
	String current_location = "Location: " + m_currentLocation->GetName();
	String current_time = Stringf("On Day %01d at %02d:%02d", m_gameTime.m_day, m_gameTime.m_hour, m_gameTime.m_min);

	String current_interest = "Interested in: ";
	if (m_currentInterest == nullptr)
	{
		current_interest += "---";
	}
	else
	{
		current_interest += m_currentInterest->GetName();
	}

	String current_subject = "Subject is: ";
	if (m_currentSubject == nullptr)
	{
		current_subject += "---";
	}
	else
	{
		current_subject += m_currentSubject->GetName();
	}

	ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "%s", current_scenario.c_str());
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "%s", current_location.c_str());
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "%s", current_time.c_str());
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "%s", current_interest.c_str());
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 1.0f), "%s", current_subject.c_str());
	ImGui::End();

}


void Scenario::Render() const
{

	//render game assets
	m_currentLocation->Render();

	if (m_currentInterest != nullptr)
	{
		m_currentInterest->Render();
	}
}


void Scenario::LoadInScenarioManually()
{
	ManuallySetLocations();
	SetupLocationLookupTable();

	ManuallySetCharacters();
	SetupCharacterLookupTable();

	ManuallySetItems();
	SetupItemLookupTable();

	ManuallySetScenarioSettings();
}


void Scenario::LoadInScenarioFile(const char* folder_dir)
{
	const String location_file = String(folder_dir) + "/Locations.xml";
	ReadLocationsXml(location_file);
	SetupLocationLookupTable();

	const String characters_file = String(folder_dir) + "/Characters.xml";
	ReadCharactersXml(characters_file);
	SetupCharacterLookupTable();

	const String item_file = String(folder_dir) + "/Items.xml";
	ReadItemsXml(item_file);
	SetupItemLookupTable();

	const String settings_file = String(folder_dir) + "/Settings.xml";
	ReadSettingsXml(settings_file);
	
	const String incidents_file = String(folder_dir) + "/Incidents.xml";
	ReadIncidentsXml(incidents_file);
	SetupIncidentLookupTable();

	const String victory_conditions_file = String(folder_dir) + "/VictoryConditions.xml";
	ReadVictoryConditionsXml(victory_conditions_file);
}



bool Scenario::IsLocationInLookupTable(LookupItr& out, const String& name)
{
	const String name_to_lower = StringToLower(name);
	out = m_locationLookup.find(name_to_lower);

	if (out != m_locationLookup.end())
	{
		return true;
	}

	return false;
}


bool Scenario::IsCharacterInLookupTable(LookupItr& out, const String& name)
{
	const String name_to_lower = StringToLower(name);
	out = m_characterLookup.find(name_to_lower);

	if (out != m_characterLookup.end())
	{
		return true;
	}

	return false;
}


bool Scenario::IsItemInLookupTable(LookupItr& out, const String& name)
{
	const String name_to_lower = StringToLower(name);
	out = m_itemLookup.find(name_to_lower);

	if (out != m_itemLookup.end())
	{
		return true;
	}

	return false;
}


bool Scenario::IsIncidentInLookupTable(LookupItr& out, const String& name)
{
	const String name_to_lower = StringToLower(name);
	out = m_incidentLookup.find(name_to_lower);

	if (out != m_incidentLookup.end())
	{
		return true;
	}

	return false;
}


bool Scenario::IsCardInLookupTable(LookupItr& out_itr, CardType& out_type, const String& name)
{
	const String name_to_lower = StringToLower(name);
	
	out_itr = m_locationLookup.find(name_to_lower);
	if (out_itr != m_locationLookup.end())
	{
		out_type = CARD_LOCATION;
		return true;
	}
	
	out_itr = m_characterLookup.find(name_to_lower);
	if (out_itr != m_characterLookup.end())
	{
		out_type = CARD_CHARACTER;
		return true;
	}

	out_itr = m_itemLookup.find(name_to_lower);
	if (out_itr != m_itemLookup.end())
	{
		out_type = CARD_ITEM;
		return true;
	}

	return false;
}


Location* Scenario::GetLocationFromList(const int idx)
{
	return &(m_locations[idx]);
}


Character* Scenario::GetCharacterFromList(const int idx)
{
	return &(m_characters[idx]);
}


Item* Scenario::GetItemFromList(const int idx)
{
	return &(m_items[idx]);
}


Incident* Scenario::GetIncidentFromList(const int idx)
{
	return &(m_incidents[idx]);
}


String& Scenario::GetUnknownLocation()
{
	const int random_dialog_idx = g_randomNumberGenerator.GetRandomIntInRange(0, 2);
	return m_unknownLocationLine[random_dialog_idx];
}


String& Scenario::GetUnknownCharacter()
{
	const int random_dialog_idx = g_randomNumberGenerator.GetRandomIntInRange(0, 2);
	return m_unknownCharacterLine[random_dialog_idx];
}

String& Scenario::GetUnknownItem()
{
	const int random_dialog_idx = g_randomNumberGenerator.GetRandomIntInRange(0, 2);
	return m_unknownItemLine[random_dialog_idx];
}

String& Scenario::GetSolution()
{
	return m_solution;
}

String& Scenario::GetCongratulations()
{
	return m_congratulations;
}

String& Scenario::GetContinueInvestigation()
{
	return m_continueInvestigation;
}


Location* Scenario::GetCurrentLocation()
{
	return m_currentLocation;
}


Card* Scenario::GetCurrentInterest()
{
	return m_currentInterest;
}


void Scenario::SetLocation(Location* loc)
{
	m_currentLocation = loc;
}


void Scenario::SetInterest(Card* card)
{
	m_currentInterest = card;
}

void Scenario::ScenarioSolved()
{
	m_solved = true;
}


void Scenario::AddGameTime(const uint mins, const uint hours)
{
	m_gameTime.m_min += mins;
	m_gameTime.m_hour += hours;

	while (m_gameTime.m_min >= 60)
	{
		m_gameTime.m_min -= 60;
		m_gameTime.m_hour += 1;
	}

	while (m_gameTime.m_hour >= 24)
	{
		m_gameTime.m_hour -= 24;
		m_gameTime.m_day += 1;
	}
}


GameTime Scenario::GetCurrentTime() const
{
	return m_gameTime;
}


uint Scenario::GetLocChangeTime() const
{
	return m_costToMoveToLocation;
}


uint Scenario::GetExamineItemChangeTime() const
{
	return m_costToExamineItem;
}


uint Scenario::GetInterrogateChangeTime() const
{
	return m_costToInterrogateCharacter;
}


uint Scenario::GetWastingTime() const
{
	return m_costForUnknownCommand;
}


void Scenario::TestIncidents()
{
	const uint num_incidents = static_cast<uint>(m_incidents.size());

	for(uint inc_idx = 0; inc_idx < num_incidents; ++inc_idx)
	{
		m_incidents[inc_idx].TestTriggers();
	}
}


void Scenario::TestVictoryConditions()
{
	const uint num_conditions = static_cast<uint>(m_victoryConditions.size());

	for(uint condition_idx = 0; condition_idx < num_conditions; ++condition_idx)
	{
		m_victoryConditions[condition_idx].TestCondition();
	}
}


bool Scenario::AreAllVictoryConditionsMet() const
{
	const uint num_conditions = static_cast<uint>(m_victoryConditions.size());

	for (uint condition_idx = 0; condition_idx < num_conditions; ++condition_idx)
	{
		if(m_victoryConditions[condition_idx].HasConditionsBeenMet() == false)
		{
			return false;
		}
	}

	return true;
}


bool Scenario::IsScenarioSolved() const
{
	return m_solved;
}


void Scenario::ManuallySetScenarioSettings()
{
	m_gameTime.m_min = 0;
	m_gameTime.m_hour = 9;
	m_gameTime.m_day = 1;

	//AddGameTime(68, 50);

	LookupItr loc_itr;
	bool home = IsLocationInLookupTable(loc_itr, "Scotland Yard");
	if (home)
	{
		m_currentLocation = &m_locations[loc_itr->second];
	}

	ASSERT_OR_DIE(home, "Need to have a valid starting position");

	m_unknownLocationLine.emplace_back("Where is that again?");
	m_unknownLocationLine.emplace_back("...I think I'm lost...");
	m_unknownLocationLine.emplace_back("What city are we in again?");


	m_unknownCharacterLine.emplace_back("Sorry, give me a minute");
	m_unknownCharacterLine.emplace_back("I'm at a lost for words");
	m_unknownCharacterLine.emplace_back("............");

	m_unknownItemLine.emplace_back("oh, where did I put it...");
	m_unknownItemLine.emplace_back("I know it is here somewhere.");
	m_unknownItemLine.emplace_back("what was I looking for again?");
}


const LocationList* Scenario::GetLocationList() const
{
	return &m_locations;
}


const CharacterList* Scenario::GetCharacterList() const
{
	return &m_characters;
}


const ItemList* Scenario::GetItemList() const
{
	return &m_items;
}


const IncidentList* Scenario::GetIncidentList() const
{
	return &m_incidents;
}


void Scenario::ManuallySetLocations()
{
	// Home location 
	StringList home_nicknames;
	home_nicknames.emplace_back("Scotland Yard");
	home_nicknames.emplace_back("Home");
	home_nicknames.emplace_back("Station");
	String home_des = "You arrive at HQ, and your partner waves to you.";
	m_locations.emplace_back(this, "Lnd1_l01", home_nicknames, home_des);

	// Westminster location
	StringList westminster_nicknames;
	westminster_nicknames.emplace_back("Westminster");
	westminster_nicknames.emplace_back("A");
	westminster_nicknames.emplace_back("Bridge");
	String westminster_des = "You arrive at the crime scene. There is blood everywhere!";
	m_locations.emplace_back(this, "Lnd1_lA", westminster_nicknames, westminster_des);

	// The City location
	StringList city_nicknames;
	city_nicknames.emplace_back("The City");
	city_nicknames.emplace_back("B");
	city_nicknames.emplace_back("Town");
	String city_des = "You are in the heart of London.";
	m_locations.emplace_back(this, "Lnd1_lB", city_nicknames, city_des);
}


void Scenario::ManuallySetCharacters()
{
	StringList cally_nicknames;
	cally_nicknames.emplace_back("Cally");
	cally_nicknames.emplace_back("Rivera");
	String cally_desc = "I am on my way to work, what do you want?";
	m_characters.emplace_back(this, "lnd1_c01", cally_nicknames, cally_desc);

	StringList fannie_nicknames;
	fannie_nicknames.emplace_back("Fannie");
	fannie_nicknames.emplace_back("Pierce");
	String fannie_desc = "Shit, you wont take me alive!";
	m_characters.emplace_back(this, "lnd1_c02", fannie_nicknames, fannie_desc);

	StringList james_nicknames;
	james_nicknames.emplace_back("James");
	james_nicknames.emplace_back("Bond");
	String james_desc = "shaken not stirred";
	m_characters.emplace_back(this, "lnd1_c03", james_nicknames, james_desc);
}


void Scenario::ManuallySetItems()
{
	// Items used in the game
	StringList money_nicknames;
	money_nicknames.emplace_back("cash");
	money_nicknames.emplace_back("dough");
	money_nicknames.emplace_back("wad");
	std::string money_des = "A wad of cash";
	m_items.emplace_back(this, "money", money_nicknames, money_des);

	StringList poison_nicknames;
	poison_nicknames.emplace_back("poison");
	std::string poison_des = "A bottle of hydrochloric acid";
	m_items.emplace_back(this, "acid", poison_nicknames, poison_des);
}


void Scenario::ReadLocationsXml(const String& file_path)
{
	tinyxml2::XMLDocument loc_doc;
	OpenXmlFile(&loc_doc, file_path);
	XmlElement* root_loc = loc_doc.RootElement();
	uint loc_count = 0;

	for (const XmlElement* child = root_loc->FirstChildElement();
		child;
		child = child->NextSiblingElement())
	{
		loc_count++;
	}

	m_locations.reserve(loc_count);
	root_loc = loc_doc.RootElement();

	for (const XmlElement* loc_element = root_loc->FirstChildElement();
		loc_element;
		loc_element = loc_element->NextSiblingElement()
		)
	{
		//m_definitions[map_definition_element->FirstAttribute()->Value()] = new MapDefinition(*map_definition_element);
		m_locations.emplace_back(this, loc_element);
	}
}


void Scenario::ReadCharactersXml(const String& file_path)
{
	tinyxml2::XMLDocument characters_doc;
	OpenXmlFile(&characters_doc, file_path);
	XmlElement* root_characters = characters_doc.RootElement();
	uint char_count = 0;

	for (const XmlElement* child = root_characters->FirstChildElement();
		child;
		child = child->NextSiblingElement())
	{
		char_count++;
	}

	m_characters.reserve(char_count);

	for (const XmlElement* character_element = root_characters->FirstChildElement();
		character_element;
		character_element = character_element->NextSiblingElement()
		)
	{
		m_characters.emplace_back(this, character_element);
	}
}


void Scenario::ReadItemsXml(const String& file_path)
{
	tinyxml2::XMLDocument items_doc;
	OpenXmlFile(&items_doc, file_path);
	XmlElement* root_items = items_doc.RootElement();
	uint item_count = 0;

	for (const XmlElement* child = root_items->FirstChildElement();
		child;
		child = child->NextSiblingElement())
	{
		item_count++;
	}

	m_items.reserve(item_count);

	for (const XmlElement* item_element = root_items->FirstChildElement();
		item_element;
		item_element = item_element->NextSiblingElement()
		)
	{
		m_items.emplace_back(this, item_element);
	}
}


void Scenario::ReadSettingsXml(const String& file_path)
{
	tinyxml2::XMLDocument setup_doc;
	OpenXmlFile(&setup_doc, file_path);
	XmlElement* root_setup = setup_doc.RootElement();

	ReadScenarioSettingsAttributes(root_setup);

	// Get the children elements
	for (const XmlElement* setup_element = root_setup->FirstChildElement();
		setup_element;
		setup_element = setup_element->NextSiblingElement()
		)
	{
		String element_name = StringToLower(setup_element->Name());

		if (element_name == "timecostforactions")
		{
			ReadScenarioTimeCostForActions(setup_element);
		}
		else if (element_name == "starsscoresettings")
		{
			ReadScenarioStarsScoreSettings(setup_element);
		}
		else if (element_name == "gametimescorebonusandpenalty")
		{
			ReadScenarioGameTimeScoreBonusAndPenalty(setup_element);
		}
		else if (element_name == "defaultending")
		{
			ReadScenarioDefaultEnding(setup_element);
		}
	}

}


void Scenario::ReadIncidentsXml(const String& file_path)
{
	tinyxml2::XMLDocument incidents_doc;
	OpenXmlFile(&incidents_doc, file_path);
	XmlElement* root_incidents = incidents_doc.RootElement();
	uint incident_count = 0;

	for (const XmlElement* child = root_incidents->FirstChildElement();
		child;
		child = child->NextSiblingElement())
	{
		incident_count++;
	}

	m_incidents.reserve(incident_count);

	for (const XmlElement* incident_element = root_incidents->FirstChildElement();
		incident_element;
		incident_element = incident_element->NextSiblingElement()
		)
	{
		m_incidents.emplace_back(this, incident_element);
	}
}


void Scenario::ReadVictoryConditionsXml(const String& file_path)
{
	tinyxml2::XMLDocument incidents_doc;
	OpenXmlFile(&incidents_doc, file_path);
	XmlElement* root_incidents = incidents_doc.RootElement();
	uint conditions_count = 0;

	for (const XmlElement* child = root_incidents->FirstChildElement();
		child;
		child = child->NextSiblingElement())
	{
		conditions_count++;
	}

	m_victoryConditions.reserve(conditions_count);

	for (const XmlElement* condition_element = root_incidents->FirstChildElement();
		condition_element;
		condition_element = condition_element->NextSiblingElement()
		)
	{
		m_victoryConditions.emplace_back(this, condition_element);
	}
}


void Scenario::ReadScenarioSettingsAttributes(const XmlElement* element)
{
	// Get all the attributes from the ScenarioSettings element
	for (const XmlAttribute* attribute = element->FirstAttribute();
		attribute;
		attribute = attribute->Next()
		)
	{
		String attribute_name = StringToLower(attribute->Name());

		if (attribute_name == "name")
		{
			m_name = attribute->Value();
		}
		else if (attribute_name == "intromessage")
		{
			g_introMessage = attribute->Value();
		}
		else if (attribute_name == "closedlocationdefaultmessage")
		{
			g_closedLocationMessage = attribute->Value();
		}
		else if (attribute_name == "samelocationmessage")
		{
			g_sameLocationMessage = attribute->Value();
		}
		else if (attribute_name == "unknowncommand")
		{
			g_unknownCommandMessage = attribute->Value();
		}
		else if (attribute_name == "startinglocation")
		{
			LookupItr loc_itr;
			String starting_location_name = attribute->Value();

			bool home = IsLocationInLookupTable(loc_itr, starting_location_name);
			if (home)
			{
				m_currentLocation = &m_locations[loc_itr->second];
			}

			ASSERT_OR_DIE(home, "Need to have a valid starting position");
		}
		else if (attribute_name == "startupevent")
		{
			//TODO: read in after we make the scenario event class
		}
		else if (attribute_name == "startingtimeinmilitary")
		{
			m_gameTime.m_day = 0;

			String military_time_string = attribute->Value();

			StringList military_hour_minute = SplitStringOnDelimiter(military_time_string, ':');
			ASSERT_OR_DIE(military_hour_minute.size() == 2, "StartintgTimeInMilitary is not a valid time")

			m_gameTime.m_hour = std::stoul(military_hour_minute[0]);
			m_gameTime.m_min = std::stoul(military_hour_minute[1]);
		}
	}

	//will need to change the unknown lines
	m_unknownLocationLine.emplace_back("Where is that again?");
	m_unknownLocationLine.emplace_back("...I think I'm lost...");
	m_unknownLocationLine.emplace_back("What city are we in again?");

	m_unknownCharacterLine.emplace_back("Sorry, give me a minute");
	m_unknownCharacterLine.emplace_back("I'm at a lost for words");
	m_unknownCharacterLine.emplace_back("............");

	m_unknownItemLine.emplace_back("oh, where did I put it...");
	m_unknownItemLine.emplace_back("I know it is here somewhere.");
	m_unknownItemLine.emplace_back("what was I looking for again?");
}


void Scenario::ReadScenarioTimeCostForActions(const XmlElement* element)
{
	for (const XmlAttribute* attribute = element->FirstAttribute();
		attribute;
		attribute = attribute->Next()
		)
	{
		String attribute_name = StringToLower(attribute->Name());

		if (attribute_name == "movetolocation")
		{
			m_costToMoveToLocation = attribute->UnsignedValue();
		}
		else if (attribute_name == "investigatelocation")
		{
			m_costToInvestigateLocation = attribute->UnsignedValue();
		}
		else if (attribute_name == "examineitem")
		{
			m_costToExamineItem = attribute->UnsignedValue();
		}
		else if (attribute_name == "interrogate")
		{
			m_costToInterrogateCharacter = attribute->UnsignedValue();
		}
		else if (attribute_name == "unknowncommand")
		{
			m_costForUnknownCommand = attribute->UnsignedValue();
		}
	}
}


void Scenario::ReadScenarioStarsScoreSettings(const XmlElement* element)
{
	UNUSED(element);
	// TODO: set up scoring class
	//ERROR_AND_DIE("ReadScenarioStarsScoreSettings has yet to be made");
}


void Scenario::ReadScenarioGameTimeScoreBonusAndPenalty(const XmlElement* element)
{
	UNUSED(element);
	// TODO: set up scoring class
	//ERROR_AND_DIE("ReadScenarioGameTimeScoreBonusAndPenalty has yet to be made");
}

void Scenario::ReadScenarioDefaultEnding(const XmlElement* element)
{
	for (const XmlAttribute* attribute = element->FirstAttribute();
		attribute;
		attribute = attribute->Next()
		)
	{
		String attribute_name = StringToLower(attribute->Name());

		if (attribute_name == "congratulations")
		{
			m_congratulations = attribute->Value();
		}
		else if(attribute_name == "solution")
		{
			m_solution = attribute->Value();
		}
		else if (attribute_name == "continueinvestigation")
		{
			m_continueInvestigation = attribute->Value();
		}
		else
		{
			ERROR_AND_DIE(Stringf("Error in settings xml, %s is an unknown DefaultEnding attibute", attribute->Name()))
		}
	}

}


void Scenario::SetupLocationLookupTable()
{
	const int num_locations = static_cast<int>(m_locations.size());
	for (int loc_idx = 0; loc_idx < num_locations; ++loc_idx)
	{
		//reference to the name itself
		AddToLocationLookupTable(m_locations[loc_idx].GetName(), loc_idx);

		//reference for every nickname
		StringList nickname_list = m_locations[loc_idx].GetListOfNicknames();
		const int num_nicknames = static_cast<int>(nickname_list.size());
		for (int nn_idx = 0; nn_idx < num_nicknames; ++nn_idx)
		{
			AddToLocationLookupTable(nickname_list[nn_idx], loc_idx);
		}
	}
}


void Scenario::SetupCharacterLookupTable()
{
	const int num_characters = static_cast<int>(m_characters.size());
	for (int char_idx = 0; char_idx < num_characters; ++char_idx)
	{
		//reference to the name itself
		AddToCharacterLookupTable(m_characters[char_idx].GetName(), char_idx);

		//reference for every nickname
		StringList nickname_list = m_characters[char_idx].GetListOfNicknames();
		const int num_nicknames = static_cast<int>(nickname_list.size());
		for (int nn_idx = 0; nn_idx < num_nicknames; ++nn_idx)
		{
			AddToCharacterLookupTable(nickname_list[nn_idx], char_idx);
		}
	}
}


void Scenario::SetupItemLookupTable()
{
	const int num_items = static_cast<int>(m_items.size());
	for (int item_idx = 0; item_idx < num_items; ++item_idx)
	{
		//reference to the name itself
		AddToItemLookupTable(m_items[item_idx].GetName(), item_idx);

		//reference for every nickname
		StringList nickname_list = m_items[item_idx].GetListOfNicknames();
		const int num_nicknames = static_cast<int>(nickname_list.size());
		for (int nn_idx = 0; nn_idx < num_nicknames; ++nn_idx)
		{
			AddToItemLookupTable(nickname_list[nn_idx], item_idx);
		}
	}
}


void Scenario::SetupIncidentLookupTable()
{
	const int num_incidents = static_cast<int>(m_incidents.size());
	for (int inc_idx = 0; inc_idx < num_incidents; ++inc_idx)
	{
		//reference to the name itself
		AddToIncidentLookupTable(m_incidents[inc_idx].GetName(), inc_idx);
	}
}


void Scenario::AddToLocationLookupTable(const String& key_loc_name, int value_idx)
{
	LookupItr loc_itr;
	String name_to_lower = StringToLower(key_loc_name);
	const bool in_list_already = IsLocationInLookupTable(loc_itr, name_to_lower);
	ASSERT_OR_DIE(!in_list_already, Stringf("Duplicate Location name: %s", loc_itr->first.c_str()));
	m_locationLookup.insert(Lookup(name_to_lower, value_idx));
}


void Scenario::AddToCharacterLookupTable(const String& key_loc_name, int value_idx)
{
	LookupItr char_itr;
	String name_to_lower = StringToLower(key_loc_name);
	const bool in_list_already = IsCharacterInLookupTable(char_itr, name_to_lower);
	ASSERT_OR_DIE(!in_list_already, Stringf("Duplicate Character name: %s", char_itr->first.c_str()));
	m_characterLookup.insert(Lookup(name_to_lower, value_idx));
}


void Scenario::AddToItemLookupTable(const String& key_loc_name, int value_idx)
{
	LookupItr item_itr;
	String name_to_lower = StringToLower(key_loc_name);
	const bool in_list_already = IsItemInLookupTable(item_itr, name_to_lower);
	ASSERT_OR_DIE(!in_list_already, Stringf("Duplicate Item name: %s", item_itr->first.c_str()));
	m_itemLookup.insert(Lookup(name_to_lower, value_idx));
}


void Scenario::AddToIncidentLookupTable(const String& key_loc_name, int value_idx)
{
	LookupItr incident_itr;
	String name_to_lower = StringToLower(key_loc_name);
	const bool in_list_already = IsIncidentInLookupTable(incident_itr, name_to_lower);
	ASSERT_OR_DIE(!in_list_already, Stringf("Duplicate Incident name: %s", incident_itr->first.c_str()));
	m_incidentLookup.insert(Lookup(name_to_lower, value_idx));
}


void Scenario::OpenXmlFile(tinyxml2::XMLDocument* out, const String& file_path)
{
	const char* file_path_c_str = file_path.c_str();
	out->LoadFile(file_path_c_str);

	if (out->ErrorID() != tinyxml2::XML_SUCCESS)
	{
		std::string error_message = "\n";

		error_message.append(Stringf(">> ERROR loading XML doc \"%s\"\n", file_path_c_str));
		error_message.append(Stringf(">> errorID = %i\n", out->ErrorID()));
		error_message.append(Stringf(">> errorLineNum = %i\n", out->ErrorLineNum()));
		error_message.append(Stringf(">> errorName = \"%s\"\n", out->ErrorName()));

		ERROR_AND_DIE(error_message);
	}
}
