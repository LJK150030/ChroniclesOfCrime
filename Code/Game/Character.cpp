#include "Game/Character.hpp"

Character::Character() { m_type = CARD_CHARACTER; }
Character::Character(Scenario* the_setup) : Card(the_setup, CARD_CHARACTER) { }
Character::Character(Scenario* the_setup, const String& name, const StringList& list_of_nicknames,
	const String& desc) : Card(the_setup, CARD_CHARACTER, name, list_of_nicknames, desc) { }
