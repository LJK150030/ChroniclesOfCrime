#pragma once
#include "Game/GameCommon.hpp"
#include "Game/GameNames.hpp"

#include <string>
#include <vector>

class Scenario;

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
	// owner
	Scenario*	m_theScenario = nullptr;

	// common variables
	CardType	m_type = UNKNOWN_CARD_TYPE;
	bool		m_found = false;
	String		m_name = "";
	StringList	m_nickNames;
	String		m_description = "";
};
