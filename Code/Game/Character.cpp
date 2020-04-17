#include "Game/Character.hpp"
#include "Game/Scenario.hpp"
#include "Game/Location.hpp"
#include "Game/Item.hpp"
#include "Game/Action.hpp"

#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Renderer/GPUMesh.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/RenderContext.hpp"

//------------------------------------------------------------
CharacterDialogue::CharacterDialogue()
{
}


CharacterDialogue::~CharacterDialogue()
{
	uint num_actions = static_cast<uint>(m_actions.size());
	for (uint action_idx = 0; action_idx < num_actions; ++action_idx)
	{
		if (m_actions[action_idx] != nullptr)
		{
			delete m_actions[action_idx];
			m_actions[action_idx] = nullptr;
		}
	}
}


//------------------------------------------------------------
Character::Character() { m_type = CARD_CHARACTER; }
Character::Character(Scenario* the_setup) : Card(the_setup, CARD_CHARACTER) { }
Character::Character(Scenario* the_setup, const String& name, const StringList& list_of_nicknames,
	const String& desc) : Card(the_setup, CARD_CHARACTER, name, list_of_nicknames, desc) { }


Character::Character(Scenario* the_setup, const XmlElement* element) : Card(the_setup, CARD_CHARACTER)
{
	String current_state = "";

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
			current_state = StringToLower(String(attribute->Value()));
		}
		else if (attribute_name == "startloc")
		{
			if (strcmp(attribute->Value(), "NONE") == 0)
			{
				continue;
			}

			LookupItr loc_itr;
			m_theScenario->IsLocationInLookupTable(loc_itr, attribute->Value());
			Location* loc = m_theScenario->GetLocationFromList(loc_itr->second);
			loc->AddCharacterToLocation(this);
		}
		else if (attribute_name == "imagedir")
		{
			String file_name = m_name + ".mat";
			m_material = g_theRenderer->CreateOrGetMaterial(file_name, false);
			m_material->SetShader("default_lit.hlsl");
			m_material->m_shader->SetDepth(COMPARE_LESS_EQUAL, true);

			std::string texture_src = attribute->Value();
			TextureView* texture(reinterpret_cast<TextureView*>(g_theRenderer->CreateOrGetTextureView2D(texture_src)));
			m_material->SetDiffuseMap(texture);

			CPUMesh quad_mesh;
			CpuMeshAddQuad(&quad_mesh,
				AABB2(
					-(CHAR_CARD_ASPECT_RATIO * CHAR_CARD_HEIGHT),
					-CHAR_CARD_HEIGHT,
					CHAR_CARD_ASPECT_RATIO * CHAR_CARD_HEIGHT,
					CHAR_CARD_HEIGHT));
			m_mesh = new GPUMesh(g_theRenderer);
			m_mesh->CreateFromCPUMesh<Vertex_Lit>(quad_mesh); // we won't be updated this;
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
		else if (element_name == "states")
		{
			ImportCharacterStatesFromXml(child_element);
		}
		else if (element_name == "chardialogue")
		{
			ImportCharacterDialogueFromXml(child_element, CARD_CHARACTER);
		}
		else if (element_name == "itemdialogue")
		{
			ImportCharacterDialogueFromXml(child_element, CARD_ITEM);
		}
		else
		{
			ERROR_RECOVERABLE(Stringf("Unknown Element in location xml file, '%s', skipping element", child_element->Name()))
		}
	}

	m_modelMatrix = m_modelMatrix.MakeTranslation2D(Vec2(55.0f, 10.0f));
	SetState(current_state);
}


void Character::ImportCharacterStatesFromXml(const XmlElement* element)
{
	for (const XmlElement* child_element = element->FirstChildElement();
		child_element;
		child_element = child_element->NextSiblingElement()
		)
	{
		String element_name(StringToLower(child_element->Name()));
		ASSERT_OR_DIE(element_name == "state", Stringf("Could not get State for Card %s", m_name.c_str()));

		CharacterState new_state;
		for (const XmlAttribute* attribute = child_element->FirstAttribute();
			attribute;
			attribute = attribute->Next())
		{
			String atr_name(StringToLower(attribute->Name()));

			if (atr_name == "name")
			{
				new_state.m_name = StringToLower(attribute->Value());
			}
			else if (atr_name == "addgametime")
			{
				new_state.m_addGameTime = attribute->BoolValue();
			}
			else if (atr_name == "contextmode")
			{

				String context_mode_str(StringToLower(attribute->Value()));

				if (context_mode_str == "interrogation")
				{
					new_state.m_contextMode = CONTEXT_INTERROGATION;
				}
				else if (context_mode_str == "player character")
				{
					new_state.m_contextMode = CONTEXT_PLAYER_CHARACTER;
				}
			}
		}

		m_states.push_back(new_state);
	}
}


void Character::ImportCharacterDialogueFromXml(const XmlElement* element, CardType type)
{
	int num_scan_lines = 0;

	for (const XmlElement* child_element = element->FirstChildElement();
		child_element;
		child_element = child_element->NextSiblingElement()
		)
	{
		num_scan_lines++;
	}

	switch (type)
	{
	case CARD_CHARACTER:
	{
		m_dialogueAboutCharacter.reserve(num_scan_lines);
		break;
	}
	case CARD_ITEM:
	{
		m_dialogueAboutItem.reserve(num_scan_lines);
		break;
	}
	}

	for (const XmlElement* child_element = element->FirstChildElement();
		child_element;
		child_element = child_element->NextSiblingElement()
		)
	{
		String element_name(StringToLower(child_element->Name()));
		ASSERT_OR_DIE(element_name == "scan", Stringf("Could not get character dialogue for Card %s", m_name.c_str()));

		CharacterDialogue* new_dialog = nullptr;

		switch (type)
		{
		case CARD_CHARACTER:
		{
			m_dialogueAboutCharacter.emplace_back();
			new_dialog = &m_dialogueAboutCharacter.back();

			break;
		}
		case CARD_ITEM:
		{
			m_dialogueAboutItem.emplace_back();
			new_dialog = &m_dialogueAboutItem.back();
			break;
		}
		}

		new_dialog->m_cardType = type;
		for (const XmlAttribute* attribute = child_element->FirstAttribute();
			attribute;
			attribute = attribute->Next())
		{
			String atr_name(StringToLower(attribute->Name()));

			if (atr_name == "state")
			{
				new_dialog->m_characterState = StringToLower(attribute->Value());
			}
			else if (atr_name == "loc")
			{
				new_dialog->m_locationName = StringToLower(attribute->Value());
			}
			else if (atr_name == "locstate")
			{
				new_dialog->m_locationState = StringToLower(attribute->Value());
			}
			else if (atr_name == "char")
			{
				new_dialog->m_cardName = StringToLower(attribute->Value());
			}
			else if (atr_name == "charstate")
			{
				new_dialog->m_cardState = StringToLower(attribute->Value());
			}
			else if (atr_name == "line")
			{
				new_dialog->m_line = attribute->Value();
			}
		}

		const XmlElement* grand_child_element = child_element->FirstChildElement();
		if (grand_child_element != nullptr)
		{
			element_name = StringToLower(grand_child_element->Name());

			if (element_name == "setcardstate")
			{
				new_dialog->m_actions.push_back(
					new ActionChangeCardState(m_theScenario, grand_child_element)
				);
			}
		}

	}
}


const CharacterState& Character::GetCharacterState() const
{
	return m_currentState;
}


bool Character::AskAboutCharacter(String& out, const Location* location, const Character* character)
{
	String loc_name = location->GetName();
	String loc_state = location->GetLocationState().m_name;
	String char_name = StringToLower(character->GetName());
	String char_state = StringToLower(character->GetCharacterState().m_name);
	
	const int num_dialogue = static_cast<int>(m_dialogueAboutCharacter.size());
	std::vector<int> dialogue_ranking;
	dialogue_ranking.reserve(num_dialogue);
	
	int highest_idx = -1;
	int highest_score = -1;

	for (int dialogue_idx = 0; dialogue_idx < num_dialogue; ++dialogue_idx)
	{
		CharacterDialogue& test_state = m_dialogueAboutCharacter[dialogue_idx];
		int score = 0;

		//testing character state
		if (test_state.m_characterState == "*")
		{
			score += 1;
		}
		else if (test_state.m_characterState == m_currentState.m_name)
		{
			score += 3;
		}

		//testing location name
		if (test_state.m_locationName == "*")
		{
			score += 1;
		}
		else if (test_state.m_locationName == loc_name)
		{
			score += 3;

			//testing location name
			if (test_state.m_locationState == "*")
			{
				score += 1;
			}
			else if (test_state.m_locationState == loc_state)
			{
				score += 5;
			}
		}

		//testing character name
		if (test_state.m_cardName == "*")
		{
			score += 1;
		}
		else if (test_state.m_cardName == char_name)
		{
			score += 3;

			//testing character state
			if (test_state.m_cardState == "*")
			{
				score += 1;
			}
			else if (test_state.m_cardState == char_state)
			{
				score += 5;
			}
		}

		//testing score
		if (score > highest_score)
		{
			highest_score = score;
			highest_idx = dialogue_idx;
		}
	}

	out += m_dialogueAboutCharacter[highest_idx].m_line;

	const int num_actions = static_cast<int>(m_dialogueAboutCharacter[highest_idx].m_actions.size());

	for (int act_idx = 0; act_idx < num_actions; ++act_idx)
	{
		m_dialogueAboutCharacter[highest_idx].m_actions[act_idx]->Execute();
	}

	if (highest_idx != 0)
	{
		return true;
	}

	return false;
}



bool Character::AskAboutItem(String& out, const Location* location, const Item* item)
{
	String loc_name = location->GetName();
	String loc_state = location->GetLocationState().m_name;
	String item_name = StringToLower(item->GetName());
	String item_state = StringToLower(item->GetItemState().m_name);

	const int num_dialogue = static_cast<int>(m_dialogueAboutItem.size());
	std::vector<int> dialogue_ranking;
	dialogue_ranking.reserve(num_dialogue);

	int highest_idx = -1;
	int highest_score = -1;

	for (int dialogue_idx = 0; dialogue_idx < num_dialogue; ++dialogue_idx)
	{
		CharacterDialogue& test_state = m_dialogueAboutItem[dialogue_idx];
		int score = 0;

		//testing character state
		if (test_state.m_characterState == "*")
		{
			score += 1;
		}
		else if (test_state.m_characterState == m_currentState.m_name)
		{
			score += 3;
		}

		//testing location name
		if (test_state.m_locationName == "*")
		{
			score += 1;
		}
		else if (test_state.m_locationName == loc_name)
		{
			score += 3;

			//testing location name
			if (test_state.m_locationState == "*")
			{
				score += 1;
			}
			else if (test_state.m_locationState == loc_state)
			{
				score += 5;
			}
		}

		//testing character name
		if (test_state.m_cardName == "*")
		{
			score += 1;
		}
		else if (test_state.m_cardName == item_name)
		{
			score += 3;

			//testing character state
			if (test_state.m_cardState == "*")
			{
				score += 1;
			}
			else if (test_state.m_cardState == item_name)
			{
				score += 5;
			}
		}

		//testing score
		if (score > highest_score)
		{
			highest_score = score;
			highest_idx = dialogue_idx;
		}
	}

	out += m_dialogueAboutItem[highest_idx].m_line;

	const int num_actions = static_cast<int>(m_dialogueAboutItem[highest_idx].m_actions.size());

	for (int act_idx = 0; act_idx < num_actions; ++act_idx)
	{
		m_dialogueAboutItem[highest_idx].m_actions[act_idx]->Execute();
	}

	if (highest_idx != 0)
	{
		return true;
	}

	return false;
}


void Character::SetState(const String& starting_state)
{
	int num_states = static_cast<int>(m_states.size());
	for (int state_idx = 0; state_idx < num_states; ++state_idx)
	{
		if (StringToLower(m_states[state_idx].m_name) == starting_state)
		{
			m_currentState = m_states[state_idx];
			return;
		}
	}

	ERROR_AND_DIE(Stringf("StartingState for Card '%s' was not found in the list of states", m_name.c_str()));
}
