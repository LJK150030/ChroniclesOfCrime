#pragma once
#include "Game/GameNames.hpp"
#include <string>

class Evidence
{
public:
	static Evidence s_evidenceCategories[NUM_EVIDENCE];
	static std::string s_unknownEvidenceLine[3];

public:
	static void LoadInScenarioEvidence();
	static std::string InvestigateItem(const char* name);

public:
	Evidence() = default;
	Evidence(EvidenceCategory category, const std::string& name, const std::string& desc);
	
	~Evidence() = default;


private:
	EvidenceCategory	m_category = UNKNOWN_EVIDENCE;
	bool				m_found = false;
	std::string			m_name = "";
	std::string			m_description = "";
	
};
