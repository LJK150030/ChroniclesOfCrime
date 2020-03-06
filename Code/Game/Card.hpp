#pragma once
#include "Game/GameCommon.hpp"

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
	CardType	GetCardType() const;
	String		GetName() const;
	StringList	GetListOfNicknames() const;
	String		GetDescription() const;
	
	// MUTATORS
	void SetDiscovery(bool discovered);

protected:
	//void ImportStatesFromXml(const XmlElement* element);
	//void ImportDialogueFromXml(const XmlElement* element);

	//void SetState(const String& starting_state);
	
protected:
	// owner
	Scenario*	m_theScenario = nullptr;
	
	// common variables
	CardType		m_type = UNKNOWN_CARD_TYPE;
	bool			m_found = false;
	String			m_description = "";
	
	String			m_name = "";
	StringList		m_nickNames;
};
