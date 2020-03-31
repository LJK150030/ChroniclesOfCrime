#pragma once
#include "Game/GameCommon.hpp"
#include "Game/Trigger.hpp"

class Scenario;

class Incident
{
public:
	explicit Incident(Scenario* the_setup, const XmlElement* element);
	~Incident();

	//mutators
	void		SetActive(bool enable);
	bool		TestTriggers(); // for loop triggers, return true if at least one trigger activates, which will also perform accompany action(s).

	//accessors
	bool		IsIncidentEnabled() const; // return isEnabled
	Scenario*	GetOwner() const;


private:
	Scenario*				m_theScenario = nullptr;
	
	String					m_name = "";
	IncidentType			m_type = INCIDENT_UNKNOWN;
	bool					m_isEnabled = false;
	TriggerList				m_triggers;
};
