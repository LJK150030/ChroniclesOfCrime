#include "Game/Location.hpp"


Location::Location() { m_type = CARD_LOCATION; }
Location::Location(Scenario* the_setup) : Card(the_setup, CARD_LOCATION) { }
Location::Location(Scenario* the_setup, const String& name, const StringList& list_of_nicknames,
	const String& desc) : Card(the_setup, CARD_LOCATION, name, list_of_nicknames, desc) { }


