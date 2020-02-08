// #pragma once
// #include "Game\Card.hpp"
// 
// 
// //TODO: you can talk to these characters at any time
// class Contact : public Card
// {
// public:
// 	static std::vector<Contact>			s_contacts;
// 	static LookupContainer				s_contactsLookup;
// 	static std::vector<std::string>		s_unknownContactLine;
// 
// public:
// 	static void LoadInScenarioContacts();
// 	static std::string CallSpecialist(const char* name);
// 
// public:
// 	Contact();
// 	explicit Contact(Game* the_game);
// 	explicit Contact(Game* the_game, const std::string& name, const std::vector<std::string>& list_of_nicknames, const std::string& desc);
// 	~Contact() = default;
// 
// 	// ACCESSORS
// 
// 	// MUTATORS 
// 
// };