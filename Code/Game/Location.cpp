#include "Game/Location.hpp"
#include "Game/Scenario.hpp"
#include "Game/Character.hpp"
#include "Game/Item.hpp"
#include "Game/Action.hpp"


#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/CPUMesh.hpp"
#include "Engine/Renderer/GPUMesh.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/RenderContext.hpp"


CharacterIntro::CharacterIntro()
{
}

CharacterIntro::~CharacterIntro()
{
	uint num_actions = static_cast<uint>(m_actions.size());
	for(uint action_idx = 0; action_idx < num_actions; ++action_idx)
	{
		if(m_actions[action_idx] != nullptr)
		{
			delete m_actions[action_idx];
			m_actions[action_idx] = nullptr;
		}
	}
}


Location::Location()
{
	m_type = CARD_LOCATION;
	m_charsInLoc = std::vector<const Character*>();
	m_states = LocStateList();
	m_presentingCharacterDialogue = LocCharacterIntro();
	m_presentingItemDialogue = LocItemIntro();
}


Location::Location(Scenario* the_setup) : Card(the_setup, CARD_LOCATION)
{
	m_charsInLoc = std::vector<const Character*>();
}


Location::Location(Scenario* the_setup, const String& name, const StringList& list_of_nicknames,
	const String& desc) : Card(the_setup, CARD_LOCATION, name, list_of_nicknames, desc) { }


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
		else if(attribute_name == "imagedir")
		{
			String file_name = m_name + "mat";
			m_material = g_theRenderer->CreateOrGetMaterial(file_name, false);
			m_material->SetShader("default_lit.hlsl");
			m_material->m_shader->SetDepth(COMPARE_LESS_EQUAL, true);

			std::string texture_src = attribute->Value();
			TextureView* texture(reinterpret_cast<TextureView*>(g_theRenderer->CreateOrGetTextureView2D(texture_src)));
			m_material->SetDiffuseMap(texture);

			CPUMesh quad_mesh;
			CpuMeshAddQuad(&quad_mesh, 
				AABB2(
					-(LOC_CARD_ASPECT_RATIO * LOC_CARD_HEIGHT),
					-LOC_CARD_HEIGHT,
					LOC_CARD_ASPECT_RATIO * LOC_CARD_HEIGHT,
					LOC_CARD_HEIGHT));
			m_mesh = new GPUMesh(g_theRenderer);
			m_mesh->CreateFromCPUMesh<Vertex_Lit>(quad_mesh); // we won't be updated this;
		}
		else if(attribute_name == "roomdir")
		{
			//todo present room;
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
			ImportLocationStatesFromXml(child_element);
		}
		else if (element_name == "introducecharacter")
		{
			ImportLocationPresentingCharacterFromXml(child_element);
		}
		else if(element_name == "introduceitem")
		{
			ImportLocationPresentingItemFromXml(child_element);
		}
		else
		{
			ERROR_RECOVERABLE(Stringf("Unknown Element in location xml file, '%s', skipping element", child_element->Name()))
		}
	}

	//m_modelMatrix = m_modelMatrix.MakeTranslation2D(Vec2(0.0f, -5.0f));
	SetState(current_state);
}


Location::~Location()
{
	//debug does not track this step when saying =default
}


bool Location::IsCharacterInLocation(const Character* character) const
{
	int num_char = static_cast<int>(m_charsInLoc.size());
	for (int char_idx = 0; char_idx < num_char; ++char_idx)
	{
		if (m_charsInLoc[char_idx]->GetName() == character->GetName())
		{
			return true;
		}
	}

	return false;
}

bool Location::GetLocationDescription(String& out) const
{
	Location* cur_loc = m_theScenario->GetCurrentLocation();

	if (cur_loc->GetName() == m_name)
	{
		out += g_sameLocationMessage;
		return false;
	}
	else if(!m_currentState.m_canMoveHere)
	{
		out += g_closedLocationMessage;
		return false;
	}
	else
	{
		out += m_currentState.m_description;
		return true;
	}
}


bool Location::IntroduceCharacter(String& out, const Character* character) const
{
	String loc_state = m_currentState.m_name;
	String char_name = character->GetName();
	String char_state = character->GetCharacterState().m_name;

	if (char_name == m_name)
	{
		out += g_unknownCommandMessage;
		return false;
	}

	const int num_dialogue = static_cast<int>(m_presentingCharacterDialogue.size());
	std::vector<int> dialogue_ranking;
	dialogue_ranking.reserve(num_dialogue);
	int highest_idx = -1;
	int highest_score = -1;

	for(int dialogue_idx = 0; dialogue_idx < num_dialogue; ++dialogue_idx)
	{
		CharacterIntro test_state = m_presentingCharacterDialogue[dialogue_idx];
		int score = 0;

		//testing location state
		if(test_state.m_locationState == "*")
		{
			score += 1;
		}
		else if(test_state.m_locationState == loc_state)
		{
			score += 3;
		}

		//testing character name
		if (test_state.m_characterName == "*")
		{
			score += 1;
		}
		else if (test_state.m_characterName == char_name)
		{
			score += 3;
		}

		//testing character state
		if (test_state.m_characterState == "*")
		{
			score += 1;
		}
		else if (test_state.m_characterState == char_state)
		{
			score += 3;
		}

		//testing score
		if(score > highest_score)
		{
			highest_score = score;
			highest_idx = dialogue_idx;
		}
	}

	if(highest_score == 3)
	{
		out += m_presentingCharacterDialogue[highest_idx].m_line;
		return false;
	}
	else
	{
		out += m_presentingCharacterDialogue[highest_idx].m_line;
		return true;
	}
}


bool Location::IntroduceItem(String& out, const Item* item) const
{
	String loc_state = m_currentState.m_name;
	String item_name = item->GetName();
	String item_state = item->GetItemState().m_name;

	if (item_name == m_name)
	{
		out += g_unknownCommandMessage;
		return false;
	}

	const int num_dialogue = static_cast<int>(m_presentingItemDialogue.size());
	std::vector<int> dialogue_ranking;
	dialogue_ranking.reserve(num_dialogue);
	int highest_idx = -1;
	int highest_score = -1;

	for (int dialogue_idx = 0; dialogue_idx < num_dialogue; ++dialogue_idx)
	{
		ItemIntro test_state = m_presentingItemDialogue[dialogue_idx];
		int score = 0;

		//testing location state
		if (test_state.m_locationState == "*")
		{
			score += 1;
		}
		else if (test_state.m_locationState == loc_state)
		{
			score += 3;
		}

		//testing character name
		if (test_state.m_itemName == "*")
		{
			score += 1;
		}
		else if (test_state.m_itemName == item_name)
		{
			score += 3;
		}

		//testing character state
		if (test_state.m_itemState == "*")
		{
			score += 1;
		}
		else if (test_state.m_itemState == item_state)
		{
			score += 3;
		}

		//testing score
		if (score > highest_score)
		{
			highest_score = score;
			highest_idx = dialogue_idx;
		}
	}

	if (highest_score == 3)
	{
		out += m_presentingItemDialogue[highest_idx].m_line;
		return false;
	}
	else
	{
		out += m_presentingItemDialogue[highest_idx].m_line;
		return true;
	}
}


const LocationState& Location::GetLocationState() const
{
	return m_currentState;
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


void Location::SetState(const String& starting_state)
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


void Location::ImportLocationStatesFromXml(const XmlElement* element)
{
	for (const XmlElement* child_element = element->FirstChildElement();
		child_element;
		child_element = child_element->NextSiblingElement()
		)
	{
		String element_name(StringToLower(child_element->Name()));
		ASSERT_OR_DIE(element_name == "state", Stringf("Could not get State for Card %s", m_name.c_str()));

		LocationState new_state;
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
			else if (atr_name == "description")
			{
				new_state.m_description = attribute->Value();
			}
		}

		m_states.push_back(new_state);
	}
}


void Location::ImportLocationPresentingCharacterFromXml(const XmlElement* element)
{
	int num_scan_lines = 0;
	for (const XmlElement* child_element = element->FirstChildElement();
		child_element;
		child_element = child_element->NextSiblingElement()
		)
	{
		num_scan_lines++;
	}
	m_presentingCharacterDialogue.reserve(num_scan_lines);

	for (const XmlElement* child_element = element->FirstChildElement();
		child_element;
		child_element = child_element->NextSiblingElement()
		)
	{
		String element_name(StringToLower(child_element->Name()));
		ASSERT_OR_DIE(element_name == "scan", Stringf("Could not get character introduction for Card %s", m_name.c_str()));

		m_presentingCharacterDialogue.emplace_back();
		CharacterIntro &new_presentation = m_presentingCharacterDialogue.back();
		for (const XmlAttribute* attribute = child_element->FirstAttribute();
			attribute;
			attribute = attribute->Next())
		{
			String atr_name(StringToLower(attribute->Name()));
			
			if (atr_name == "state")
			{
				new_presentation.m_locationState = attribute->Value();
			}
			else if (atr_name == "character")
			{
				new_presentation.m_characterName = attribute->Value();
			}
			else if (atr_name == "characterstate")
			{
				new_presentation.m_characterState = attribute->Value();
			}
			else if (atr_name == "line")
			{
				new_presentation.m_line = attribute->Value();
			}
		}

		const XmlElement* grand_child_element = child_element->FirstChildElement();
		if (grand_child_element != nullptr)
		{
			element_name = StringToLower(grand_child_element->Name());

			if (element_name == "setcardstate")
			{
				CardType type = UNKNOWN_CARD_TYPE;
				String target_loc_name = "";
				String from_loc_state = "";
				String to_loc_state = "";


				for (const XmlAttribute* attribute = grand_child_element->FirstAttribute();
					attribute;
					attribute = attribute->Next())
				{
					String atr_name(StringToLower(attribute->Name()));

					if (atr_name == "type")
					{
						String card_type = StringToLower(attribute->Value());
						if (card_type == "location")
						{
							type = CARD_LOCATION;
						}
						else if (card_type == "character")
						{
							type = CARD_CHARACTER;
						}
						else if (card_type == "item")
						{
							type = CARD_ITEM;
						}

					}
					else if (atr_name == "name")
					{
						target_loc_name = StringToLower(attribute->Value());
					}
					else if (atr_name == "fromstate")
					{
						from_loc_state = StringToLower(attribute->Value());
					}
					else if (atr_name == "tostate")
					{
						to_loc_state = StringToLower(attribute->Value());
					}
					else
					{
						ERROR_AND_DIE("Unexpected attribute in SetLocState")
					}
				}

				new_presentation.m_actions.push_back(new ChangeCardState(m_theScenario, type, target_loc_name, from_loc_state, to_loc_state));

			}
		}

	}
}


void Location::ImportLocationPresentingItemFromXml(const XmlElement* element)
{
	int num_scan_lines = 0;
	for (const XmlElement* child_element = element->FirstChildElement();
		child_element;
		child_element = child_element->NextSiblingElement()
		)
	{
		num_scan_lines++;
	}
	m_presentingItemDialogue.reserve(num_scan_lines);

	for (const XmlElement* child_element = element->FirstChildElement();
		child_element;
		child_element = child_element->NextSiblingElement()
		)
	{
		String element_name(StringToLower(child_element->Name()));
		ASSERT_OR_DIE(element_name == "scan", Stringf("Could not get item introduction for Card %s", m_name.c_str()));

		m_presentingItemDialogue.emplace_back();
		ItemIntro& new_presentation = m_presentingItemDialogue.back();
		for (const XmlAttribute* attribute = child_element->FirstAttribute();
			attribute;
			attribute = attribute->Next())
		{
			String atr_name(StringToLower(attribute->Name()));

			if (atr_name == "state")
			{
				new_presentation.m_locationState = attribute->Value();
			}
			else if (atr_name == "item")
			{
				new_presentation.m_itemName = attribute->Value();
			}
			else if (atr_name == "itemstate")
			{
				new_presentation.m_itemState = attribute->Value();
			}
			else if (atr_name == "line")
			{
				new_presentation.m_line = attribute->Value();
			}
		}

		const XmlElement* grand_child_element = child_element->FirstChildElement();
		if (grand_child_element != nullptr)
		{
			element_name = StringToLower(grand_child_element->Name());

			if (element_name == "setcardstate")
			{
				CardType type = UNKNOWN_CARD_TYPE;
				String target_loc_name = "";
				String from_loc_state = "";
				String to_loc_state = "";


				for (const XmlAttribute* attribute = grand_child_element->FirstAttribute();
					attribute;
					attribute = attribute->Next())
				{
					String atr_name(StringToLower(attribute->Name()));

					if (atr_name == "type")
					{
						String card_type = StringToLower(attribute->Value());
						if (card_type == "location")
						{
							type = CARD_LOCATION;
						}
						else if (card_type == "character")
						{
							type = CARD_CHARACTER;
						}
						else if (card_type == "item")
						{
							type = CARD_ITEM;
						}

					}
					else if (atr_name == "name")
					{
						target_loc_name = StringToLower(attribute->Value());
					}
					else if (atr_name == "fromstate")
					{
						from_loc_state = StringToLower(attribute->Value());
					}
					else if (atr_name == "tostate")
					{
						to_loc_state = StringToLower(attribute->Value());
					}
					else
					{
						ERROR_AND_DIE("Unexpected attribute in SetLocState")
					}
				}

				new_presentation.m_actions.push_back(new ChangeCardState(m_theScenario, type, target_loc_name, from_loc_state, to_loc_state));

			}
		}

	}
}


