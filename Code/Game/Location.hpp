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
	String m_locationState = "*";

	// either a character or item
	String m_characterName = "*";
	String m_characterState = "*";

	String m_line = "default dialogue for any card";
};


class Location: public Card
{
public:
	Location();
	explicit Location(Scenario* the_setup);
	explicit Location(Scenario* the_setup, const String& name, const StringList& list_of_nicknames,
		const String& desc);
	explicit Location(Scenario* the_setup, const XmlElement* element);
	~Location();

	// ACCESSORS
	bool		IsCharacterInLocation(const Character* character) const;
	String		GetLocationDescription() const;
	String		IntroduceCharacter(const Character* character) const;
	
	// MUTATORS
	void AddCharacterToLocation(const Character* character);
	void RemoveCharacterFromLocation(const Character* character);
	void SetState(const String& starting_state);
	
	void ImportLocationStatesFromXml(const XmlElement* element);
	void ImportLocationPresentingEntityFromXml(const XmlElement* element);
	
private:
	std::vector<const Character*> m_charsInLoc;

	LocationState			m_currentState;
	LocStateList			m_states;

	LocCharacterIntro		m_presentingDialogue;

	const float LOC_HEIGHT = 20.0f;
	const float LOC_ASPECT_RATIO = 2.2572855953372189841798501248959f;
};