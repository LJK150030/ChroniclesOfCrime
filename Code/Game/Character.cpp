#include "Game/Character.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

Character::Character() { m_type = CARD_CHARACTER; }
Character::Character(Scenario* the_setup) : Card(the_setup, CARD_CHARACTER) { }
Character::Character(Scenario* the_setup, const String& name, const StringList& list_of_nicknames,
	const String& desc) : Card(the_setup, CARD_CHARACTER, name, list_of_nicknames, desc) { }

Character::Character(Scenario* the_setup, const XmlElement* element)
{
	for (const XmlAttribute* attribute = element->FirstAttribute();
		attribute;
		attribute = attribute->Next()
		)
	{
		String attribute_name = StringToLower(attribute->Name());

		if (attribute_name.compare("name") == 0)
		{
			m_name = attribute->Value();
		}
		else if (attribute_name.compare("startingstate") == 0)
		{
			String state = StringToLower(String(attribute->Value()));
			m_found = state.compare("init") == 0;
		}
		else
		{
			ERROR_RECOVERABLE("Unknown Attribute in location xml file, '%S', skipping attribute", attribute->Name())
		}
	}


	for (const XmlElement* child_element = element->FirstChildElement();
		child_element;
		child_element = child_element->NextSiblingElement()
		)
	{
		String element_name = StringToLower(child_element->Name());

		if (element_name.compare("nicknames") == 0)
		{
			const XmlAttribute* child_attribute = child_element->FirstAttribute();
			const char* nickname_list_c_str = child_attribute->Name();
			ASSERT_OR_DIE(nickname_list_c_str, "Could not get list of nicknames for location %S", m_name.c_str());

			String nickname_list(child_attribute->Value());
			m_nickNames = SplitStringOnDelimiter(nickname_list, ',');
		}
		else if (element_name.compare("state") == 0)
		{
			const XmlAttribute* child_attribute = child_element->FirstAttribute();
			const char* state_c_str = child_attribute->Name();
			ASSERT_OR_DIE(state_c_str, "Could not get State for location %S", m_name.c_str());

			String state(child_attribute->Value());
			if (state.compare("init") == 0)
			{
				const XmlAttribute* sibling_attribute = child_attribute->Next();
				m_description = sibling_attribute->Value();
			}
		}
		else
		{
			ERROR_RECOVERABLE("Unknown Element in location xml file, '%S', skipping element", child_element->Name())
		}
	}
}
