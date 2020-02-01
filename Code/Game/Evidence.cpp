#include "Game/Evidence.hpp"
#include "Game/GameCommon.hpp"

STATIC Evidence Evidence::s_evidenceCategories[NUM_EVIDENCE];
STATIC std::string Evidence::s_searchLine[3];

STATIC void Evidence::LoadInScenarioCards()
{
	// leading scenario cards
	s_evidenceCategories[EVIDENCE_COMPUTER_DEVICES] = Evidence(
		EVIDENCE_COMPUTER_DEVICES,
		"computer",
		"The Hackers Laptop"
	);

	s_evidenceCategories[EVIDENCE_PHOTOS_AND_CAMERAS] = Evidence(
		EVIDENCE_PHOTOS_AND_CAMERAS,
		"camera",
		"A Disposable Camera"
	);

	// loading scenario search lines
	s_searchLine[0] = "I don't think that's relevant";
	s_searchLine[1] = "I don't know about that";
	s_searchLine[2] = "I should keep looking";
}


STATIC std::string Evidence::InvestigateItem(const std::string& name)
{
	//TODO: how can we look up faster with just the name and not the category
	for(int evidence_idx = 0; evidence_idx < NUM_EVIDENCE; ++evidence_idx)
	{
		if(s_evidenceCategories[evidence_idx].m_name.compare(name))
		{
			s_evidenceCategories[evidence_idx].m_found = true;
			return "Found: " + s_evidenceCategories[evidence_idx].m_description;
		}
	}

	return "";
}

Evidence::Evidence(e_EvidenceCategory category, const std::string& name, const std::string& desc):
	m_category(category), m_name(name), m_description(desc)
{	
}

e_EvidenceCategory Evidence::GetCategory()
{
	return m_category;
}


