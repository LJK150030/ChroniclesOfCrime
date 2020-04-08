#include "Game/Condition.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"


Condition::Condition(Trigger* event_trigger) : m_trigger(event_trigger)
{
};


Condition::~Condition()
{

}


bool Condition::Test()
{
	ERROR_AND_DIE("Have not setup the Trigger::Execute() function");
	return false;
}


String Condition::GetAsString() const
{
	return String("Base Condition");
}


//-------------------------------------------------------------------
ConditionTimePassed::ConditionTimePassed(Trigger* event_trigger, const XmlElement* element) :
	Condition(event_trigger)
{
	for (const XmlAttribute* attribute = element->FirstAttribute();
		attribute;
		attribute = attribute->Next()
		)
	{
		String attribute_name = StringToLower(attribute->Name());

		if (attribute_name == "since")
		{
			String since_string = StringToLower(attribute->Value());

			if (since_string == "eventenabled")
			{
				m_since = INCIDENT_ENABLED;
			}
			else if (since_string == "absolutegametime")
			{
				m_since = ABSOLUTE_GAME_TIME;
			}
			else
			{
				ERROR_RECOVERABLE(Stringf("Unknown attribute value, '%s', for attribute, '%s', in element, '%s', from xml", since_string.c_str(), attribute_name.c_str(), element->Name()));
			}
		}
		else if (attribute_name == "dayspassed")
		{
			m_timePassed.m_day = attribute->UnsignedValue();

		}
		else if (attribute_name == "hourspassed")
		{
			m_timePassed.m_hour = attribute->UnsignedValue();
		}
		else if (attribute_name == "minutespassed")
		{
			m_timePassed.m_min = attribute->UnsignedValue();
		}
		else
		{
			ERROR_RECOVERABLE(Stringf("Unknown attribute, '%s', in element, '%s', from xml", attribute_name.c_str(), element->Name()));
		}
	}
}


ConditionTimePassed::~ConditionTimePassed()
{
}

bool ConditionTimePassed::Test()
{
	ERROR_AND_DIE("Have not setup the Trigger::Execute() function");
	return false;
}

String ConditionTimePassed::GetAsString() const
{
	String since_string = "";

	switch (m_since)
	{
	case ABSOLUTE_GAME_TIME:
	{
		since_string = Stringf("if the time is %02d:%02d on day %d.",
			m_timePassed.m_hour, m_timePassed.m_min, m_timePassed.m_day);
		break;
	}
	case INCIDENT_ENABLED:
	{
		since_string = Stringf("if it is after %d day(s), %d hour(s), and %d min(s) since this event was made",
			m_timePassed.m_hour, m_timePassed.m_min, m_timePassed.m_day);
		break;
	}
	default:
	{
		break;
	}
	}

	return Stringf("ConditionTimePassed: %s", since_string.c_str());
}

//-------------------------------------------------------------------
ConditionLocationCheck::ConditionLocationCheck(Trigger* event_trigger, const XmlElement* element) :
	Condition(event_trigger)
{
	for (const XmlAttribute* attribute = element->FirstAttribute();
		attribute;
		attribute = attribute->Next()
		)
	{
		String attribute_name = StringToLower(attribute->Name());

		if (attribute_name == "location")
		{
			m_atLocationName = attribute->Value();
		}
		else if (attribute_name == "condition")
		{
			String condition_string = StringToLower(attribute->Value());

			if (condition_string == "inside")
			{
				m_playerPresence = true;
			}
			else if (condition_string == "outside")
			{
				m_playerPresence = false;
			}
			else
			{
				ERROR_RECOVERABLE(Stringf("Unknown attribute value, '%s', for attribute, '%s', in element, '%s', from xml", condition_string.c_str(), attribute_name.c_str(), element->Name()));
			}
		}
		else
		{
			ERROR_RECOVERABLE(Stringf("Unknown attribute, '%s', in element, '%s', from xml", attribute_name.c_str(), element->Name()));
		}
	}
}


ConditionLocationCheck::~ConditionLocationCheck()
{

}


bool ConditionLocationCheck::Test()
{
	ERROR_AND_DIE("Have not setup the Trigger::Execute() function");
	return false;
}


String ConditionLocationCheck::GetAsString() const
{
	String line;

	if (m_playerPresence)
	{
		line = Stringf("if the player is at %s", m_atLocationName.c_str());
	}
	else
	{
		line = Stringf("if the player is not at %s", m_atLocationName.c_str());
	}

	return Stringf("ConditionLocationCheck: %s", line.c_str());
}

//-------------------------------------------------------------------
ConditionStateCheck::ConditionStateCheck(Trigger* event_trigger, const XmlElement* element) :
	Condition(event_trigger)
{
	for (const XmlAttribute* attribute = element->FirstAttribute();
		attribute;
		attribute = attribute->Next()
		)
	{
		String attribute_name = StringToLower(attribute->Name());

		if (attribute_name == "object")
		{
			m_cardName = attribute->Value();
		}
		else if (attribute_name == "type")
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
		else if (attribute_name == "operation")
		{
			String condition_string = StringToLower(attribute->Value());

			if (condition_string == "is")
			{
				m_qualCondition = QUAL_IS;
			}
			else if (condition_string == "is not")
			{
				m_qualCondition = QUAL_IS_NOT;
			}
			else
			{
				ERROR_RECOVERABLE(Stringf("Unknown attribute value, '%s', for attribute, '%s', in element, '%s', from xml", condition_string.c_str(), attribute_name.c_str(), element->Name()));
			}
		}
		else if (attribute_name == "state")
		{
			m_cardStateName = attribute->Value();
		}
		else
		{
			ERROR_RECOVERABLE(Stringf("Unknown attribute, '%s', in element, '%s', from xml", attribute_name.c_str(), element->Name()));
		}
	}
}


ConditionStateCheck::~ConditionStateCheck()
{

}


bool ConditionStateCheck::Test()
{
	ERROR_AND_DIE("Have not setup the Trigger::Execute() function");
	return false;
}

String ConditionStateCheck::GetAsString() const
{
	String line;

	switch (m_cardType)
	{
	case CARD_LOCATION:
	{
		line = Stringf("if the location %s", m_cardName.c_str());
		switch (m_qualCondition)
		{
		case QUAL_IS:
		{
			line += " is";
			break;
		}
		case QUAL_IS_NOT:
		{
			line += " is not";
			break;
		}
		}
		break;
	}
	case CARD_CHARACTER:
	{
		line = Stringf("if the character %s", m_cardName.c_str());
		switch (m_qualCondition)
		{
		case QUAL_IS:
		{
			line += " is";
			break;
		}
		case QUAL_IS_NOT:
		{
			line += " is not";
			break;
		}
		}
		break;
	}
	case CARD_ITEM:
	{
		line = Stringf("if the item %s", m_cardName.c_str());
		switch (m_qualCondition)
		{
		case QUAL_IS:
		{
			line += " is";
			break;
		}
		case QUAL_IS_NOT:
		{
			line += " is not";
			break;
		}
		}
		break;
	}
	}

	line += Stringf(" in the state %s", m_cardStateName.c_str());

	return Stringf("ConditionStateCheck: %s", line.c_str());
}

//-------------------------------------------------------------------
ConditionContextCheck::ConditionContextCheck(Trigger* event_trigger, const XmlElement* element) :
	Condition(event_trigger)
{
	for (const XmlAttribute* attribute = element->FirstAttribute();
		attribute;
		attribute = attribute->Next()
		)
	{
		String attribute_name = StringToLower(attribute->Name());

		if (attribute_name == "object")
		{
			m_cardName = attribute->Value();
		}
		else if (attribute_name == "type")
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
		else if (attribute_name == "condition")
		{
			String condition_string = StringToLower(attribute->Value());

			if (condition_string == "is")
			{
				m_qualCondition = QUAL_IS;
			}
			else if (condition_string == "is not")
			{
				m_qualCondition = QUAL_IS_NOT;
			}
			else
			{
				ERROR_RECOVERABLE(Stringf("Unknown attribute value, '%s', for attribute, '%s', in element, '%s', from xml", condition_string.c_str(), attribute_name.c_str(), element->Name()));
			}
		}
		else
		{
			ERROR_RECOVERABLE(Stringf("Unknown attribute, '%s', in element, '%s', from xml", attribute_name.c_str(), element->Name()));
		}
	}
}


ConditionContextCheck::~ConditionContextCheck()
{

}


bool ConditionContextCheck::Test()
{
	ERROR_AND_DIE("Have not setup the Trigger::Execute() function");
	return false;
}

String ConditionContextCheck::GetAsString() const
{
	String line;

	switch (m_cardType)
	{
	case CARD_LOCATION:
	{
		line = Stringf("if the location %s", m_cardName.c_str());
		switch (m_qualCondition)
		{
		case QUAL_IS:
		{
			line += " is";
			break;
		}
		case QUAL_IS_NOT:
		{
			line += " is not";
			break;
		}
		}
		break;
	}
	case CARD_CHARACTER:
	{
		line = Stringf("if the character %s", m_cardName.c_str());
		switch (m_qualCondition)
		{
		case QUAL_IS:
		{
			line += " is";
			break;
		}
		case QUAL_IS_NOT:
		{
			line += " is not";
			break;
		}
		}

		break;
	}
	case CARD_ITEM:
	{
		line = Stringf("if the item %s", m_cardName.c_str());
		switch (m_qualCondition)
		{
		case QUAL_IS:
		{
			line += " is";
			break;
		}
		case QUAL_IS_NOT:
		{
			line += " is not";
			break;
		}
		}
		break;
	}
	}

	line += " the subject.";

	return Stringf("ConditionContextCheck: %s", line.c_str());
}



