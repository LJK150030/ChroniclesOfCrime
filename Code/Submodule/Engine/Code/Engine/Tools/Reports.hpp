#pragma once
#include "Engine/Core/StringUtils.hpp"
#include "Engine/EngineCommon.hpp"
#include "Engine/Core/StringUtils.hpp"

struct ReportNode
{
	char			m_label[64] = "";
	uint			m_numCalls = 0;
	double			m_elapsedTime = 0.0;
	double			m_selfTime = 0.0;

	ReportNode*					m_parent = nullptr;
	std::vector<ReportNode*>	m_children = std::vector<ReportNode*>();
};

class Reports
{
public:

	/**
	 * \brief debug prints the report
	 * \param frame_since_now indexing from the end of the list to the front
	 * \param view true = tree view, false = flat view
	 * \param sort true = total, false = self
	 * \param order true = desc, false = asc
	 */
	void DebugPrintReport( uint frame_since_now = 0, bool view = true, bool sort = true, bool order = true );
	ReportNode* GetTreeFromProfiler( uint frame_since_now = 0 );
	void TreeVeiw(ReportNode* node, int depth);

	std::vector<ReportNode> FlatView(uint frame_since_now = 0);
	void SortByTotal(std::vector<ReportNode>& nodes_to_sort, bool desc = true);
	// std::vector<std::string> FlatByTotalTimeReport( uint frame_since_now = 0, bool order = true);
	// std::vector<std::string> FlatBySelfTimeReport( uint frame_since_now = 0, bool order = true);
	// std::vector<std::string> TotalTimeReport( uint frame_since_now = 0, bool order = true);
	// std::vector<std::string> TreeBySelfTimeReport( uint frame_since_now = 0, bool order = true);

};