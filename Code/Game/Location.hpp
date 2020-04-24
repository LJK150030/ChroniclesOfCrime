#pragma once
#include "Game/Card.hpp"

struct LocationState
{
public:
	String	m_name = "";
	bool	m_canMoveHere = false;
	bool	m_addGameTime = false;
	String	m_description = "";

	LocationSpecialAction	m_specialAction = LSA_NONE;
	Material*				m_roomMaterial = nullptr;
};


struct IntroFromLocation
{
public:
	IntroFromLocation();
	~IntroFromLocation();

public:
	String m_locationState = "*";

	// either a character or item
	CardType m_cardType = UNKNOWN_CARD_TYPE;
	String m_cardName = "*";
	String m_cardState = "*";

	String m_line = "default dialogue for any card";
	ActionList m_actions = ActionList();
};


class Location : public Card
{
public:
	Location();
	explicit Location(Scenario* the_setup);
	explicit Location(Scenario* the_setup, const String& name, const StringList& list_of_nicknames, const String& desc);
	explicit Location(Scenario* the_setup, const XmlElement* element);
	~Location();

	void Render() const override;

	// ACCESSORS
	bool					IsCharacterInLocation(const Character* character) const;
	bool					GetLocationDescription(String& out) const;
	bool					IntroduceCharacter(String& out, const Character* character);
	bool					IntroduceItem(String& out, const Item* item);
	bool					CanInvestigateLocation() const;
	bool					IsPlayerInvestigatingRoom() const;
	bool					CanSolveCaseHere() const;
	const LocationState&	GetLocationState() const;
	String					GetAsString() const;

	// MUTATORS
	void AddCharacterToLocation(const Character* character);
	void RemoveCharacterFromLocation(const Character* character);
	void SetState(const String& starting_state);
	void SetInvestigation(bool set);

	void ImportLocationStatesFromXml(const XmlElement* element);
	void ImportLocationIntroductions(const XmlElement* element, CardType type);

private:
	std::vector<const Character*> m_charsInLoc;

	bool m_investigating = false;

	LocationState			m_currentState;
	LocStateList			m_states;

	Intros			m_presentingCharacterDialogue;
	Intros			m_presentingItemDialogue;

	const float LOC_CARD_HEIGHT = 30.0f;
	const float LOC_CARD_ASPECT_RATIO = 2.2572855953372189841798501248959f;

	Material* m_defaultRoomMaterial = nullptr; //can investigate will just check if this is not null
	GPUMesh* m_defaultRoomMesh = nullptr;
};