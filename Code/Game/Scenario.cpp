#include "Game/Scenario.hpp"
#include "Game/DialogueSystem.hpp"
#include "Game/Location.hpp"
#include "Game/Contact.hpp"
#include "Game/Character.hpp"
#include "Game/Evidence.hpp"
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


STATIC String Scenario::CallSpecialist(Scenario* the_setup, const char* name)
{
	LookupItr cont_itr;
	const bool is_in_list = the_setup->IsContactInLookupTable(cont_itr, String(name));
	
	if (is_in_list)
	{
		the_setup->m_contacts[cont_itr->second].SetDiscovery(true);
		return g_contactHeader + the_setup->m_contacts[cont_itr->second].GetDescription();
	}

	const int random_dialog_idx = g_randomNumberGenerator.GetRandomIntInRange(0, 2);
	return the_setup->m_unknownContactLine[random_dialog_idx];
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


String Scenario::InvestigateEvidence(Scenario* the_setup, const char* name)
{
	LookupItr evid_itr;
	const bool is_in_list = the_setup->IsEvidenceInLookupTable(evid_itr, String(name));

	if (is_in_list)
	{
		the_setup->m_evidence[evid_itr->second].SetDiscovery(true);
		return g_evidenceHeader + the_setup->m_evidence[evid_itr->second].GetDescription();
	}

	const int random_dialog_idx = g_randomNumberGenerator.GetRandomIntInRange(0, 2);
	return the_setup->m_unknownEvidenceLine[random_dialog_idx];
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


void Scenario::LoadInScenario()
{
	LoadInScenarioLocations();
	LoadInScenarioContacts();
	LoadInScenarioCharacters();
	LoadInScenarioEvidence();
	LoadInScenarioItems();
	LoadInScenarioSettings();
}


void Scenario::LoadInScenarioSettings()
{
	m_currentHourMilitary = 9;
	m_currentMinute = 0;

	LookupItr loc_itr;
	bool home = IsLocationInLookupTable(loc_itr, "home");
	if(home)
	{
		m_currentLocation = &m_locations[loc_itr->second];
	}

	ASSERT_OR_DIE(home, "Need to have a valid starting position");

}


void Scenario::LoadInScenarioLocations()
{
	ManuallySetLocations();
	SetupLocationLookupTable();
}


void Scenario::LoadInScenarioContacts()
{
	ManuallySetContacts();
	SetupContactLookupTable();
}


void Scenario::LoadInScenarioCharacters()
{
	ManuallySetCharacters();
	SetupCharacterLookupTable();
}


void Scenario::LoadInScenarioEvidence()
{
	ManuallySetEvidence();
	SetupEvidenceLookupTable();
}


void Scenario::LoadInScenarioItems()
{
	ManuallySetItems();
	SetupItemLookupTable();
}


void Scenario::ManuallySetLocations()
{
	// loading scenario unknown search lines
	m_unknownLocationLine.emplace_back("Where is that again?");
	m_unknownLocationLine.emplace_back("...I think I'm lost...");
	m_unknownLocationLine.emplace_back("What city are we in again?");

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


void Scenario::ManuallySetContacts()
{

	// loading scenario search lines
	m_unknownContactLine.emplace_back("*** Busy signal ***");
	m_unknownContactLine.emplace_back("*Sorry the number your trying to reach has been disconnected or no longer in service*");
	m_unknownContactLine.emplace_back("what was that number again...");
	
	// Criminologist contact 
	StringList hacker_nicknames;
	hacker_nicknames.emplace_back("Hacker");
	hacker_nicknames.emplace_back("Eric");
	hacker_nicknames.emplace_back("Gloomerry");
	hacker_nicknames.emplace_back("Intelligence Bureau");
	String hacker_des = "Hey this is Eric, from the Intelligence Bureau, how can I help?";
	m_contacts.emplace_back(this, "lnd1_p01", hacker_nicknames, hacker_des);

	StringList doctor_nicknames;
	doctor_nicknames.emplace_back("Doctor");
	doctor_nicknames.emplace_back("Jeremy");
	doctor_nicknames.emplace_back("Dr. King");
	doctor_nicknames.emplace_back("Morgue");
	String doctor_des = "Hello, this is Dr. King, from the Morgue, how can I help?";
	m_contacts.emplace_back(this, "lnd1_p02", doctor_nicknames, doctor_des);

	StringList criminologist_nicknames;
	criminologist_nicknames.emplace_back("Criminologist");
	criminologist_nicknames.emplace_back("Harvey");
	criminologist_nicknames.emplace_back("Marshall");
	criminologist_nicknames.emplace_back("Criminology Center");
	String criminologist_des = "Hello, this is Harvey, from the Criminology Center, how can I help?";
	m_contacts.emplace_back(this, "lnd1_p03", criminologist_nicknames, criminologist_des);

	StringList scientist_nicknames;
	scientist_nicknames.emplace_back("Scientist");
	scientist_nicknames.emplace_back("Lou");
	scientist_nicknames.emplace_back("Chin");
	scientist_nicknames.emplace_back("Forensic Laboratory");
	String scientist_des = "Hello, this is Lou, from the Forensic Laboratory, how can I help?";
	m_contacts.emplace_back(this, "lnd1_p04", scientist_nicknames, scientist_des);
}


void Scenario::ManuallySetCharacters()
{
	// loading scenario search lines
	m_unknownCharacterLine.emplace_back("Sorry, give me a minute");
	m_unknownCharacterLine.emplace_back("I'm at a lost for words");
	m_unknownCharacterLine.emplace_back("............");

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


void Scenario::ManuallySetEvidence()
{
	// loading scenario search lines
	m_unknownEvidenceLine.emplace_back("I don't think that's relevant");
	m_unknownEvidenceLine.emplace_back("I don't know about that");
	m_unknownEvidenceLine.emplace_back("I should keep looking");

	StringList laptop_nicknames;
	laptop_nicknames.emplace_back("laptop");
	laptop_nicknames.emplace_back("computer");
	laptop_nicknames.emplace_back("PC");
	String laptop_desc = "The Hacker's laptop";
	m_evidence.emplace_back(this, "lnd1_i01", laptop_nicknames, laptop_desc);

	StringList camera_nicknames;
	camera_nicknames.emplace_back("camera");
	String camera_desc = "a disposable camera";
	m_evidence.emplace_back(this, "lnd1_i02", camera_nicknames, camera_desc);
}


void Scenario::ManuallySetItems()
{
	// loading scenario search lines
	m_unknownItemLine.emplace_back("oh, where did I put it...");
	m_unknownItemLine.emplace_back("I know it is here somewhere.");
	m_unknownItemLine.emplace_back("what was I looking for again?");

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


void Scenario::ReadLocationsXml()
{
	ASSERT_OR_DIE(false, "ReadLocationsXml has not been implemented yet!")
}


void Scenario::ReadContactsXml()
{
	ASSERT_OR_DIE(false, "ReadContactsXML has not been implemented yet!")
}


void Scenario::ReadCharactersXml()
{
	ASSERT_OR_DIE(false, "ReadCharactersXml has not been implemented yet!")
}

void Scenario::ReadEvidenceXml()
{
	ASSERT_OR_DIE(false, "ReacdEvidenceXml has not been implemented yet!")
}


void Scenario::ReadItemsXml()
{
	ASSERT_OR_DIE(false, "ReacdEvidenceXml has not been implemented yet!")
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


void Scenario::SetupContactLookupTable()
{
	const int num_contacts = static_cast<int>(m_contacts.size());
	for (int cont_idx = 0; cont_idx < num_contacts; ++cont_idx)
	{
		//reference to the name itself
		AddToContactLookupTable(m_contacts[cont_idx].GetName(), cont_idx);

		//reference for every nickname
		StringList nickname_list = m_contacts[cont_idx].GetListOfNicknames();
		const int num_nicknames = static_cast<int>(nickname_list.size());
		for (int nn_idx = 0; nn_idx < num_nicknames; ++nn_idx)
		{
			AddToContactLookupTable(nickname_list[nn_idx], cont_idx);
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


void Scenario::SetupEvidenceLookupTable()
{
	const int num_evidences = static_cast<int>(m_evidence.size());
	for (int evid_idx = 0; evid_idx < num_evidences; ++evid_idx)
	{
		//reference to the name itself
		AddToEvidenceLookupTable(m_evidence[evid_idx].GetName(), evid_idx);

		//reference for every nickname
		StringList nickname_list = m_evidence[evid_idx].GetListOfNicknames();
		const int num_nicknames = static_cast<int>(nickname_list.size());
		for (int nn_idx = 0; nn_idx < num_nicknames; ++nn_idx)
		{
			AddToEvidenceLookupTable(nickname_list[nn_idx], evid_idx);
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


void Scenario::AddToContactLookupTable(const String& key_loc_name, int value_idx)
{
	LookupItr cont_itr;
	String name_to_lower = StringToLower(key_loc_name);
	const bool in_list_already = IsContactInLookupTable(cont_itr, name_to_lower);
	ASSERT_OR_DIE(!in_list_already, Stringf("Duplicate Contact name: %s", cont_itr->first.c_str()));
	m_contactLookup.insert(Lookup(name_to_lower, value_idx));
}


void Scenario::AddToCharacterLookupTable(const String& key_loc_name, int value_idx)
{
	LookupItr char_itr;
	String name_to_lower = StringToLower(key_loc_name);
	const bool in_list_already = IsCharacterInLookupTable(char_itr, name_to_lower);
	ASSERT_OR_DIE(!in_list_already, Stringf("Duplicate Character name: %s", char_itr->first.c_str()));
	m_characterLookup.insert(Lookup(name_to_lower, value_idx));
}


void Scenario::AddToEvidenceLookupTable(const String& key_loc_name, int value_idx)
{
	LookupItr evid_itr;
	String name_to_lower = StringToLower(key_loc_name);
	const bool in_list_already = IsEvidenceInLookupTable(evid_itr, name_to_lower);
	ASSERT_OR_DIE(!in_list_already, Stringf("Duplicate Evidence name: %s", evid_itr->first.c_str()));
	m_evidenceLookup.insert(Lookup(name_to_lower, value_idx));
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


bool Scenario::IsContactInLookupTable(LookupItr& out, const String& name)
{
	const String name_to_lower = StringToLower(name);
	out = m_contactLookup.find(name_to_lower);

	if (out != m_contactLookup.end())
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

bool Scenario::IsEvidenceInLookupTable(LookupItr& out, const String& name)
{
	const String name_to_lower = StringToLower(name);
	out = m_evidenceLookup.find(name_to_lower);

	if (out != m_evidenceLookup.end())
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
