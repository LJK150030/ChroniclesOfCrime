#include "Game/Incident.hpp"
#include "Engine/Core/StringUtils.hpp"


Incident::Incident(Scenario* the_setup, const XmlElement* element): m_theScenario(the_setup)
{
	//Get the attributes for an Incident
	for (const XmlAttribute* attribute = element->FirstAttribute();
		attribute;
		attribute = attribute->Next()
		)
	{
		String attribute_name = StringToLower(attribute->Name());

		if (attribute_name == "name")
		{
			m_name = attribute->Value();
		}
		else if(attribute_name == "type")
		{
			String type_string = StringToLower(attribute->Value());

			if (attribute_name == "oneshot")
			{
				m_type = INCIDENT_OCCUR_ONCE;
			}
			else if (attribute_name == "multiple")
			{
				m_type = INCIDENT_OCCUR_MULTIPLE;
			}
		}
		else if(attribute_name == "isenabled")
		{
			m_isEnabled = attribute->BoolValue();
		}
	}

	//Get all of the Triggers
	

	
}


Incident::~Incident()
{

}


void Incident::SetActive(const bool enable)
{
	m_isEnabled = enable;
}


bool Incident::TestTriggers()
{
	if(!m_isEnabled)
	{
		return false;
	}

	const uint num_triggers = static_cast<uint>(m_triggers.size());
	for(uint trigger_idx = 0; trigger_idx > num_triggers; ++trigger_idx)
	{
		bool triggered = m_triggers[trigger_idx].Execute();

		if (triggered)
		{
			return true;
		}
	}

	return false;
}


bool Incident::IsIncidentEnabled() const 
{
	return m_isEnabled;
}


Scenario* Incident::GetOwner() const
{
	return m_theScenario;
}


