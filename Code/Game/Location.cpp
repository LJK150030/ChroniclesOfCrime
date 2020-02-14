#include "Game/Location.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"


Location::Location()
{
	m_type = CARD_LOCATION;
	m_charsInLoc = std::vector<const Character*>();
}


Location::Location(Scenario* the_setup) : Card(the_setup, CARD_LOCATION)
{
	m_charsInLoc = std::vector<const Character*>();
}


Location::Location(Scenario* the_setup, const String& name, const StringList& list_of_nicknames,
	const String& desc) : Card(the_setup, CARD_LOCATION, name, list_of_nicknames, desc)
{
	
}


Location::Location(Scenario* the_setup, const XmlElement* element): Card(the_setup, CARD_LOCATION)
{
	String current_state = "";

	// Get all the attributes from the Location element
	for (const XmlAttribute* attribute = element->FirstAttribute();
		attribute;
		attribute = attribute->Next()
		)
	{
		String attribute_name = StringToLower(attribute->Name());

		if(attribute_name == "name")
		{
			m_name = attribute->Value();
		}
		else if(attribute_name == "startingstate")
		{
			current_state = StringToLower(String(attribute->Value()));
		}
		else
		{
			ERROR_RECOVERABLE(Stringf("Unknown Attribute in location xml file, '%s', skipping attribute", attribute->Name()))
		}
	}

	// Get all the children elements
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
		else if (element_name == "states")
		{
			ImportStatesFromXml(child_element);
		}
		else if (element_name == "dialogue")
		{
			ImportDialogueFromXml(child_element);
		}
		else
		{
			ERROR_RECOVERABLE(Stringf("Unknown Element in location xml file, '%s', skipping element", child_element->Name()))
		}
	}

	SetState(current_state);
}

bool Location::IsCharacterInLocation(const Character* character) const
{
	int num_char = static_cast<int>(m_charsInLoc.size());
	for (int char_idx = 0; char_idx < num_char; ++char_idx)
	{
		if (m_charsInLoc[char_idx] == character)
		{
			return true;
		}
	}

	return false;
}

String Location::GetBestDialogue() const
{
	String current_state = StringToLower(m_currentState.m_name);
	if(current_state == StringToLower("closed"))
	{
		return g_unknownLocation;
	}

	int num_lines = static_cast<int>(m_dialogueLines.size());
	for (int state_idx = 0; state_idx < num_lines; ++state_idx)
	{
		if (StringToLower(m_dialogueLines[state_idx].m_inState) == current_state)
		{
			return m_dialogueLines[state_idx].m_line;
		}
	}

	ERROR_RECOVERABLE(Stringf("Could not find dialogue for '%s'"), m_name.c_str());
	return String("unfound dialogue");
}


void Location::AddCharacterToLocation(const Character* character)
{
	int num_char = static_cast<int>(m_charsInLoc.size());
	for(int char_idx = 0; char_idx < num_char; ++char_idx)
	{
		if(m_charsInLoc[char_idx] == nullptr)
		{
			m_charsInLoc[char_idx] = character;
			return;
		}
	}

	m_charsInLoc.push_back(character);
}


void Location::RemoveCharacterFromLocation(const Character* character)
{
	int num_char = static_cast<int>(m_charsInLoc.size());
	for (int char_idx = 0; char_idx < num_char; ++char_idx)
	{
		if (m_charsInLoc[num_char] == character)
		{
			m_charsInLoc[num_char] = nullptr;
			return;
		}
	}
}


