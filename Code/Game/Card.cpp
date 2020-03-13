#include "Game/Card.hpp"

#include "Engine/Renderer/GPUMesh.hpp"
#include "Engine/Renderer/RenderContext.hpp"


Card::Card() = default;
Card::~Card()
{
	if(m_mesh != nullptr)
	{
		delete m_mesh;
		m_mesh = nullptr;
	}
}


void Card::Render() const
{
	if(m_material != nullptr && m_mesh != nullptr)
	{
		g_theRenderer->BindModelMatrix(m_modelMatrix);
		g_theRenderer->BindMaterial(*m_material);
		g_theRenderer->DrawMesh(*m_mesh);
	}
}


Card::Card(Scenario* the_setup, CardType type) : m_theScenario(the_setup), m_type(type)
{
	m_nickNames = StringList();
}


Card::Card(Scenario* the_setup, const CardType type, const String& name, const StringList& list_of_nicknames,
	const String& desc)	: m_theScenario(the_setup), m_type(type), m_name(name), m_nickNames(list_of_nicknames),
	m_description(desc)
{
	
}


bool Card::IsDiscovered() const
{
	return m_found;
}


CardType Card::GetCardType() const
{
	return m_type;
}


String Card::GetName() const
{
	return m_name;
}


StringList Card::GetListOfNicknames() const
{
	return m_nickNames;
}


String Card::GetDescription() const
{
	return m_description;
}


void Card::SetDiscovery(const bool discovered)
{
	m_found = discovered;
}
