#include "Game/Trigger.hpp"
#include "Game/Condition.hpp"
#include "Game/Action.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"


Trigger::Trigger(Incident* scenario_event, const XmlElement* element) : m_scenarioEvent(scenario_event)
{
	m_conditions = ConditionList();
	m_actions = ActionList();


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
		else
		{
			ERROR_RECOVERABLE(Stringf("Unknown attribute in Incident xml file, '%s', skipping attribute", attribute->Name()))
		}
	}

	for (const XmlElement* child_element = element->FirstChildElement();
		child_element;
		child_element = child_element->NextSiblingElement()
		)
	{
		String element_name = StringToLower(child_element->Name());

		if (element_name == "conditions")
		{
			ImportConditionsFromXml(child_element);
		}
		else if (element_name == "actions")
		{
			ImportActionsFromXml(child_element);
		}
		else
		{
			ERROR_RECOVERABLE(Stringf("Unknown Element in Incident xml file, '%s', skipping element", child_element->Name()))
		}
	}
}


Trigger::~Trigger()
{
	uint num_actions = m_actions.size();
	for (uint act_idx = 0; act_idx < num_actions; ++act_idx)
	{
		delete m_actions[act_idx];
		m_actions[act_idx] = nullptr;
	}

	uint num_conditions = m_conditions.size();
	for (uint cond_idx = 0; cond_idx < num_conditions; ++cond_idx)
	{
		delete m_conditions[cond_idx];
		m_conditions[cond_idx] = nullptr;
	}
}


bool Trigger::Execute()
{
	
	// the set of conditions in an incident is using logical 'and'
	const uint num_conditions = static_cast<uint>(m_conditions.size());
	for(uint con_idx = 0; con_idx < num_conditions; ++con_idx)
	{
		const bool result = m_conditions[con_idx]->Test();

		if(!result)
		{
			return false;
		}
	}

	const uint num_actions = static_cast<uint>(m_actions.size());
	for (uint act_idx = 0; act_idx < num_actions; ++act_idx)
	{
		m_actions[act_idx]->Execute();
	}

	return true;
}


Incident* Trigger::GetOwner() const
{
	return m_scenarioEvent;
}


String Trigger::GetName() const
{
	return m_name;
}


const ConditionList* Trigger::GetConditionList() const
{
	return &m_conditions;
}


const ActionList* Trigger::GetActionList() const
{
	return &m_actions;
}


void Trigger::ImportConditionsFromXml(const XmlElement* element)
{
	for (const XmlElement* child_element = element->FirstChildElement();
		child_element;
		child_element = child_element->NextSiblingElement()
		)
	{
		String element_name = StringToLower(child_element->Name());

		if (element_name == "objectstatecheck")
		{
			m_conditions.push_back(new ConditionStateCheck(this, child_element));

		}
		else if (element_name == "timepassed")
		{
			m_conditions.push_back(new ConditionTimePassed(this, child_element));

		}
		else if (element_name == "locationcheck")
		{
			m_conditions.push_back(new ConditionLocationCheck(this, child_element));

		}
		else if (element_name == "contextcheck")
		{
			m_conditions.push_back(new ConditionContextCheck(this, child_element));

		}
		else
		{
			ERROR_RECOVERABLE(Stringf("Unknown Condition, '%s', in conditions for '%s' Trigger xml", element_name.c_str(), m_name.c_str()));
		}
	}
}


void Trigger::ImportActionsFromXml(const XmlElement* element)
{
	for (const XmlElement* child_element = element->FirstChildElement();
		child_element;
		child_element = child_element->NextSiblingElement()
		)
	{
		String element_name = StringToLower(child_element->Name());

		if (element_name == "displaytext")
		{
			m_actions.push_back(new ActionDisplayText(this, child_element));

		}
		else if (element_name == "activateincident")
		{
			m_actions.push_back(new ActionIncidentToggle(this, child_element));

		}
		else if (element_name == "setcardstate")
		{
			m_actions.push_back(new ActionChangeCardState(this, child_element));

		}
		else
		{
			ERROR_RECOVERABLE(Stringf("Unknown Action, '%s', in actions for '%s' Trigger xml", element_name.c_str(), m_name.c_str()));
		}
	}
}

