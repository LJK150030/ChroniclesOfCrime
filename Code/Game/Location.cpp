#include "Engine/EngineCommon.hpp"
#include "Game/Location.hpp"

STATIC Location Location::s_locationRegions[NUM_LOCATIONS];
STATIC Location Location::s_currentLocation;
STATIC std::string Location::s_unknownLocationLine[3];

STATIC void Location::LoadInScenarioLocations()
{
	// leading scenario cards
	s_locationRegions[LOC_SCOTLAND_YARD] = Location(
		LOC_SCOTLAND_YARD,
		"station",
		"The police head quarters"
	);

	s_locationRegions[LOC_WESTMINSTER] = Location(
		LOC_WESTMINSTER,
		"apartment",
		"Angela's apartment"
	);


	// loading scenario search lines
	s_unknownLocationLine[0] = "Where is that again?";
	s_unknownLocationLine[1] = "...I think I'm lost...";
	s_unknownLocationLine[2] = "What city are we in again?";
}

STATIC std::string Location::TravelToLocation(const char* name)
{
	for (int location_idx = 0; location_idx < NUM_LOCATIONS; ++location_idx)
	{
		if (s_locationRegions[location_idx].m_name.compare(name) == 0)
		{
			s_locationRegions[location_idx].m_found = true;
			return g_locationHeader + s_locationRegions[location_idx].m_description;
		}
	}

	const int random_dialog_idx = g_randomNumberGenerator.GetRandomIntInRange(0, 2);
	return s_unknownLocationLine[random_dialog_idx];
}

Location::Location(LocationDistrict district, const std::string& name, const std::string& desc):
	m_category(district), m_name(name), m_description(desc)
{
}


