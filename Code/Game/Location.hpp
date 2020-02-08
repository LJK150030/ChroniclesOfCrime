#pragma once
#include "Game/Card.hpp"


class Location: public Card
{
public:
	static LocationList		s_locations;
	static LookupTable		s_locationLookup;
	static StringList		s_unknownLocationLine;
	static Location			s_currentLocation;
	
public:
	// setup Scenario
	static void LoadInScenarioLocations(Game* the_game);
	static void ManuallySetLocations(Game* the_game);
	static void ReadLocationsXml();
	static void SetupLocationLookupTable();

	// Scenario interaction functions
	static String TravelToLocation(const char* name);

private:
	static void AddToLocationLookupTable(const String& key_loc_name, int value_idx);
	static bool IsLocationInLookupTable(LookupItr& out, const String& name);
	
public:
	Location();
	explicit Location(Game* the_game);
	explicit Location(Game* the_game, const String& name, const StringList& list_of_nicknames, 
		const String& desc);
	~Location() = default;

	// ACCESSORS

	
	// MUTATORS
	
};
