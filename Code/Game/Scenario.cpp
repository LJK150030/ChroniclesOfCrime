#include "Game/Scenario.hpp"
#include "Game/DialogueSystem.hpp"
#include "Game/Location.hpp"
#include "Game/Character.hpp"
#include "Game/Item.hpp"

#include "Game/App.hpp"
#include "Game/Game.hpp"
#include "Game/DialogueSystem.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Renderer/ImGUISystem.hpp"

// Debugging ------------------------------------------------------------


STATIC bool DumpLocations(EventArgs& args)
{
	UNUSED(args);

	Scenario*	current_scenario = g_theApp->GetTheGame()->GetCurrentScenario();
	LocationList locations = current_scenario->GetLocationList();
	int num_locs = static_cast<int>(locations.size());

	g_theDevConsole->PrintString(Rgba::GREEN, "Locations:");
	for(int loc_idx = 0; loc_idx < num_locs; ++loc_idx)
	{
		String loc_name = locations[loc_idx].GetName();
		String line = loc_name.c_str();

		StringList nn_list = locations[loc_idx].GetListOfNicknames();
		int num_nn = static_cast<int>(nn_list.size());
		for(int nn_idx = 0; nn_idx < num_nn; ++nn_idx)
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
	CharacterList characters = current_scenario->GetCharacterList();
	int num_char = static_cast<int>(characters.size());

	g_theDevConsole->PrintString(Rgba::GREEN, "Characters:");
	for (int char_idx = 0; char_idx < num_char; ++char_idx)
	{
		String char_name = characters[char_idx].GetName();
		String line = char_name.c_str();

		StringList nn_list = characters[char_idx].GetListOfNicknames();
		int num_nn = static_cast<int>(nn_list.size());
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
	ItemList items = current_scenario->GetItemList();
	int num_items = static_cast<int>(items.size());

	g_theDevConsole->PrintString(Rgba::GREEN, "Items:");
	for (int item_idx = 0; item_idx < num_items; ++item_idx)
	{
		String item_name = items[item_idx].GetName();
		String line = item_name.c_str();

		StringList nn_list = items[item_idx].GetListOfNicknames();
		int num_nn = static_cast<int>(nn_list.size());
		for (int nn_idx = 0; nn_idx < num_nn; ++nn_idx)
		{
			line += Stringf(" '%s'", nn_list[nn_idx].c_str());
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

	//find the location in case the player uses a nick name
	LookupItr loc_itr;
	String log;
	String loc_name = args.GetValue("card", String("something and nothing"));
	const bool is_in_list = current_scenario->IsLocationInLookupTable(loc_itr, String(loc_name));

	if (is_in_list)
	{
		log = current_scenario->GetLocationFromList(loc_itr->second)->GetLocationDescription();
	}
	else
	{
		log = current_scenario->GetUnknownLocation();
	}

	ds->AddLog(LOG_LOCATION, log);

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

	String log;
	if (is_char_in_list)
	{
		Location* cur_loc = current_scenario->GetCurrentLocation();
		Character* char_subject = current_scenario->GetCharacterFromList(char_itr->second);
		log = cur_loc->IntroduceCharacter(char_subject);
	}
	else
	{
		log = current_scenario->GetUnknownCharacter();
	}

	ds->AddLog(LOG_CHARACTER, log);

	return false;
}


STATIC bool AskLocationForItem(EventArgs& args)
{
	return false;
}


STATIC bool InterrogateCharacter(EventArgs& args)
{
	// args will be card = "character name"
	String		name = args.GetValue("card", String("something and nothing"));
	Scenario*	current_scenario = g_theApp->GetTheGame()->GetCurrentScenario();
	DialogueSystem* ds = g_theApp->GetTheGame()->GetDialogueSystem();

	LookupItr char_itr;
	String log;
	const bool is_in_list = current_scenario->IsCharacterInLookupTable(char_itr, String(name));

	// 	Location* cur_loc = current_scenario->GetCurrentLocation();
	// 	cur_loc->IsCharacterInLocation()

	if (is_in_list)
	{
		current_scenario->GetCharacterFromList(char_itr->second)->SetDiscovery(true);
		log = current_scenario->GetCharacterFromList(char_itr->second)->GetDescription();
	}
	else
	{
		log = current_scenario->GetUnknownCharacter();
	}

	ds->AddLog(LOG_CHARACTER, log);

	return true;
}


STATIC bool InvestigateItem(EventArgs& args)
{
	// args will be card = "item name"
	String name = args.GetValue("card", String("something and nothing"));
	Scenario*	current_scenario = g_theApp->GetTheGame()->GetCurrentScenario();
	DialogueSystem* ds = g_theApp->GetTheGame()->GetDialogueSystem();

	LookupItr item_itr;
	String log;
	const bool is_in_list = current_scenario->IsItemInLookupTable(item_itr, String(name));

	if (is_in_list)
	{
		current_scenario->GetItemFromList(item_itr->second)->SetDiscovery(true);
		log = current_scenario->GetItemFromList(item_itr->second)->GetDescription();
	}
	else
	{
		log = current_scenario->GetUnknownItem();	
	}

	ds->AddLog(LOG_ITEM, log);

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
	for(int name_id = 0; name_id < num_events; ++name_id)
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

	
	g_theDialogueEventSystem = new EventSystem();
	
	// when the player is interacting with a location
	g_theDialogueEventSystem->SubscribeEventCallbackFunction("goto", TravelToLocation);
	g_theDialogueEventSystem->SubscribeEventCallbackFunction("talk", AskLocationForCharacter);
	g_theDialogueEventSystem->SubscribeEventCallbackFunction("view", AskLocationForItem);
	
	// when the player is interacting with a character
	//g_theDialogueEventSystem->SubscribeEventCallbackFunction("ask", InterrogateCharacter);
	//g_theDialogueEventSystem->SubscribeEventCallbackFunction("goodbye", InterrogateCharacter);

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
	String current_location = "Current Location: " + m_currentLocation->GetName();
	ImGui::TextColored(ImVec4(1.0f, 0.0f, 1.0f, 1.0f), current_location.c_str());
	ImGui::End();

}


void Scenario::Render() const
{

	//render game assets
	m_currentLocation->Render();
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
// 
// 	const String item_file = String(folder_dir) + "/Items.xml";
// 	ReadItemsXml(item_file);
// 	SetupItemLookupTable();
	
	ManuallySetScenarioSettings();
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


Location* Scenario::GetCurrentLocation()
{
	return m_currentLocation;
}


Card* Scenario::GetCurrentInterest()
{
	return m_currentInterest;
}


void Scenario::ManuallySetScenarioSettings()
{
	m_currentHourMilitary = 9;
	m_currentMinute = 0;

	LookupItr loc_itr;
	bool home = IsLocationInLookupTable(loc_itr, "Scotland Yard");
	if(home)
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


LocationList Scenario::GetLocationList()
{
	return m_locations;
}


CharacterList Scenario::GetCharacterList()
{
	return m_characters;
}


ItemList Scenario::GetItemList()
{
	return m_items;
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

	for (const XmlElement* item_element = root_items->FirstChildElement();
		item_element;
		item_element = item_element->NextSiblingElement()
		)
	{
		m_items.emplace_back(this, item_element);
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
