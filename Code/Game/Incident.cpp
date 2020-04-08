//Incident.cpp
#include "Game/Incident.hpp"
#include "Game/Scenario.hpp"

#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"


Incident::Incident(Scenario* the_setup, const XmlElement* element) : m_theScenario(the_setup)
{
	m_triggers = TriggerList();

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
		else if (attribute_name == "type")
		{
			String type_string = StringToLower(attribute->Value());

			if (type_string == "oneshot")
			{
				m_type = INCIDENT_OCCUR_ONCE;
			}
			else if (type_string == "multiple")
			{
				m_type = INCIDENT_OCCUR_MULTIPLE;
			}
		}
		else if (attribute_name == "isenabled")
		{
			m_isEnabled = attribute->BoolValue();
		}
		else
		{
			ERROR_RECOVERABLE(Stringf("Unknown attribute in Incident xml file, '%s', skipping attribute", attribute->Name()))
		}
	}

	if (m_isEnabled && m_theScenario != nullptr)
	{
		m_timeAtActive = m_theScenario->GetCurrentTime();
	}

	//Get all of the Triggers
	// Get all the children elements
	for (const XmlElement* child_element = element->FirstChildElement();
		child_element;
		child_element = child_element->NextSiblingElement()
		)
	{
		String element_name = StringToLower(child_element->Name());

		if (element_name == "trigger")
		{
			m_triggers.push_back(new Trigger(this, child_element));
		}
		else
		{
			ERROR_RECOVERABLE(Stringf("Unknown Element in Incident xml file, '%s', skipping element", child_element->Name()))
		}
	}
}


Incident::~Incident()
{
	uint num_triggers = m_triggers.size();
	for (uint trigger_idx = 0; trigger_idx < num_triggers; ++trigger_idx)
	{
		delete m_triggers[trigger_idx];
		m_triggers[trigger_idx] = nullptr;
	}
}


void Incident::SetActive(const bool enable)
{
	m_isEnabled = enable;
}


bool Incident::TestTriggers()
{
	if (!m_isEnabled)
	{
		return false;
	}

	const uint num_triggers = static_cast<uint>(m_triggers.size());
	for (uint trigger_idx = 0; trigger_idx > num_triggers; ++trigger_idx)
	{
		const bool triggered = m_triggers[trigger_idx]->Execute();

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

String Incident::GetName() const
{
	return m_name;
}

IncidentType Incident::GetType() const
{
	return m_type;
}

const TriggerList* Incident::GetTriggerList() const
{
	return &m_triggers;
}


