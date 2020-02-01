#pragma once
#include "Game/GameNames.hpp"
#include <string>

//TODO: you can talk to these characters at any time
class Contact
{
public:
	static Contact		s_contacts[NUM_CONTACTS];
	static std::string	s_unknownContactLine[3];

public:
	static void LoadInScenarioContacts();
	static std::string CallSpecialist(const char* name);

public:
	Contact() = default;
	Contact(ContactSpecialist specialist, const std::string& name, const std::string& desc);

	~Contact() = default;

private:
	ContactSpecialist	m_category = UNKNOWN_CONTACT;
	bool				m_found = false;
	std::string			m_name = "";
	std::string			m_description = "";

};