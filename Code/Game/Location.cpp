#include "Game/Location.hpp"
#include "Game/Gamecommon.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"

STATIC LocationList		Location::s_locations = LocationList();
STATIC LookupTable		Location::s_locationLookup = LookupTable();
STATIC StringList		Location::s_unknownLocationLine = StringList();
STATIC Location			Location::s_currentLocation;


STATIC void Location::LoadInScenarioLocations(Game* the_game)
{
	ManuallySetLocations(the_game);
	SetupLocationLookupTable();
}


STATIC void Location::ManuallySetLocations(Game* the_game)
{
	// loading scenario unknown search lines
	s_unknownLocationLine.emplace_back("Where is that again?");
	s_unknownLocationLine.emplace_back("...I think I'm lost...");
	s_unknownLocationLine.emplace_back("What city are we in again?");

	// Home location 
	StringList home_nicknames;
	home_nicknames.emplace_back("Station");
	home_nicknames.emplace_back("Headquarters");
	home_nicknames.emplace_back("Police Station");
	String home_des = "You arrive at HQ, and your partner waves to you.";
	s_locations.emplace_back(the_game, "Home", home_nicknames, home_des);

	// Westminster location
	StringList crime_nicknames;
	crime_nicknames.emplace_back("Apartment");
	crime_nicknames.emplace_back("Victim's home");
	String westminster_des = "You arrive at the crime scene. There is blood everywhere!";
	s_locations.emplace_back(the_game, "Westminster", crime_nicknames, westminster_des);
}


STATIC void Location::ReadLocationsXml()
{
	ASSERT_OR_DIE(false, "ReadLocationsXml has not been implemented yet!")
}


STATIC void Location::SetupLocationLookupTable()
{
	const int num_locations = static_cast<int>(s_locations.size());
	for (int loc_idx = 0; loc_idx < num_locations; ++loc_idx)
	{
		//reference to the name itself
		AddToLocationLookupTable(s_locations[loc_idx].GetName(), loc_idx);

		//reference for every nickname
		StringList nickname_list = s_locations[loc_idx].GetListOfNicknames();
		const int num_nicknames = static_cast<int>(nickname_list.size());
		for(int nn_idx = 0; nn_idx < num_nicknames; ++nn_idx)
		{
			AddToLocationLookupTable(nickname_list[nn_idx], loc_idx);
		}
	}
}


STATIC std::string Location::TravelToLocation(const char* name)
{
	LookupItr loc_itr;
	const bool is_in_list = IsLocationInLookupTable(loc_itr, String(name));

	if(is_in_list)
	{
		s_locations[loc_itr->second].m_found = true;
		return g_locationHeader + s_locations[loc_itr->second].m_description;
	}

	const int random_dialog_idx = g_randomNumberGenerator.GetRandomIntInRange(0, 2);
	return s_unknownLocationLine[random_dialog_idx];
}


STATIC void Location::AddToLocationLookupTable(const String& key_loc_name, int value_idx)
{
	LookupItr loc_itr;
	String name_to_lower = StringToLower(key_loc_name);
	const bool in_list_already = IsLocationInLookupTable(loc_itr, name_to_lower);
	ASSERT_OR_DIE(in_list_already == false, Stringf("Duplicate Location name: %s", loc_itr->first.c_str()));
	s_locationLookup.insert(Lookup(name_to_lower, value_idx));
}


STATIC  bool Location::IsLocationInLookupTable(LookupItr& out, const String& name)
{
	const String name_to_lower = StringToLower(name);
	out = s_locationLookup.find(name_to_lower);
	
	if (out != s_locationLookup.end())
	{
		return true;
	}

	return false;
}

Location::Location() { m_type = CARD_LOCATION; }
Location::Location(Game* the_game) : Card(the_game, CARD_LOCATION) { }
Location::Location(Game* the_game, const String& name, const StringList& list_of_nicknames, 
	const String& desc) : Card(the_game, CARD_LOCATION, name, list_of_nicknames, desc) { }


