#pragma once
#include "Game/GameCommon.hpp"

#include "Engine/Math/Matrix44.hpp"
#include "Engine/Core/EventSystem.hpp"


class Game;
class Material;
class Shader;
class GPUMesh;
class DialogueSystem;

// Scenario interaction functions
static bool TravelToLocation(EventArgs& args);
static bool InterrogateCharacter(EventArgs& args);
static bool InvestigateItem(EventArgs& args);

//Don't like these here, but reducing too many includes
static bool ClearCommandDs(EventArgs& args);
static bool HelpCommandDs(EventArgs& args);

class Scenario
{
	friend class DialogueSystem;
	
public:
	Scenario(Game* the_game);
	~Scenario();

	void Startup();
	void Shutdown();

	void LoadInScenarioManually();
	void LoadInScenarioFile(const char* folder_dir);

	bool IsLocationInLookupTable(LookupItr& out, const String& name);
	bool IsCharacterInLookupTable(LookupItr& out, const String& name);
	bool IsItemInLookupTable(LookupItr& out, const String& name);

	Location&	GetLocationFromList( int idx );
	Character&	GetCharacterFromList(int idx);
	Item&		GetItemFromList(int idx);

	String& GetUnknownLocation();
	String& GetUnknownCharacter();
	String& GetUnknownItem();
	
private:
	// setup Scenario
	void ManuallySetLocations();
	void ManuallySetCharacters();
	void ManuallySetItems();
	void ManuallySetScenarioSettings();

	
	void ReadLocationsXml(const String& file_path);
	void ReadCharactersXml(const String& file_path);
	void ReadItemsXml(const String& file_path);
	void ReadSettingsXml(const String& file_path);
	
	// Database manipulation
	void SetupLocationLookupTable();
	void SetupCharacterLookupTable();
	void SetupItemLookupTable();

	
	void AddToLocationLookupTable(const String& key_loc_name, int value_idx);
	void AddToCharacterLookupTable(const String& key_loc_name, int value_idx);
	void AddToItemLookupTable(const String& key_loc_name, int value_idx);
	

	void OpenXmlFile(tinyxml2::XMLDocument* out, const String& file_path);
	
private:
	// Owner
	Game*			m_theGame = nullptr;
	
	// Game state data
	Location*		m_currentLocation = nullptr;
	int				m_currentHourMilitary = 0;
	int				m_currentMinute = 0;

	
	// Physical representation of Entities in the game
	LocationList	m_locations;
	CharacterList	m_characters;
	ItemList		m_items;

	
	// To quickly lookup where an item is in it's respective list
	LookupTable		m_locationLookup;
	LookupTable		m_characterLookup;
	LookupTable		m_itemLookup;

	
	// Random lines to say when the player writes an unknown name
	StringList		m_unknownLocationLine;
	StringList		m_unknownCharacterLine;
	StringList		m_unknownItemLine;
};
