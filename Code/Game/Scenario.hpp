#pragma once
#include "Game/GameCommon.hpp"

#include "Engine/Math/Matrix44.hpp"
#include "Engine/Core/EventSystem.hpp"


class Game;
class Material;
class Shader;
class GPUMesh;
class DialogueSystem;

//Debugging
static bool DumpLocations(EventArgs& args);
static bool DumpCharacter(EventArgs& args);
static bool DumpItems(EventArgs& args);

// Scenario interaction functions
static bool TravelToLocation(EventArgs& args);
static bool AskLocationForCharacter(EventArgs& args);
static bool AskLocationForItem(EventArgs& args);
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

	void Update(const double delta_seconds);
	void Render() const;

	void LoadInScenarioManually();
	void LoadInScenarioFile(const char* folder_dir);

	bool IsLocationInLookupTable(LookupItr& out, const String& name);
	bool IsCharacterInLookupTable(LookupItr& out, const String& name);
	bool IsItemInLookupTable(LookupItr& out, const String& name);

	Location*	GetLocationFromList( int idx );
	Character*	GetCharacterFromList(int idx);
	Item*		GetItemFromList(int idx);

	String& GetUnknownLocation();
	String& GetUnknownCharacter();
	String& GetUnknownItem();

	Location*	GetCurrentLocation();
	Card*		GetCurrentInterest();

	int AddMinutesToTime();
	int AddHoursToTime();

	//Helpper
	LocationList GetLocationList();
	CharacterList GetCharacterList();
	ItemList GetItemList();
	
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
	int				m_currentHourMilitary = 0;
	int				m_currentMinute = 0;
	
	Location*		m_currentLocation = nullptr;
	Card*			m_currentInterest = nullptr;
	Card*			m_currentSubject = nullptr;
	

	
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

	bool	m_show = true;
	bool	m_imguiError = false;
	float	m_gameResolution[2] = { 720.0f, 1080.0f };
	float	m_dialogWindowSize[2] = { 800.0f, 200.0f };
};
