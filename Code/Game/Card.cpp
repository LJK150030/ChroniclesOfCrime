#include "Game/Card.hpp"

Card::Card() = default;
Card::~Card() = default;

Card::Card(Scenario* the_scene, CardType type) : m_theScenario(the_scene), m_type(type)
{
	m_nickNames = StringList();
}

Card::Card(Scenario* the_scene, const CardType type, const String& name, const StringList& list_of_nicknames,
	const String& desc)	: m_theScenario(the_scene), m_type(type), m_name(name), m_nickNames(list_of_nicknames),
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
