#pragma once
#include "Game/GameCommon.hpp"

class Scenario;

enum TimeRelativeTo
{
	ABSOLUTE_GAME_TIME,
	INCIDENT_ENABLED
};

enum QualCondition
{
	QUAL_IS,
	QUAL_IS_NOT
};

enum QuantCondition
{
	QUANT_IS,
	QUANT_IS_NOT,
	QUANT_LESS_THAN,
	QUANT_LESS_THAN_EQUAL_TO,
	QUANT_GREATER_THAN,
	QUANT_GREATER_THAN_EQUAL_TO,
};


class Condition
{
public:
	explicit Condition(Trigger* event_trigger);
	virtual ~Condition();

	virtual bool Test();
	virtual String GetAsString() const;

protected:
	Trigger*	m_trigger = nullptr;

	//ConditionTimePassed data
	TimeRelativeTo	m_since = ABSOLUTE_GAME_TIME;
	GameTime		m_timePassed;

	//ConditionLocationCheck data
	String	m_atLocationName = "unkown";
	bool	m_playerPresence = true;

	//ConditionStateCheck data
	String			m_cardName = "unkown";
	CardType		m_cardType = UNKNOWN_CARD_TYPE;
	QualCondition	m_qualCondition = QUAL_IS;
	String			m_cardStateName = "unkown";

	//ConditionContextCheck data
	//String		m_cardName = "unkown";
	//CardType		m_cardType = UNKNOWN_CARD_TYPE;
	//QualCondition	m_qualCondition		= QUAL_IS;
};


//-------------------------------------------------------------------
class ConditionTimePassed final : public Condition
{
public:
	ConditionTimePassed(Trigger* event_trigger, const XmlElement* element);
	virtual ~ConditionTimePassed();

	virtual bool Test() override;
	virtual String GetAsString() const override;
};


//-------------------------------------------------------------------
class ConditionLocationCheck : public Condition
{
public:
	ConditionLocationCheck(Trigger* event_trigger, const XmlElement* element);
	virtual ~ConditionLocationCheck();

	virtual bool Test() override;
	virtual String GetAsString() const override;

};


//-------------------------------------------------------------------
class ConditionStateCheck : public Condition
{
public:
	ConditionStateCheck(Trigger* event_trigger, const XmlElement* element);
	virtual ~ConditionStateCheck();

	virtual bool Test() override;
	virtual String GetAsString() const override;

};


//-------------------------------------------------------------------
class ConditionContextCheck : public Condition
{
public:
	ConditionContextCheck(Trigger* event_trigger, const XmlElement* element);
	virtual ~ConditionContextCheck();

	virtual bool Test() override;
	virtual String GetAsString() const override;

};

