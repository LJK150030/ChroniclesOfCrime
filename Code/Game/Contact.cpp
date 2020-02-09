#include "Game/Contact.hpp"


Contact::Contact() { m_type = CARD_CONTACT; }
Contact::Contact(Scenario* the_setup) : Card(the_setup, CARD_CONTACT) { }
Contact::Contact(Scenario* the_setup, const String& name, const StringList& list_of_nicknames,
	const String& desc) : Card(the_setup, CARD_CONTACT, name, list_of_nicknames, desc) { }
