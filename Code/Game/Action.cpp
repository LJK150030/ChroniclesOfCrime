#include "Game/Action.hpp"
#include "Game/App.hpp"
#include "Game/Game.hpp"
#include "Game/Scenario.hpp"
#include "Game/DialogueSystem.hpp"
#include "Game/Location.hpp"
#include "Game/Character.hpp"
#include "Game/Item.hpp"
#include "Game/Incident.hpp"
#include "Game/Trigger.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"

Action::Action(Scenario* the_setup) : m_theScenario(the_setup)
{
}


Action::Action(Trigger* event_trigger)
{
	m_theScenario = event_trigger->GetOwner()->GetOwner();
}


Action::~Action()
{
}


void Action::Execute()
{
	ERROR_AND_DIE("Base class execute should never be called.");
}


String Action::GetAsString()
{
	return String("Base Action");
}


//-----------------------------------------------------
ActionDisplayText::ActionDisplayText(Scenario* event_trigger, const XmlElement* element) :
	Action(event_trigger)
{
	for (const XmlAttribute* attribute = element->FirstAttribute();
		attribute;
		attribute = attribute->Next()
		)
	{
		String attribute_name = StringToLower(attribute->Name());

		if (attribute_name == "type")
		{
			continue;
		}
		else if (attribute_name == "message")
		{
			m_message = StringToLower(attribute->Value());
		}
		else
		{
			ERROR_RECOVERABLE(Stringf("Unknown attribute, '%s', in element, '%s', from xml", attribute_name.c_str(), element->Name()));
		}
	}
}


ActionDisplayText::ActionDisplayText(Trigger* event_trigger, const XmlElement* element) :
	Action(event_trigger)
{
	for (const XmlAttribute* attribute = element->FirstAttribute();
		attribute;
		attribute = attribute->Next()
		)
	{
		String attribute_name = StringToLower(attribute->Name());

		if (attribute_name == "type")
		{
			continue;
		}
		else if (attribute_name == "message")
		{
			m_message = attribute->Value();
		}
		else
		{
			ERROR_RECOVERABLE(Stringf("Unknown attribute, '%s', in element, '%s', from xml", attribute_name.c_str(), element->Name()));
		}
	}
}


ActionDisplayText::~ActionDisplayText()
{
}


void ActionDisplayText::Execute()
{
	DialogueSystem* ds = g_theApp->GetTheGame()->GetDialogueSystem();
	ds->AddLog(LOG_LOCATION, m_message);
}


String ActionDisplayText::GetAsString()
{
	return Stringf("ActionDisplayText: '%s'", m_message.c_str());
}


//-----------------------------------------------------
ActionChangeCardState::ActionChangeCardState(Scenario* the_setup, const XmlElement* element) :
	Action(the_setup)
{
	for (const XmlAttribute* attribute = element->FirstAttribute();
		attribute;
		attribute = attribute->Next()
		)
	{
		String attribute_name = StringToLower(attribute->Name());

		if (attribute_name == "type")
		{
			String card_type_string = StringToLower(attribute->Value());

			if (card_type_string == "location")
			{
				m_cardType = CARD_LOCATION;
			}
			else if (card_type_string == "character")
			{
				m_cardType = CARD_CHARACTER;
			}
			else if (card_type_string == "item")
			{
				m_cardType = CARD_ITEM;
			}
			else
			{
				ERROR_RECOVERABLE(Stringf("Unknown attribute value, '%s', for attribute, '%s', in element, '%s', from xml", card_type_string.c_str(), attribute_name.c_str(), element->Name()));
			}
		}
		else if (attribute_name == "name")
		{
			m_cardName = StringToLower(attribute->Value());
		}
		else if (attribute_name == "fromstate")
		{
			m_fromStateName = StringToLower(attribute->Value());
		}
		else if (attribute_name == "tostate")
		{
			m_toStateName = StringToLower(attribute->Value());
		}
		else
		{
			ERROR_RECOVERABLE(Stringf("Unknown attribute, '%s', in element, '%s', from xml", attribute_name.c_str(), element->Name()));
		}
	}
}


ActionChangeCardState::ActionChangeCardState(Trigger* event_trigger, const XmlElement* element) :
	Action(event_trigger)
{
	for (const XmlAttribute* attribute = element->FirstAttribute();
		attribute;
		attribute = attribute->Next()
		)
	{
		String attribute_name = StringToLower(attribute->Name());

		if (attribute_name == "type")
		{
			String card_type_string = StringToLower(attribute->Value());

			if (card_type_string == "location")
			{
				m_cardType = CARD_LOCATION;
			}
			else if (card_type_string == "character")
			{
				m_cardType = CARD_CHARACTER;
			}
			else if (card_type_string == "item")
			{
				m_cardType = CARD_ITEM;
			}
			else
			{
				ERROR_RECOVERABLE(Stringf("Unknown attribute value, '%s', for attribute, '%s', in element, '%s', from xml", card_type_string.c_str(), attribute_name.c_str(), element->Name()));
			}
		}
		else if (attribute_name == "name")
		{
			m_cardName = StringToLower(attribute->Value());
		}
		else if (attribute_name == "fromstate")
		{
			m_fromStateName = StringToLower(attribute->Value());
		}
		else if (attribute_name == "tostate")
		{
			m_toStateName = StringToLower(attribute->Value());
		}
		else
		{
			ERROR_RECOVERABLE(Stringf("Unknown attribute, '%s', in element, '%s', from xml", attribute_name.c_str(), element->Name()));
		}
	}
}


ActionChangeCardState::~ActionChangeCardState()
{
}


void ActionChangeCardState::Execute()
{
	ASSERT_OR_DIE(m_cardType != UNKNOWN_CARD_TYPE, "Attempting to change the state of an unknown card type");

	String current_state = "";

	switch (m_cardType)
	{
	case CARD_LOCATION:
	{
		LookupItr loc_itr;
		bool in_scenario = m_theScenario->IsLocationInLookupTable(loc_itr, m_cardName);
		if (in_scenario)
		{
			Location* loc_card = m_theScenario->GetLocationFromList(loc_itr->second);
			current_state = loc_card->GetLocationState().m_name;

			if (current_state == m_fromStateName)
			{
				loc_card->SetState(m_toStateName);
			}
		}
		break;
	}
	case CARD_CHARACTER:
	{
		LookupItr char_itr;
		bool in_scenario = m_theScenario->IsCharacterInLookupTable(char_itr, m_cardName);
		if (in_scenario)
		{
			Character* char_card = m_theScenario->GetCharacterFromList(char_itr->second);
			current_state = char_card->GetCharacterState().m_name;

			if (current_state == m_fromStateName)
			{
				char_card->SetState(m_toStateName);
			}
		}
		break;
	}
	case CARD_ITEM:
	{
		LookupItr item_itr;
		bool in_scenario = m_theScenario->IsItemInLookupTable(item_itr, m_cardName);
		if (in_scenario)
		{
			Item* item_card = m_theScenario->GetItemFromList(item_itr->second);
			current_state = item_card->GetItemState().m_name;

			if (current_state == m_fromStateName)
			{
				item_card->SetState(m_toStateName);
			}
		}
		break;
	}
	}
}


String ActionChangeCardState::GetAsString()
{
	String line;

	switch (m_cardType)
	{
	case CARD_LOCATION:
	{
		line = Stringf("Change the location %s", m_cardName.c_str());
		break;
	}
	case CARD_CHARACTER:
	{
		line = Stringf("Change the character %s", m_cardName.c_str());
		break;
	}
	case CARD_ITEM:
	{
		line = Stringf("Change the item %s", m_cardName.c_str());
		break;
	}
	}

	line += Stringf(" from state %s to state %s.", m_fromStateName.c_str(), m_toStateName.c_str());

	return Stringf("ActionChangeCardState: %s", line.c_str());
}


//-----------------------------------------------------
ActionIncidentToggle::ActionIncidentToggle(Scenario* event_trigger, const XmlElement* element) :
	Action(event_trigger)
{
	for (const XmlAttribute* attribute = element->FirstAttribute();
		attribute;
		attribute = attribute->Next()
		)
	{
		String attribute_name = StringToLower(attribute->Name());

		if (attribute_name == "incident")
		{
			m_incidentName = attribute->Value();
		}
		else if (attribute_name == "set")
		{
			String set_string = StringToLower(attribute->Value());

			if (set_string == "enable")
			{
				m_set = true;
			}
			else if (set_string == "disable")
			{
				m_set = false;
			}
			else
			{
				ERROR_RECOVERABLE(Stringf("Unknown attribute value, '%s', for attribute, '%s', in element, '%s', from xml", set_string.c_str(), attribute_name.c_str(), element->Name()));
			}
		}
		else
		{
			ERROR_RECOVERABLE(Stringf("Unknown attribute, '%s', in element, '%s', from xml", attribute_name.c_str(), element->Name()));
		}
	}
}

ActionIncidentToggle::ActionIncidentToggle(Trigger* event_trigger, const XmlElement* element) :
	Action(event_trigger)
{
	for (const XmlAttribute* attribute = element->FirstAttribute();
		attribute;
		attribute = attribute->Next()
		)
	{
		String attribute_name = StringToLower(attribute->Name());

		if (attribute_name == "incident")
		{
			m_incidentName = attribute->Value();
		}
		else if (attribute_name == "set")
		{
			String set_string = StringToLower(attribute->Value());

			if (set_string == "enable")
			{
				m_set = true;
			}
			else if (set_string == "disable")
			{
				m_set = false;
			}
			else
			{
				ERROR_RECOVERABLE(Stringf("Unknown attribute value, '%s', for attribute, '%s', in element, '%s', from xml", set_string.c_str(), attribute_name.c_str(), element->Name()));
			}
		}
		else
		{
			ERROR_RECOVERABLE(Stringf("Unknown attribute, '%s', in element, '%s', from xml", attribute_name.c_str(), element->Name()));
		}
	}
}


ActionIncidentToggle::~ActionIncidentToggle()
{
}


void ActionIncidentToggle::Execute()
{
	LookupItr inc_itr;
	const bool in_scenario = m_theScenario->IsIncidentInLookupTable(inc_itr, m_incidentName);
	if (in_scenario)
	{
		Incident* inc = m_theScenario->GetIncidentFromList(inc_itr->second);
		inc->SetActive(m_set);
	}
}


String ActionIncidentToggle::GetAsString()
{
	String line;

	if (m_set)
	{
		line = String("enable");
	}
	else
	{
		line = String("disable");
	}

	line += Stringf(" the %s event", m_incidentName.c_str());

	return Stringf("ActionIncidentToggle: %s", line.c_str());
}
