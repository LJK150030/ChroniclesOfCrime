#include "Game/Location.hpp"
#include "Game/Scenario.hpp"
#include "Game/Character.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/CPUMesh.hpp"
#include "Engine/Renderer/GPUMesh.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/RenderContext.hpp"


Location::Location()
{
	m_type = CARD_LOCATION;
	m_charsInLoc = std::vector<const Character*>();
	m_states = LocStateList();
	m_presentingDialogue = LocCharacterIntro();
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
					-(LOC_ASPECT_RATIO * LOC_HEIGHT), 
					-LOC_HEIGHT,
					LOC_ASPECT_RATIO * LOC_HEIGHT,
					LOC_HEIGHT));
			m_mesh = new GPUMesh(g_theRenderer);
			m_mesh->CreateFromCPUMesh<Vertex_Lit>(quad_mesh); // we won't be updated this;
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
			ImportLocationPresentingEntityFromXml(child_element);
		}
		else
		{
			ERROR_RECOVERABLE(Stringf("Unknown Element in location xml file, '%s', skipping element", child_element->Name()))
		}
	}

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

String Location::GetLocationDescription() const
{
	if(!m_currentState.m_canMoveHere)
	{
		return g_unknownLocation;
	}

	Location* current_location = m_theScenario->GetCurrentLocation();
	if(current_location->m_name == m_name)
	{
		return g_sameLocation;
	}

	return m_currentState.m_description;
}


String Location::IntroduceCharacter(const Character* character) const
{
	String loc_state = m_currentState.m_name;
	String char_name = character->GetName();
	String char_state = character->GetCharacterState().m_name;

	const int num_dialogue = static_cast<int>(m_presentingDialogue.size());
	std::vector<int> dialogue_ranking;
	dialogue_ranking.reserve(num_dialogue);
	int highest_idx = -1;
	int highest_score = -1;

	for(int dialogue_idx = 0; dialogue_idx < num_dialogue; ++dialogue_idx)
	{
		CharacterIntro test_state = m_presentingDialogue[dialogue_idx];
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
	
	return m_presentingDialogue[highest_idx].m_line;
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


void Location::ImportLocationPresentingEntityFromXml(const XmlElement* element)
{
	for (const XmlElement* child_element = element->FirstChildElement();
		child_element;
		child_element = child_element->NextSiblingElement()
		)
	{
		String element_name(StringToLower(child_element->Name()));
		ASSERT_OR_DIE(element_name == "line", Stringf("Could not get character introduction for Card %s", m_name.c_str()));

		CharacterIntro new_presentation;
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

		m_presentingDialogue.push_back(new_presentation);
	}
}


