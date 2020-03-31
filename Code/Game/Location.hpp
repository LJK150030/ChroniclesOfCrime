#pragma once
#include "Game/Card.hpp"

struct LocationState
{
public:
	String	m_name = "";
	bool	m_canMoveHere = false;
	bool	m_addGameTime = false;
	String	m_description = "";
};


struct CharacterIntro
{
public:
	CharacterIntro();
	~CharacterIntro();

public:
	String m_locationState = "*";

	// either a character or item
	String m_characterName = "*";
	String m_characterState = "*";

	String m_line = "default dialogue for any card";
	ActionList m_actions = ActionList();
};


struct ItemIntro
{
public:
	String m_locationState = "*";

	// either a character or item
	String m_itemName = "*";
	String m_itemState = "*";

	String m_line = "default dialogue for any card";
	ActionList m_actions = ActionList();
};


class Location: public Card
{
public:
	Location();
	explicit Location(Scenario* the_setup);
	explicit Location(Scenario* the_setup, const String& name, const StringList& list_of_nicknames, const String& desc);
	explicit Location(Scenario* the_setup, const XmlElement* element);
	~Location();

	// ACCESSORS
	bool					IsCharacterInLocation(const Character* character) const;
	bool					GetLocationDescription(String& out) const;
	bool					IntroduceCharacter(String& out, const Character* character) const;
	bool					IntroduceItem(String& out, const Item* item) const;
	const LocationState&	GetLocationState() const;
	
	// MUTATORS
	void AddCharacterToLocation(const Character* character);
	void RemoveCharacterFromLocation(const Character* character);
	void SetState(const String& starting_state);
	
	void ImportLocationStatesFromXml(const XmlElement* element);
	void ImportLocationPresentingCharacterFromXml(const XmlElement* element);
	void ImportLocationPresentingItemFromXml(const XmlElement* element);
	
private:
	std::vector<const Character*> m_charsInLoc;

	LocationState			m_currentState;
	LocStateList			m_states;

	LocCharacterIntro		m_presentingCharacterDialogue;
	LocItemIntro			m_presentingItemDialogue;

	const float LOC_CARD_HEIGHT = 30.0f;
	const float LOC_CARD_ASPECT_RATIO = 2.2572855953372189841798501248959f;
};