#include "Engine/EngineCommon.hpp"
#include "Game/Contact.hpp"

STATIC Contact Contact::s_contacts[NUM_CONTACTS];
STATIC std::string Contact::s_unknownContactLine[3];

STATIC void Contact::LoadInScenarioContacts()
{
	// TODO: There must always be 4 contacts for every scenario
	s_contacts[CONT_CRIMINOLOGIST] = Contact(
		CONT_CRIMINOLOGIST,
		"criminologist",
		"It seems the killer is getting restless"
	);

	s_contacts[CONT_DOCTOR] = Contact(
		CONT_DOCTOR,
		"doctor",
		"Appears to be hemorrhaging on the brain"
	);

	s_contacts[CONT_HACKER] = Contact(
		CONT_DOCTOR,
		"hacker",
		"Looks like we can trace the call"
	);

	s_contacts[CONT_SCIENTIST] = Contact(
		CONT_DOCTOR,
		"scientist",
		"Ballistics shows it was a 20 caliber bullet"
	);


	// loading scenario search lines
	s_unknownContactLine[0] = "*** Busy signal ***";
	s_unknownContactLine[1] = "*Sorry the number your trying to reach has been disconnected or no longer in service*";
	s_unknownContactLine[2] = "what was that number again...";
}

STATIC std::string Contact::CallSpecialist(const char* name)
{
	//TODO: how can we look up faster with just the name and not the category
	for (int contact_idx = 0; contact_idx < NUM_CONTACTS; ++contact_idx)
	{
		if (s_contacts[contact_idx].m_name.compare(name) == 0)
		{
			s_contacts[contact_idx].m_found = true;
			return g_contactHeader + s_contacts[contact_idx].m_description;
		}
	}

	const int random_dialog_idx = g_randomNumberGenerator.GetRandomIntInRange(0, 2);
	return s_unknownContactLine[random_dialog_idx];
}

Contact::Contact(ContactSpecialist specialist, const std::string& name, const std::string& desc) :
	m_category(specialist), m_name(name), m_description(desc)
{
}


