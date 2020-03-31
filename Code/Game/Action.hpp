#pragma once
#include "Game/GameCommon.hpp"

class Scenario;

class Action
{
public:
	explicit Action(Scenario* the_setup);
	virtual ~Action();

	virtual void Execute();

protected:
	Scenario*	m_theScenario = nullptr;

};


//-----------------------------------------------------
class DisplayText : public Action
{
public:
	explicit DisplayText(Scenario* the_setup, const String& message);
	virtual ~DisplayText();

	virtual void Execute() override;

private:
	String m_message;
};


//-----------------------------------------------------
class ChangeCardState : public Action
{
public:
	explicit ChangeCardState(Scenario* the_setup, CardType type, const String& card_name, const String& from_state_name, const String& to_state_name);
	virtual ~ChangeCardState();

	virtual void Execute() override;

private:
	CardType m_type = UNKNOWN_CARD_TYPE;
	String m_cardName = "";
	String m_fromStateName = "";
	String m_toStateName = "";
};