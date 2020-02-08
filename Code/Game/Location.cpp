#include "Game/Location.hpp"


Location::Location() { m_type = CARD_LOCATION; }
Location::Location(Scenario* the_scene) : Card(the_scene, CARD_LOCATION) { }
Location::Location(Scenario* the_scene, const String& name, const StringList& list_of_nicknames,
	const String& desc) : Card(the_scene, CARD_LOCATION, name, list_of_nicknames, desc) { }


