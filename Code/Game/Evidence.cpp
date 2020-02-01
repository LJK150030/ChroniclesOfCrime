#include "Game/Evidence.hpp"
#include "Game/GameCommon.hpp"

STATIC Evidence Evidence::s_evidenceCategories[NUM_EVIDENCE];
STATIC std::string Evidence::s_unknownEvidenceLine[3];

STATIC void Evidence::LoadInScenarioEvidence()
{
	// leading scenario cards
	s_evidenceCategories[EVIDENCE_COMPUTER_DEVICES] = Evidence(
		EVIDENCE_COMPUTER_DEVICES,
		"laptop",
		"The Hackers Laptop"
	);

	s_evidenceCategories[EVIDENCE_PHOTOS_AND_CAMERAS] = Evidence(
		EVIDENCE_PHOTOS_AND_CAMERAS,
		"camera",
		"A Disposable Camera"
	);

	// loading scenario search lines
	s_unknownEvidenceLine[0] = "I don't think that's relevant";
	s_unknownEvidenceLine[1] = "I don't know about that";
	s_unknownEvidenceLine[2] = "I should keep looking";
}


STATIC std::string Evidence::InvestigateItem(const char* name)
{
	//TODO: how can we look up faster with just the name and not the category
	for(int evidence_idx = 0; evidence_idx < NUM_EVIDENCE; ++evidence_idx)
	{
		if(s_evidenceCategories[evidence_idx].m_name.compare(name) == 0)
		{
			s_evidenceCategories[evidence_idx].m_found = true;
			return g_evidenceHeader + s_evidenceCategories[evidence_idx].m_description;
		}
	}

	const int random_dialog_idx = g_randomNumberGenerator.GetRandomIntInRange(0, 2);
	return s_unknownEvidenceLine[random_dialog_idx];
}

Evidence::Evidence(EvidenceCategory category, const std::string& name, const std::string& desc):
	m_category(category), m_name(name), m_description(desc)
{	
}
