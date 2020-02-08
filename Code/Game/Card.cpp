#include "Game/Card.hpp"

Card::Card() = default;
Card::~Card() = default;

Card::Card(Game* the_game, CardType type) : m_theGame(the_game), m_type(type)
{
	m_nickNames = StringList();
}

Card::Card(Game* the_game, const CardType type, const String& name, const StringList& list_of_nicknames, 
	const String& desc)	: m_theGame(the_game), m_type(type), m_name(name), m_nickNames(list_of_nicknames),
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
