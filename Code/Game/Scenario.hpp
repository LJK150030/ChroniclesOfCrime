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
static bool DumpIncident(EventArgs& args);

// Scenario interaction functions
static bool TravelToLocation(EventArgs& args);
static bool AskLocationForCharacter(EventArgs& args);
static bool AskLocationForItem(EventArgs& args);
static bool InterrogateCharacter(EventArgs& args);
static bool SayGoodbyToCharacter(EventArgs& args);
//static bool InvestigateItem(EventArgs& args);
static bool InvestigateRoom(EventArgs& args);
static bool LeaveRoom(EventArgs& args);
static bool SolveScenario(EventArgs& args);

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
	bool IsIncidentInLookupTable(LookupItr& out, const String& name);
	bool IsCardInLookupTable(LookupItr& out_itr, CardType& out_type, const String& name);

	Location*	GetLocationFromList(int idx);
	Character*	GetCharacterFromList(int idx);
	Item*		GetItemFromList(int idx);
	Incident*	GetIncidentFromList(int idx);

	String& GetUnknownLocation();
	String& GetUnknownCharacter();
	String& GetUnknownItem();
	String& GetSolution();
	String& GetCongratulations();
	String& GetContinueInvestigation();

	Location*	GetCurrentLocation();
	Card*		GetCurrentInterest();

	void	SetLocation(Location* loc);
	void	SetInterest(Card* card);
	void	ScenarioSolved();

	void		AddGameTime(const uint mins, const uint hours);
	GameTime	GetCurrentTime() const;
	uint		GetLocChangeTime() const;
	uint		GetExamineItemChangeTime() const;
	uint		GetInterrogateChangeTime() const;
	uint		GetWastingTime() const;
	void		TestIncidents();
	void		TestVictoryConditions();
	bool		AreAllVictoryConditionsMet() const;
	bool		IsScenarioSolved() const;


	//Helpper
	const LocationList* GetLocationList() const;
	const CharacterList* GetCharacterList() const;
	const ItemList* GetItemList() const;
	const IncidentList* GetIncidentList() const;

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
	void ReadIncidentsXml(const String& file_path);
	void ReadVictoryConditionsXml(const String& file_path);

	void ReadScenarioSettingsAttributes(const XmlElement* element);
	void ReadScenarioTimeCostForActions(const XmlElement* element);
	void ReadScenarioStarsScoreSettings(const XmlElement* element);
	void ReadScenarioGameTimeScoreBonusAndPenalty(const XmlElement* element);
	void ReadScenarioDefaultEnding(const XmlElement* element);
	

	// Database manipulation
	void SetupLocationLookupTable();
	void SetupCharacterLookupTable();
	void SetupItemLookupTable();
	void SetupIncidentLookupTable();


	void AddToLocationLookupTable(const String& key_loc_name, int value_idx);
	void AddToCharacterLookupTable(const String& key_loc_name, int value_idx);
	void AddToItemLookupTable(const String& key_loc_name, int value_idx);
	void AddToIncidentLookupTable(const String& key_loc_name, int value_idx);


	void OpenXmlFile(tinyxml2::XMLDocument* out, const String& file_path);

private:
	// Owner
	Game*	m_theGame = nullptr;
	String	m_solution;
	String	m_congratulations;
	String	m_continueInvestigation;
	bool	m_solved = false;
	
	// Game state data
	String		m_name = "";
	GameTime	m_gameTime;
	Location*	m_currentLocation = nullptr;
	Card*		m_currentInterest = nullptr;	// the card we are talking to, interacting with (aka context)
	Card*		m_currentSubject = nullptr;		// the card we are asking about, relevant to
	
	// Scenario constants
	uint		m_costToMoveToLocation = 20;
	uint		m_costToInvestigateLocation = 5;
	uint		m_costToExamineItem = 5;
	uint		m_costToInterrogateCharacter = 5;
	uint		m_costForUnknownCommand = 5;


	// Physical representation of Entities in the game
	LocationList		m_locations;
	CharacterList		m_characters;
	ItemList			m_items;
	IncidentList		m_incidents;
	VictoryConditions	m_victoryConditions;


	// To quickly lookup where an item is in it's respective list
	LookupTable		m_locationLookup;
	LookupTable		m_characterLookup;
	LookupTable		m_itemLookup;
	LookupTable		m_incidentLookup;


	// Random lines to say when the player writes an unknown name
	StringList		m_unknownLocationLine;
	StringList		m_unknownCharacterLine;
	StringList		m_unknownItemLine;

	// ImGUI settings
	bool	m_show = true;
	bool	m_imguiError = false;
	float	m_gameResolution[2] = { 720.0f, 1080.0f };
	float	m_dialogWindowSize[2] = { 800.0f, 200.0f };
};
