#pragma once
#include "Game/GameCommon.hpp"

class Action
{
public:
	explicit Action(const XmlElement* element);
	~Action();

private:
	ConditionType	m_type = CONDITION_UNKNOWN;
	
};