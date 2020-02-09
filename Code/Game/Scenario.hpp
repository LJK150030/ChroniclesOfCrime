#pragma once
#include "Game/GameCommon.hpp"

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
	static String TravelToLocation(Scenario* the_setup, const char* name);
	static String CallSpecialist(Scenario* the_setup, const char* name);
	static String InterrogateCharacter(Scenario* the_setup, const char* name);
	static String InvestigateEvidence(Scenario* the_setup, const char* name);
	static String InvestigateItem(Scenario* the_setup, const char* name);
	
public:
	Scenario(Game* the_game);
	~Scenario();

	void Startup();
	void Shutdown();

	void LoadInScenario();

private:
	// setup Scenario
	void LoadInScenarioSettings();
	void LoadInScenarioLocations();
	void LoadInScenarioContacts();
	void LoadInScenarioCharacters();
	void LoadInScenarioEvidence();
	void LoadInScenarioItems();

	
	void ManuallySetLocations();
	void ManuallySetContacts();
	void ManuallySetCharacters();
	void ManuallySetEvidence();
	void ManuallySetItems();

	
	void ReadLocationsXml();
	void ReadContactsXml();
	void ReadCharactersXml();
	void ReadEvidenceXml();
	void ReadItemsXml();

	
	// Database manipulation
	void SetupLocationLookupTable();
	void SetupContactLookupTable();
	void SetupCharacterLookupTable();
	void SetupEvidenceLookupTable();
	void SetupItemLookupTable();

	
	void AddToLocationLookupTable(const String& key_loc_name, int value_idx);
	void AddToContactLookupTable(const String& key_loc_name, int value_idx);
	void AddToCharacterLookupTable(const String& key_loc_name, int value_idx);
	void AddToEvidenceLookupTable(const String& key_loc_name, int value_idx);
	void AddToItemLookupTable(const String& key_loc_name, int value_idx);
	
	bool IsLocationInLookupTable(LookupItr& out, const String& name);
	bool IsContactInLookupTable(LookupItr& out, const String& name);
	bool IsCharacterInLookupTable(LookupItr& out, const String& name);
	bool IsEvidenceInLookupTable(LookupItr& out, const String& name);
	bool IsItemInLookupTable(LookupItr& out, const String& name);
	
	
private:
	// Owner
	Game*			m_theGame = nullptr;
	
	// Game state data
	Location*		m_currentLocation = nullptr;
	int				m_currentHourMilitary = 0;
	int				m_currentMinute = 0;

	
	// Physical representation of Entities in the game
	LocationList	m_locations;
	ContactList		m_contacts;
	CharacterList	m_characters;
	EvidenceList	m_evidence;
	ItemList		m_items;

	
	// To quickly lookup where an item is in it's respective list
	LookupTable		m_locationLookup;
	LookupTable		m_contactLookup;
	LookupTable		m_characterLookup;
	LookupTable		m_evidenceLookup;
	LookupTable		m_itemLookup;

	
	// Random lines to say when the player writes an unknown name
	StringList		m_unknownLocationLine;
	StringList		m_unknownContactLine;
	StringList		m_unknownCharacterLine;
	StringList		m_unknownEvidenceLine;
	StringList		m_unknownItemLine;
};
