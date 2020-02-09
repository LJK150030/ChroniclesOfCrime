#pragma once
#include "Game/GameCommon.hpp"

#include "Engine/Math/Matrix44.hpp"


class Game;
class Material;
class Shader;
class GPUMesh;
class DialogueSystem;

class Location;
class Contact;


class Scenario
{
	friend class DialogueSystem;
public:
	// Scenario interaction functions
	//TODO: only static so that DialogueSystem can get to it. Use EventSystem
	static String TravelToLocation(Scenario* the_setup, const char* name);
	static String CallSpecialist(Scenario* the_setup, const char* name);
	
public:
	Scenario(Game* the_game);
	~Scenario();

	void Startup();
	void Shutdown();

	void LoadInScenario();

private:
	// setup Scenario
	void LoadInScenarioLocations(Scenario* the_setup);
	void LoadInScenarioContacts(Scenario* the_setup);

	
	void ManuallySetLocations(Scenario* the_setup);
	void ManuallySetContacts(Scenario* the_setup);

	
	void ReadLocationsXml();
	void ReadContactsXML();

	// Database manipulation
	void SetupLocationLookupTable();
	void SetupContactLookupTable();

	
	void AddToLocationLookupTable(const String& key_loc_name, int value_idx);
	void AddToContactLookupTable(const String& key_loc_name, int value_idx_);
	
	bool IsLocationInLookupTable(LookupItr& out, const String& name);
	bool IsContactInLookupTable(LookupItr& out, const String& name);
	
	
private:
	// Owner
	Game*			m_theGame = nullptr;
	
	// Game state data
	Location*		m_currentLocation = nullptr;

	
	// Physical representation of Entities in the game
	LocationList	m_locations;
	ContactList		m_contacts;

	
	// To quickly lookup where an item is in it's respective list
	LookupTable		m_locationLookup;
	LookupTable		m_contactsLookup;

	
	// Random lines to say when the player writes an unknown name
	StringList		m_unknownLocationLine;
	StringList		m_unknownContactLine;
};
