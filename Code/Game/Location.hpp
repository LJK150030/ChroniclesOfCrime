#pragma once
#include "Game/GameNames.hpp"
#include <string>

class Location
{
public:
	static Location		s_locationRegions[NUM_LOCATIONS];
	static Location		s_currentLocation;
	static std::string	s_unknownLocationLine[3];

public:
	static void LoadInScenarioLocations();
	static std::string TravelToLocation(const char* name);

public:
	Location() = default;
	Location(LocationDistrict district, const std::string& name, const std::string& desc);

	~Location() = default;

private:
	LocationDistrict	m_category = UNKNOWN_LOC;
	bool				m_found = false;
	std::string			m_name = "";
	std::string			m_description = "";

};