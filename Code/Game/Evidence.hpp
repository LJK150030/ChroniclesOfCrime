#pragma once
#include "Game/GameNames.hpp"
#include <string>

class Evidence
{
public:
	static Evidence s_evidenceCategories[NUM_EVIDENCE];
	static std::string s_searchLine[3];

public:
	static void LoadInScenarioCards();
	static std::string InvestigateItem(const std::string& name);

public:
	Evidence() = default;
	Evidence(e_EvidenceCategory category, const std::string& name, const std::string& desc);
	
	~Evidence() = default;


	e_EvidenceCategory GetCategory();

private:
	e_EvidenceCategory	m_category = UNKNOWN_EVIDENCE;
	bool				m_found = false;
	std::string			m_name = "";
	std::string			m_description = "";
	
};
