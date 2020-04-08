#pragma once
#include "Game/GameCommon.hpp"

class Incident;

class Trigger
{
public:
	explicit Trigger(Incident* scenario_event, const XmlElement* element);
	~Trigger();

	bool	Execute();

	Incident*				GetOwner() const;
	String					GetName() const;
	const ConditionList*	GetConditionList() const;
	const ActionList*		GetActionList() const;


private:
	void ImportConditionsFromXml(const XmlElement* element);
	void ImportActionsFromXml(const XmlElement* element);

private:
	Incident*	m_scenarioEvent = nullptr;

	String			m_name;
	ConditionList	m_conditions;
	ActionList		m_actions;
};
