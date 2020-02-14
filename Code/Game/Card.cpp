#include "Game/Card.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"

Card::Card() = default;
Card::~Card() = default;

Card::Card(Scenario* the_setup, CardType type) : m_theScenario(the_setup), m_type(type)
{
	m_nickNames = StringList();
	m_states = StateList();
	m_dialogueLines = DialogueList();
}

Card::Card(Scenario* the_setup, const CardType type, const String& name, const StringList& list_of_nicknames,
	const String& desc)	: m_theScenario(the_setup), m_type(type), m_name(name), m_nickNames(list_of_nicknames),
	m_description(desc) { }


bool Card::IsDiscovered() const
{
	return m_found;
}


String& Card::GetName()
{
	return m_name;
}


StringList& Card::GetListOfNicknames()
{
	return m_nickNames;
}


String& Card::GetDescription()
{
	return m_description;
}


void Card::SetDiscovery(const bool discovered)
{
	m_found = discovered;
}


void Card::ImportStatesFromXml(const XmlElement* element)
{
	for (const XmlElement* child_element = element->FirstChildElement();
		child_element;
		child_element = child_element->NextSiblingElement()
		)
	{
		String element_name(StringToLower(child_element->Name()));
		ASSERT_OR_DIE(element_name == "state", Stringf("Could not get State for Card %s", m_name.c_str()));

		State new_state;
		for(const XmlAttribute* attribute = child_element->FirstAttribute();
			attribute;
			attribute = attribute->Next())
		{
			String atr_name(StringToLower(attribute->Name()));
			if(atr_name == "name")
			{
				new_state.m_name = attribute->Value();
			}
			else if(atr_name == "canmovehere")
			{
				new_state.m_canMoveHere = attribute->BoolValue();
			}
			else if (atr_name == "addgametime")
			{
				new_state.m_addGameTime = attribute->BoolValue();
			}
		}

		m_states.push_back(new_state);
	}
}

void Card::ImportDialogueFromXml(const XmlElement* element)
{
	for (const XmlElement* child_element = element->FirstChildElement();
		child_element;
		child_element = child_element->NextSiblingElement()
		)
	{
		String element_name(StringToLower(child_element->Name()));
		ASSERT_OR_DIE(element_name == "line", Stringf("Could not get Dialogue for Card %s", m_name.c_str()));

		Dialogue new_dialogue;
		for (const XmlAttribute* attribute = child_element->FirstAttribute();
			attribute;
			attribute = attribute->Next())
		{
			String atr_name(StringToLower(attribute->Name()));
			if (atr_name == "state")
			{
				new_dialogue.m_inState = attribute->Value();
			}
			else if (atr_name == "line")
			{
				new_dialogue.m_line = attribute->Value();
			}
		}

		m_dialogueLines.push_back(new_dialogue);
	}
}

void Card::SetState(const String& starting_state)
{
	int num_states = static_cast<int>(m_states.size());
	for(int state_idx = 0; state_idx < num_states; ++state_idx)
	{
		if(StringToLower(m_states[state_idx].m_name) == starting_state)
		{
			m_currentState = m_states[state_idx];
			return;
		}
	}

	ERROR_AND_DIE(Stringf("StartingState for Card '%s' was not found in the list of states", m_name.c_str()));
}
