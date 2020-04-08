#include "Game/Item.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/GPUMesh.hpp"

Item::Item() { m_type = CARD_ITEM; }
Item::Item(Scenario* the_setup) : Card(the_setup, CARD_ITEM) { }
Item::Item(Scenario* the_setup, const String& name, const StringList& list_of_nicknames,
	const String& desc) : Card(the_setup, CARD_ITEM, name, list_of_nicknames, desc) { }

Item::Item(Scenario* the_setup, const XmlElement* element) : Card(the_setup, CARD_ITEM)
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
					-(ITEM_CARD_ASPECT_RATIO * ITEM_CARD_HEIGHT),
					-ITEM_CARD_HEIGHT,
					ITEM_CARD_ASPECT_RATIO * ITEM_CARD_HEIGHT,
					ITEM_CARD_HEIGHT));
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
			ImportItemStatesFromXml(child_element);
		}
		else
		{
			ERROR_RECOVERABLE(Stringf("Unknown Element in location xml file, '%s', skipping element", child_element->Name()))
		}
	}

	m_modelMatrix = m_modelMatrix.MakeTranslation2D(Vec2(55.0f, 10.0f));
	SetState(current_state);
}


const ItemState& Item::GetItemState() const
{
	return m_currentState;
}


void Item::SetState(const String& starting_state)
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


void Item::ImportItemStatesFromXml(const XmlElement* element)
{
	for (const XmlElement* child_element = element->FirstChildElement();
		child_element;
		child_element = child_element->NextSiblingElement()
		)
	{
		String element_name(StringToLower(child_element->Name()));
		ASSERT_OR_DIE(element_name == "state", Stringf("Could not get State for Card %s", m_name.c_str()));

		ItemState new_state;
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
		}

		m_states.push_back(new_state);
	}
}

