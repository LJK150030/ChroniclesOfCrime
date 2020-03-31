#pragma once
#include "Game/GameCommon.hpp"

class Scenario;

// Following the Command Pattern to
class Condition
{
public:
	explicit Condition(Trigger* event_trigger);
	virtual ~Condition();

	virtual bool Test();

private:
	Trigger*	m_trigger = nullptr;
};


//-------------------------------------------------------------------
class ConditionTimePassed : public Condition
{
public:
	ConditionTimePassed(Scenario* the_setup, const XmlElement* element);
	virtual ~ConditionTimePassed();

	virtual bool Test() override;

private:
	
};