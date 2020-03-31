#include "Game/Trigger.hpp"

Trigger::Trigger(Incident* scenario_event, const XmlElement* element)
{
	
}

Trigger::~Trigger()
{
	
}

bool Trigger::Execute()
{
	return false;
}

Incident* Trigger::GetOwner() const
{
	return m_scenarioEvent;
}



