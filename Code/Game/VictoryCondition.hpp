#pragma once
#include "Game/GameCommon.hpp"

class Scenario;

class VictoryCondition
{
public:
	explicit VictoryCondition(Scenario* the_scenario, const XmlElement* element);
	~VictoryCondition();

	// ACCESSORS
	bool HasConditionsBeenMet() const;

	// MUTATORS
	void TestCondition();

	
private:
	CardType	m_cardType = UNKNOWN_CARD_TYPE;
	String		m_cardName;
	String		m_requiredState;
	bool		m_hasBeenMet = false;

	Scenario*	m_scenario = nullptr;
};