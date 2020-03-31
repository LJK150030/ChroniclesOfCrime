#pragma once
#include "Game/GameCommon.hpp"

class Incident;

class Trigger
{
public:
	explicit Trigger(Incident* scenario_event, const XmlElement* element);
	~Trigger();

	bool	Execute();

	Incident*	GetOwner() const;

private:
	Incident*	m_scenarioEvent = nullptr;
		
	String			m_name;
	//ConditionList	m_conditions;
	//ActionList		m_actions;
};
