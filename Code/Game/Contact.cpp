// #include "Game/Contact.hpp"
// 
// #include "Engine/EngineCommon.hpp"
// #include "Engine/Core/ErrorWarningAssert.hpp"
// 
// // STATIC ContactList	Contact::s_contacts = ContactList();
// // STATIC LookupTable	Contact::s_contactsLookup = LookupTable();
// // STATIC StringList	Contact::s_unknownContactLine = StringList();
// // 
// // 
// // STATIC void Contact::LoadInScenarioContacts(Game* the_game)
// // {
// // 	ManuallySetContacts(the_game);
// // 	SetupContactLookupTable();
// // }
// // 
// // 
// // void Contact::ManuallySetContacts(Game* the_game)
// // {
// // 	// loading scenario search lines
// // 	s_unknownContactLine.emplace_back("*** Busy signal ***");
// // 	s_unknownContactLine.emplace_back("*Sorry the number your trying to reach has been disconnected or no longer in service*");
// // 	s_unknownContactLine.emplace_back("what was that number again...");
// // 	
// // 	// Criminologist RIMINOLOGIST contact 
// // 	StringList home_nicknames;
// // 	home_nicknames.emplace_back("Station");
// // 	home_nicknames.emplace_back("Headquarters");
// // 	home_nicknames.emplace_back("Police Station");
// // 	String home_des = "You arrive at HQ, and your partner waves to you.";
// // 	s_locations.emplace_back(the_game, "Home", home_nicknames, home_des);
// // 
// // 	// Westminster location
// // 	StringList crime_nicknames;
// // 	crime_nicknames.emplace_back("Apartment");
// // 	crime_nicknames.emplace_back("Victim's home");
// // 	String westminster_des = "You arrive at the crime scene. There is blood everywhere!";
// // 	s_locations.emplace_back(the_game, "Westminster", crime_nicknames, westminster_des);
// // 	
// // 	// TODO: There must always be 4 contacts for every scenario
// // 	s_contacts[CONT_CRIMINOLOGIST] = Contact(
// // 		CONT_CRIMINOLOGIST,
// // 		"criminologist",
// // 		"It seems the killer is getting restless"
// // 	);
// // 
// // 	s_contacts[CONT_DOCTOR] = Contact(
// // 		CONT_DOCTOR,
// // 		"doctor",
// // 		"Appears to be hemorrhaging on the brain"
// // 	);
// // 
// // 	s_contacts[CONT_HACKER] = Contact(
// // 		CONT_HACKER,
// // 		"hacker",
// // 		"Looks like we can trace the call"
// // 	);
// // 
// // 	s_contacts[CONT_SCIENTIST] = Contact(
// // 		CONT_SCIENTIST,
// // 		"scientist",
// // 		"Ballistics shows it was a 20 caliber bullet"
// // 	);
// // }
// // 
// // 
// // void Contact::ReadContactsXml()
// // {
// // 	ASSERT_OR_DIE(false, "ReadLocationsXml has not been implemented yet!")
// // }
// // 
// // 
// // void Contact::SetupContactLookupTable()
// // {
// // }
// // 
// // 
// // void Contact::AddToContactLookupTable(const String& key_loc_name, int value_idx)
// // {
// // }
// // 
// // 
// // bool Contact::IsContactInLookupTable(LookupItr& out, const String& name)
// // {
// // }
// // 
// 
// 
// 
// std::string Contact::CallSpecialist(const char* name)
// {
// 	//TODO: how can we look up faster with just the name and not the category
// 	for (int contact_idx = 0; contact_idx < NUM_CONTACTS; ++contact_idx)
// 	{
// 		if (s_contacts[contact_idx].m_name.compare(name) == 0)
// 		{
// 			s_contacts[contact_idx].m_found = true;
// 			return g_contactHeader + s_contacts[contact_idx].m_description;
// 		}
// 	}
// 
// 	const int random_dialog_idx = g_randomNumberGenerator.GetRandomIntInRange(0, 2);
// 	return s_unknownContactLine[random_dialog_idx];
// }
// 
// 
// Contact::Contact() { m_type = CARD_CONTACT; }
// Contact::Contact(Game* the_game): Card(the_game, CARD_CONTACT) { }
// Contact::Contact(Game* the_game, const std::string& name, const std::vector<std::string>& list_of_nicknames,
// 	const std::string& desc) : Card(the_game, CARD_CONTACT, name, list_of_nicknames, desc) { }
// 
