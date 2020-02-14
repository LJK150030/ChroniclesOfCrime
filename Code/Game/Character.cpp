#include "Game/Character.hpp"
#include "Game/Scenario.hpp"
#include "Game/Location.hpp"

#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

Character::Character() { m_type = CARD_CHARACTER; }
Character::Character(Scenario* the_setup) : Card(the_setup, CARD_CHARACTER) { }
Character::Character(Scenario* the_setup, const String& name, const StringList& list_of_nicknames,
	const String& desc) : Card(the_setup, CARD_CHARACTER, name, list_of_nicknames, desc) { }

Character::Character(Scenario* the_setup, const XmlElement* element) : Card(the_setup, CARD_CHARACTER)
{
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
		else if (attribute_name == "startingstate")
		{
			String state = StringToLower(String(attribute->Value()));
			m_found = state == "init";
		}
		else if (attribute_name == "startloc")
		{
			LookupItr loc_itr;
			m_theScenario->IsLocationInLookupTable(loc_itr, attribute->Value());
			Location* loc = m_theScenario->GetLocationFromList(loc_itr->second);
			loc->AddCharacterToLocation(this);
		}
		else
		{
			ERROR_RECOVERABLE(Stringf("Unknown Attribute in location xml file, '%s', skipping attribute", attribute->Name()))
		}
	}


	for (const XmlElement* child_element = element->FirstChildElement();
		child_element;
		child_element = child_element->NextSiblingElement()
		)
	{
		String element_name = StringToLower(child_element->Name());

		if (element_name == "nicknames")
		{
			const XmlAttribute* child_attribute = child_element->FirstAttribute();
			const char* nickname_list_c_str = child_attribute->Name();
			ASSERT_OR_DIE(nickname_list_c_str, Stringf("Could not get list of nicknames for location %s", m_name.c_str()));

			String nickname_list(child_attribute->Value());
			m_nickNames = SplitStringOnDelimiter(nickname_list, ',');
		}
		else if (element_name == "state")
		{
			const XmlAttribute* child_attribute = child_element->FirstAttribute();
			const char* state_c_str = child_attribute->Name();
			ASSERT_OR_DIE(state_c_str, Stringf("Could not get State for location %s", m_name.c_str()));

			String state(child_attribute->Value());
			if (state == "init")
			{
				const XmlAttribute* sibling_attribute = child_attribute->Next();
				m_description = sibling_attribute->Value();
			}
		}
		else
		{
			ERROR_RECOVERABLE(Stringf("Unknown Element in location xml file, '%s', skipping element", child_element->Name()))
		}
	}
}
