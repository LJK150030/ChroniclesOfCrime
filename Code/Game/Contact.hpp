 #pragma once
#include "Game/Card.hpp"


class Contact : public Card
{
public:
	Contact();
	explicit Contact(Scenario* the_setup);
	explicit Contact(Scenario* the_setup, const String& name, const StringList& list_of_nicknames,
		const String& desc);
	explicit Contact(Scenario* the_setup, const XmlElement* element);
	~Contact() = default;

	// ACCESSORS


	// MUTATORS

};
