#pragma once
#include "Game/GameCommon.hpp"

class Scenario;

class Action
{
public:
	explicit Action(Scenario* the_setup);		// needed for dialogue
	explicit Action(Trigger* event_trigger);	// need for events
	virtual ~Action();

	virtual void Execute();
	virtual String GetAsString();

protected:
	Scenario*	m_theScenario = nullptr;

	//ActionDisplayText
	String m_message;

	//ActionChangeCardState
	String m_cardName = "";
	CardType m_cardType = UNKNOWN_CARD_TYPE;
	String m_fromStateName = "";
	String m_toStateName = "";

	//ActionIncidentToggle
	String m_incidentName = "";
	bool m_set = false;
};


//-----------------------------------------------------
class ActionDisplayText : public Action
{
public:
	explicit ActionDisplayText(Scenario* event_trigger, const XmlElement* element);
	explicit ActionDisplayText(Trigger* event_trigger, const XmlElement* element);
	virtual ~ActionDisplayText();

	virtual void Execute() override;
	virtual String GetAsString() override;
};


//-----------------------------------------------------
class ActionChangeCardState : public Action
{
public:
	explicit ActionChangeCardState(Scenario* the_setup, const XmlElement* element);
	explicit ActionChangeCardState(Trigger* event_trigger, const XmlElement* element);
	virtual ~ActionChangeCardState();

	virtual void Execute() override;
	virtual String GetAsString() override;

};


//-----------------------------------------------------
class ActionIncidentToggle : public Action
{
public:
	explicit ActionIncidentToggle(Scenario* event_trigger, const XmlElement* element);
	explicit ActionIncidentToggle(Trigger* event_trigger, const XmlElement* element);
	virtual ~ActionIncidentToggle();

	virtual void Execute() override;
	virtual String GetAsString() override;

};