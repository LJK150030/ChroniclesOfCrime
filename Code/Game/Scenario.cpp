#include "Game/Scenario.hpp"
#include "Game/DialogueSystem.hpp"
#include "Game/Location.hpp"
#include "Game/Character.hpp"
#include "Game/Item.hpp"


#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"


STATIC String Scenario::TravelToLocation(Scenario* the_setup, const char* name)
{
	LookupItr loc_itr;
	const bool is_in_list = the_setup->IsLocationInLookupTable(loc_itr, String(name));

	if (is_in_list)
	{
		the_setup->m_locations[loc_itr->second].SetDiscovery(true);
		the_setup->m_currentLocation = &the_setup->m_locations[loc_itr->second];
		return g_locationHeader + the_setup->m_locations[loc_itr->second].GetDescription();
	}

	const int random_dialog_idx = g_randomNumberGenerator.GetRandomIntInRange(0, 2);
	return the_setup->m_unknownLocationLine[random_dialog_idx];
}



STATIC String Scenario::InterrogateCharacter(Scenario* the_setup, const char* name)
{
	LookupItr char_itr;
	const bool is_in_list = the_setup->IsCharacterInLookupTable(char_itr, String(name));

	if (is_in_list)
	{
		the_setup->m_characters[char_itr->second].SetDiscovery(true);
		return g_characterHeader + the_setup->m_characters[char_itr->second].GetDescription();
	}

	const int random_dialog_idx = g_randomNumberGenerator.GetRandomIntInRange(0, 2);
	return the_setup->m_unknownCharacterLine[random_dialog_idx];
}


String Scenario::InvestigateItem(Scenario* the_setup, const char* name)
{
	LookupItr item_itr;
	const bool is_in_list = the_setup->IsItemInLookupTable(item_itr, String(name));

	if (is_in_list)
	{
		the_setup->m_items[item_itr->second].SetDiscovery(true);
		return g_itemHeader + the_setup->m_items[item_itr->second].GetDescription();
	}

	const int random_dialog_idx = g_randomNumberGenerator.GetRandomIntInRange(0, 2);
	return the_setup->m_unknownItemLine[random_dialog_idx];
}


Scenario::Scenario(Game* the_game) : m_theGame(the_game) { }
Scenario::~Scenario() = default;


void Scenario::Startup()
{

}


void Scenario::Shutdown()
{

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
	
	ManuallySetScenarioSettings();
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
