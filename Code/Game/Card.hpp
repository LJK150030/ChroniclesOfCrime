#pragma once
#include "Game/GameCommon.hpp"
#include "Game/GameNames.hpp"

#include <string>
#include <vector>

class Scenario;


struct State
{
public:
	String	m_name = "";
	bool	m_canMoveHere = false;
	bool	m_addGameTime = false;
};

struct Dialogue
{
public:
	String m_inState = "*";
	String m_line = "default dialogue for any card";
};


class Card
{
public:
	Card();
	explicit Card(Scenario* the_setup, CardType type);
	explicit Card(Scenario* the_setup, CardType type, const String& name, const StringList& list_of_nicknames,
		const String& desc);
	~Card();

public:
	// ACCESSORS
	bool		IsDiscovered() const;
	String&		GetName();
	StringList&	GetListOfNicknames();
	String&		GetDescription();
	
	// MUTATORS
	void SetDiscovery(bool discovered);

protected:
	void ImportStatesFromXml(const XmlElement* element);
	void ImportDialogueFromXml(const XmlElement* element);

	void SetState(const String& starting_state);
	
protected:
	// owner
	Scenario*	m_theScenario = nullptr;

	// common variables
	CardType		m_type = UNKNOWN_CARD_TYPE;
	bool			m_found = false;
	String			m_description = "";
	
	String			m_name = "";
	StringList		m_nickNames;

	State			m_currentState;
	StateList		m_states;
	
	DialogueList	m_dialogueLines;
};
