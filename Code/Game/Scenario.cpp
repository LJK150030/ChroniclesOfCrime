#include "Game/Scenario.hpp"
#include "Game/DialogueSystem.hpp"
#include "Game/Location.hpp"
#include "Game/Contact.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"


STATIC String Scenario::TravelToLocation(Scenario* the_setup, const char* name)
{
	LookupItr loc_itr;
	const bool is_in_list = the_setup->IsLocationInLookupTable(loc_itr, String(name));

	if (is_in_list)
	{
		the_setup->m_locations[loc_itr->second].SetDiscovery(true);
		return g_locationHeader + the_setup->m_locations[loc_itr->second].GetDescription();
	}

	const int random_dialog_idx = g_randomNumberGenerator.GetRandomIntInRange(0, 2);
	return the_setup->m_unknownLocationLine[random_dialog_idx];
}


String Scenario::CallSpecialist(Scenario* the_setup, const char* name)
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
	LoadInScenarioLocations(this);
	LoadInScenarioContacts(this);
	//	Evidence::LoadInScenarioEvidence();
	// 	Item::LoadInScenarioItems();
	// 	Character::LoadInScenarioCharacters();
	// 	Contact::LoadInScenarioContacts();
}


void Scenario::LoadInScenarioLocations(Scenario* the_setup)
{
	ManuallySetLocations(the_setup);
	SetupLocationLookupTable();
}


void Scenario::LoadInScenarioContacts(Scenario* the_setup)
{
	ManuallySetContacts(the_setup);
	SetupContactLookupTable();
}


void Scenario::ManuallySetLocations(Scenario* the_setup)
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
	m_locations.emplace_back(the_setup, "Lnd1_l01", home_nicknames, home_des);

	// Westminster location
	StringList westminster_nicknames;
	westminster_nicknames.emplace_back("Westminster");
	westminster_nicknames.emplace_back("A");
	westminster_nicknames.emplace_back("Bridge");
	String westminster_des = "You arrive at the crime scene. There is blood everywhere!";
	m_locations.emplace_back(the_setup, "Lnd1_lA", westminster_nicknames, westminster_des);

	// The City location
	StringList city_nicknames;
	city_nicknames.emplace_back("The City");
	city_nicknames.emplace_back("B");
	city_nicknames.emplace_back("Town");
	String city_des = "You are in the heart of London.";
	m_locations.emplace_back(the_setup, "Lnd1_lB", city_nicknames, city_des);
}


void Scenario::ManuallySetContacts(Scenario* the_setup)
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
	m_contacts.emplace_back(the_setup, "lnd1_p01", hacker_nicknames, hacker_des);

	StringList doctor_nicknames;
	doctor_nicknames.emplace_back("Doctor");
	doctor_nicknames.emplace_back("Jeremy");
	doctor_nicknames.emplace_back("Dr. King");
	doctor_nicknames.emplace_back("Morgue");
	String doctor_des = "Hello, this is Dr. King, from the Morgue, how can I help?";
	m_contacts.emplace_back(the_setup, "lnd1_p02", doctor_nicknames, doctor_des);

	StringList criminologist_nicknames;
	criminologist_nicknames.emplace_back("Criminologist");
	criminologist_nicknames.emplace_back("Harvey");
	criminologist_nicknames.emplace_back("Marshall");
	criminologist_nicknames.emplace_back("Criminology Center");
	String criminologist_des = "Hello, this is Harvey, from the Criminology Center, how can I help?";
	m_contacts.emplace_back(the_setup, "lnd1_p03", criminologist_nicknames, criminologist_des);

	StringList scientist_nicknames;
	scientist_nicknames.emplace_back("Scientist");
	scientist_nicknames.emplace_back("Lou");
	scientist_nicknames.emplace_back("Chin");
	scientist_nicknames.emplace_back("Forensic Laboratory");
	String scientist_des = "Hello, this is Lou, from the Forensic Laboratory, how can I help?";
	m_contacts.emplace_back(the_setup, "lnd1_p04", scientist_nicknames, scientist_des);
}


void Scenario::ReadLocationsXml()
{
	ASSERT_OR_DIE(false, "ReadLocationsXml has not been implemented yet!")
}


void Scenario::ReadContactsXML()
{
	ASSERT_OR_DIE(false, "ReadContactsXML has not been implemented yet!")
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


void Scenario::AddToLocationLookupTable(const String& key_loc_name, int value_idx)
{
	LookupItr loc_itr;
	String name_to_lower = StringToLower(key_loc_name);
	const bool in_list_already = IsLocationInLookupTable(loc_itr, name_to_lower);
	ASSERT_OR_DIE(in_list_already == false, Stringf("Duplicate Location name: %s", loc_itr->first.c_str()));
	m_locationLookup.insert(Lookup(name_to_lower, value_idx));
}


void Scenario::AddToContactLookupTable(const String& key_loc_name, int value_idx)
{
	LookupItr cont_itr;
	String name_to_lower = StringToLower(key_loc_name);
	const bool in_list_already = IsContactInLookupTable(cont_itr, name_to_lower);
	ASSERT_OR_DIE(in_list_already == false, Stringf("Duplicate Contact name: %s", cont_itr->first.c_str()));
	m_contactsLookup.insert(Lookup(name_to_lower, value_idx));
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
	out = m_contactsLookup.find(name_to_lower);

	if (out != m_contactsLookup.end())
	{
		return true;
	}

	return false;
}