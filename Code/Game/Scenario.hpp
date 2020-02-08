#pragma once
#include "Game/GameCommon.hpp"
#include "Game/Location.hpp"

#include "Engine/Math/Matrix44.hpp"


class Game;
class Material;
class Shader;
class GPUMesh;
class DialogueSystem;


class Scenario
{
	friend class DialogueSystem;
public:
	// Scenario interaction functions
	//TODO: only static so that DialogueSystem can get to it. Use EventSystem
	static String TravelToLocation(Scenario* sce, const char* name);
	
public:
	Scenario(Game* the_game);
	~Scenario();

	void Startup();
	void Shutdown();

	void LoadInScenario();

private:
	// setup Scenario
	void LoadInScenarioLocations(Scenario* the_game);
	void ManuallySetLocations(Scenario* the_game);
	void ReadLocationsXml();
	void SetupLocationLookupTable();

	// Helpper functions
	void AddToLocationLookupTable(const String& key_loc_name, int value_idx);
	bool IsLocationInLookupTable(LookupItr& out, const String& name);
	
private:
	// Owner
	Game*			m_theGame = nullptr;
	
	// Game state data
	Location		m_currentLocation;

	
	// Physical representation of Entities in the game
	LocationList	m_locations;
	//ContactList		m_contacts;

	
	// To quickly lookup where an item is in it's respective list
	LookupTable		m_locationLookup;
	//LookupTable		m_contactsLookup;

	
	// Random lines to say when the player writes an unknown name
	StringList		m_unknownLocationLine;
	//StringList		m_unknownContactLine;
};
