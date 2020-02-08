#pragma once
#include "Game/Card.hpp"

class Location: public Card
{
public:
	Location();
	explicit Location(Scenario* the_scene);
	explicit Location(Scenario* the_scene, const String& name, const StringList& list_of_nicknames,
		const String& desc);
	~Location() = default;

	// ACCESSORS

	
	// MUTATORS

};
