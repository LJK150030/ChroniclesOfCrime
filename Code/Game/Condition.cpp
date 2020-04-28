#include "Game/Condition.hpp"
#include "Game/Trigger.hpp"
#include "Game/Incident.hpp"
#include "Game/Scenario.hpp"
#include "Game/Location.hpp"
#include "Game/Character.hpp"
#include "Game/Item.hpp"

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

	
	switch(m_since)
	{
		case ABSOLUTE_GAME_TIME:
		{
			Scenario* the_scenario = m_trigger->GetOwner()->GetOwner();
			GameTime relative_time = the_scenario->GetCurrentTime();

			int relative_time_in_minutes = static_cast<int>(relative_time.m_min);
			relative_time_in_minutes += 60 * static_cast<int>(relative_time.m_hour);
			relative_time_in_minutes += 1440 * static_cast<int>(relative_time.m_day);

			int time_set_in_minutes = static_cast<int>(m_timePassed.m_min);
			time_set_in_minutes += 60 * static_cast<int>(m_timePassed.m_hour);
			time_set_in_minutes += 1440 * static_cast<int>(m_timePassed.m_day);

			const uint time_remaining_in_minutes = time_set_in_minutes - relative_time_in_minutes;

			if(time_remaining_in_minutes <= 0)
			{
				return true;
			}
				
			break;
		}
		case INCIDENT_ENABLED:
		{
			Scenario* the_scenario = m_trigger->GetOwner()->GetOwner();
			const GameTime relative_time = the_scenario->GetCurrentTime();

			int relative_time_in_minutes = static_cast<int>(relative_time.m_min);
			relative_time_in_minutes += 60 * static_cast<int>(relative_time.m_hour);
			relative_time_in_minutes += 1440 * static_cast<int>(relative_time.m_day);
				
			Incident* the_event = m_trigger->GetOwner();
			const GameTime time_event_active = the_event->GetActivatedTime();
			int active_in_minutes = static_cast<int>(time_event_active.m_min);
			active_in_minutes += 60 * static_cast<int>(time_event_active.m_hour);
			active_in_minutes += 1440 * static_cast<int>(time_event_active.m_day);

			const int time_remaining_in_minutes = relative_time_in_minutes - active_in_minutes;
				
			int time_threshold_minutes = static_cast<int>(m_timePassed.m_min);
			time_threshold_minutes += 60 * static_cast<int>(m_timePassed.m_hour);
			time_threshold_minutes += 1440 * static_cast<int>(m_timePassed.m_day);

			if (time_remaining_in_minutes >= time_threshold_minutes)
			{
				return true;
			}

			break;
		}
		default:
		{
			break;
		}
	}
	
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
			m_atLocationName = StringToLower(attribute->Value());
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
	Scenario* the_scenario = m_trigger->GetOwner()->GetOwner();
	Location* current_location = the_scenario->GetCurrentLocation();
	
	LookupItr loc_itr;
	bool is_valid_location = the_scenario->IsLocationInLookupTable(loc_itr, m_atLocationName);

	if(is_valid_location)
	{
		Location* location_lookup = the_scenario->GetLocationFromList(loc_itr->second);

		if (m_playerPresence)
		{
			if (current_location == location_lookup)
			{
				return true;
			}
		}
		else
		{
			if (current_location != location_lookup)
			{
				return true;
			}
		}
	}
	else
	{
		ERROR_RECOVERABLE(Stringf("ConditionLocationCheck error, the name of the location %s is not a valid location", m_atLocationName.c_str()))
	}


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
			m_cardName = StringToLower(attribute->Value());
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
			m_cardStateName = StringToLower(attribute->Value());
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
	Scenario* the_scenario = m_trigger->GetOwner()->GetOwner();

	switch(m_cardType)
	{
		case CARD_LOCATION:
		{
			LookupItr loc_itr;
			const bool is_valid_location = the_scenario->IsLocationInLookupTable(loc_itr, m_cardName);

			if (is_valid_location)
			{
				Location* location_lookup = the_scenario->GetLocationFromList(loc_itr->second);
				LocationState loc_cur_state = location_lookup->GetLocationState();
				String loc_cur_state_name = StringToLower(loc_cur_state.m_name);

				switch(m_qualCondition)
				{
					case QUAL_IS:
					{
						if(loc_cur_state_name == m_cardStateName)
						{
							return true;
						}
						break;
					}
					case QUAL_IS_NOT: 
					{
						if (loc_cur_state_name != m_cardStateName)
						{
							return true;
						}
						break;
					}
					default: 
					{
						ERROR_RECOVERABLE("ConditionStateCheck error, the qual condition was not set properly")
						break;
					}
				}
			}
			else
			{
				ERROR_RECOVERABLE(Stringf("ConditionStateCheck error, the name of the location %s is not a valid location", m_cardName.c_str()))
			}
				
			break;
		}
		case CARD_CHARACTER:
		{
			LookupItr char_itr;
			const bool is_valid_character = the_scenario->IsCharacterInLookupTable(char_itr, m_cardName);

			if (is_valid_character)
			{
				Character* character_lookup = the_scenario->GetCharacterFromList(char_itr->second);
				CharacterState char_cur_state = character_lookup->GetCharacterState();
				String char_cur_state_name = StringToLower(char_cur_state.m_name);

				switch (m_qualCondition)
				{
					case QUAL_IS:
					{
						if (char_cur_state_name == m_cardStateName)
						{
							return true;
						}
						break;
					}
					case QUAL_IS_NOT:
					{
						if (char_cur_state_name != m_cardStateName)
						{
							return true;
						}
						break;
					}
					default:
					{
						ERROR_RECOVERABLE("ConditionStateCheck error, the qual condition was not set properly")
							break;
					}
				}
			}
			else
			{
				ERROR_RECOVERABLE(Stringf("ConditionStateCheck error, the name of the character %s is not a valid character", m_cardName.c_str()))
			}

			break;
		}
		case CARD_ITEM: 
		{
			LookupItr item_itr;
			const bool is_valid_item = the_scenario->IsItemInLookupTable(item_itr, m_cardName);

			if (is_valid_item)
			{
				Item* item_lookup = the_scenario->GetItemFromList(item_itr->second);
				ItemState item_cur_state = item_lookup->GetItemState();
				String item_cur_state_name = StringToLower(item_cur_state.m_name);

				switch (m_qualCondition)
				{
				case QUAL_IS:
				{
					if (item_cur_state_name == m_cardStateName)
					{
						return true;
					}
					break;
				}
				case QUAL_IS_NOT:
				{
					if (item_cur_state_name != m_cardStateName)
					{
						return true;
					}
					break;
				}
				default:
				{
					ERROR_RECOVERABLE("ConditionStateCheck error, the qual condition was not set properly")
						break;
				}
				}
			}
			else
			{
				ERROR_RECOVERABLE(Stringf("ConditionStateCheck error, the name of the item %s is not a valid item", m_cardName.c_str()))
			}

			break;
		}
		default:
		{
			ERROR_AND_DIE("ConditionStateCheck error, CardType was not properly set")
			break;
		}
	}

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
			m_cardName = StringToLower(attribute->Value());
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
	Scenario* the_scenario = m_trigger->GetOwner()->GetOwner();

	switch (m_cardType)
	{
		case CARD_LOCATION:
		{
			LookupItr loc_itr;
			const bool is_valid_location = the_scenario->IsLocationInLookupTable(loc_itr, m_cardName);

			if (is_valid_location)
			{
				Location* location_lookup = the_scenario->GetLocationFromList(loc_itr->second);
				String loc_name = StringToLower(location_lookup->GetName());

				switch (m_qualCondition)
				{
				case QUAL_IS:
				{
					if (loc_name == m_cardName)
					{
						return true;
					}
					break;
				}
				case QUAL_IS_NOT:
				{
					if (loc_name != m_cardName)
					{
						return true;
					}
					break;
				}
				default:
				{
					ERROR_RECOVERABLE("ConditionStateCheck error, the qual condition was not set properly")
						break;
				}
				}
			}
			else
			{
				ERROR_RECOVERABLE(Stringf("ConditionStateCheck error, the name of the location %s is not a valid location", m_cardName.c_str()))
			}

			break;
		}
		case CARD_CHARACTER:
		{
			LookupItr char_itr;
			const bool is_valid_character = the_scenario->IsCharacterInLookupTable(char_itr, m_cardName);

			if (is_valid_character)
			{
				Character* character_lookup = the_scenario->GetCharacterFromList(char_itr->second);
				String char_name = StringToLower(character_lookup->GetName());

				switch (m_qualCondition)
				{
				case QUAL_IS:
				{
					if (char_name == m_cardName)
					{
						return true;
					}
					break;
				}
				case QUAL_IS_NOT:
				{
					if (char_name != m_cardName)
					{
						return true;
					}
					break;
				}
				default:
				{
					ERROR_RECOVERABLE("ConditionStateCheck error, the qual condition was not set properly")
						break;
				}
				}
			}
			else
			{
				ERROR_RECOVERABLE(Stringf("ConditionStateCheck error, the name of the character %s is not a valid character", m_cardName.c_str()))
			}

			break;
		}
		case CARD_ITEM:
		{
			LookupItr item_itr;
			const bool is_valid_item = the_scenario->IsItemInLookupTable(item_itr, m_cardName);

			if (is_valid_item)
			{
				Item* item_lookup = the_scenario->GetItemFromList(item_itr->second);
				String item_name = StringToLower(item_lookup->GetName());

				switch (m_qualCondition)
				{
				case QUAL_IS:
				{
					if (item_name == m_cardName)
					{
						return true;
					}
					break;
				}
				case QUAL_IS_NOT:
				{
					if (item_name != m_cardName)
					{
						return true;
					}
					break;
				}
				default:
				{
					ERROR_RECOVERABLE("ConditionStateCheck error, the qual condition was not set properly")
						break;
				}
				}
			}
			else
			{
				ERROR_RECOVERABLE(Stringf("ConditionStateCheck error, the name of the item %s is not a valid item", m_cardName.c_str()))
			}

			break;
		}
		default:
		{
			ERROR_AND_DIE("ConditionStateCheck error, CardType was not properly set")
				break;
		}
	}
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



