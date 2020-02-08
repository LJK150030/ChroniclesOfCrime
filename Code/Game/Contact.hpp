// #pragma once
// #include "Game/Card.hpp"
// 
// 
// class Contact : public Card
// {
// public:
// 	static ContactList	s_contacts;
// 	static LookupTable	s_contactsLookup;
// 	static StringList	s_unknownContactLine;
// 
// public:
// 
// 	static void LoadInScenarioContacts(Game* the_game);
// 	static void ManuallySetContacts(Game* the_game);
// 	static void ReadContactsXml();
// 	static void SetupContactLookupTable();
// 	
// 	// Scenario interaction functions
// 	static String CallSpecialist(const char* name);
// 
// private:
// 	static void AddToContactLookupTable(const String& key_loc_name, int value_idx);
// 	static bool IsContactInLookupTable(LookupItr& out, const String& name);
// 
// public:
// 	Contact();
// 	explicit Contact(Game* the_game);
// 	explicit Contact(Game* the_game, const String& name, const StringList& list_of_nicknames,
// 		const String& desc);
// 	~Contact() = default;
// 
// 	// ACCESSORS
// 
// 
// 	// MUTATORS
// 
// };