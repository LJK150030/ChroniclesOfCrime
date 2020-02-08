#include "Game/Scenario.hpp"
#include "Game/DialogueSystem.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"


STATIC String Scenario::TravelToLocation(Scenario* sce, const char* name)
{
	LookupItr loc_itr;
	const bool is_in_list = sce->IsLocationInLookupTable(loc_itr, String(name));

	if (is_in_list)
	{
		sce->m_locations[loc_itr->second].SetDiscovery(true);
		return g_locationHeader + sce->m_locations[loc_itr->second].GetDescription();
	}

	const int random_dialog_idx = g_randomNumberGenerator.GetRandomIntInRange(0, 2);
	return sce->m_unknownLocationLine[random_dialog_idx];
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
	//	Evidence::LoadInScenarioEvidence();
	// 	Item::LoadInScenarioItems();
	// 	Character::LoadInScenarioCharacters();
	// 	Contact::LoadInScenarioContacts();
}


void Scenario::LoadInScenarioLocations(Scenario* the_game)
{
	ManuallySetLocations(the_game);
	SetupLocationLookupTable();
}


void Scenario::ManuallySetLocations(Scenario* the_game)
{
	// loading scenario unknown search lines
	m_unknownLocationLine.emplace_back("Where is that again?");
	m_unknownLocationLine.emplace_back("...I think I'm lost...");
	m_unknownLocationLine.emplace_back("What city are we in again?");

	// Home location 
	StringList home_nicknames;
	home_nicknames.emplace_back("Station");
	home_nicknames.emplace_back("Headquarters");
	home_nicknames.emplace_back("Police Station");
	String home_des = "You arrive at HQ, and your partner waves to you.";
	m_locations.emplace_back(the_game, "Home", home_nicknames, home_des);

	// Westminster location
	StringList crime_nicknames;
	crime_nicknames.emplace_back("Apartment");
	crime_nicknames.emplace_back("Victim's home");
	String westminster_des = "You arrive at the crime scene. There is blood everywhere!";
	m_locations.emplace_back(the_game, "Westminster", crime_nicknames, westminster_des);
}


void Scenario::ReadLocationsXml()
{
	ASSERT_OR_DIE(false, "ReadLocationsXml has not been implemented yet!")
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





void Scenario::AddToLocationLookupTable(const String& key_loc_name, int value_idx)
{
	LookupItr loc_itr;
	String name_to_lower = StringToLower(key_loc_name);
	const bool in_list_already = IsLocationInLookupTable(loc_itr, name_to_lower);
	ASSERT_OR_DIE(in_list_already == false, Stringf("Duplicate Location name: %s", loc_itr->first.c_str()));
	m_locationLookup.insert(Lookup(name_to_lower, value_idx));
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