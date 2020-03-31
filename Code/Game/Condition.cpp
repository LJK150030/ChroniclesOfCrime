#include "Game/Condition.hpp"


Condition::Condition(Trigger* event_trigger, const XmlElement* element): m_trigger(event_trigger)
{
	
};


Condition::~Condition()
{
	
}


bool Condition::Test()
{
	return false;
}

//-------------------------------------------------------------------
