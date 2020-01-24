#include "Engine/Tools/Reports.hpp"
#include "Engine/EngineCommon.hpp"
#include "Engine/Tools/Profiler.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

TODO("Need to implement reporter for profiler")
void Reports::DebugPrintReport( uint frame_since_now, bool view, bool sort, bool order ) {
	UNUSED(view);
	UNUSED(sort);
	UNUSED(order);

	ReportNode* root = GetTreeFromProfiler(frame_since_now);
	root = root->m_children[0];
	
	//SortByTotal(nodes_to_print, true);
	
	DebuggerPrintf("-- Profiler: ------------------------------------ \n");
	DebuggerPrintf("LABEL \t \t \t \t \t \t CALLS \t TOTAL \t SELF \n");
	
	TreeVeiw(root, 0);
	
	DebuggerPrintf("---------------------------------------------------- \n");

}

ReportNode* Reports::GetTreeFromProfiler(uint frame_since_now)
{
	ProfilerNode* grammar_letter = g_profiler->GetRootAtPreviousFrame(frame_since_now);
	ReportNode* root = new ReportNode();
	
	root->m_parent = nullptr;

	ReportNode* current_node = root;
	
	while(grammar_letter != nullptr)
	{
		if(strcmp(grammar_letter->m_name, "") == 0) //  pop
		{
			current_node->m_elapsedTime += grammar_letter->m_timeOfCreation;
			current_node = current_node->m_parent;
		}
		else
		{
			ReportNode* child = new ReportNode();
			strcpy_s(child->m_label, grammar_letter->m_name);
			child->m_elapsedTime = 0.0f - grammar_letter->m_timeOfCreation;
			child->m_selfTime = child->m_elapsedTime;
			child->m_numCalls = 1;
			child->m_parent = current_node;
			
			current_node->m_children.push_back(child);
			current_node = child;
		}
		
		grammar_letter = grammar_letter->m_next;
	}
	
	return current_node;
}

void Reports::TreeVeiw(ReportNode* node, int depth)
{
	std::string line = "";
	
	for (int i = 0; i < depth; ++i) {
		line += "\t";
	}
	
	line += node->m_label;
	DebuggerPrintf("%s \t \t %i \t %F \n", line.c_str(), node->m_numCalls, node->m_elapsedTime);

	std::vector<ReportNode*> children = node->m_children;
	for (int child_idx = 0; child_idx < children.size(); ++child_idx) {
		TreeVeiw(children[child_idx], depth + 1);
	}
}

TODO("Need to implement flatview in reporter for profiler")
std::vector<ReportNode> Reports::FlatView(uint frame_since_now)
{
	UNUSED(frame_since_now);
	
	ProfilerNode* root = g_profiler->GetRootAtPreviousFrame();
	std::vector<ReportNode> nodes_to_process;
	std::vector<ReportNode> nodes_complete;

	while(root != nullptr)
	{
		if(strcmp(root->m_name, "") == 0) //  pop
		{
			ReportNode node = nodes_to_process.back();
			node.m_elapsedTime = root->m_timeOfCreation - node.m_elapsedTime;
			nodes_to_process[nodes_to_process.size() - 1].m_selfTime += root->m_timeOfCreation;

			if(nodes_to_process.size() > 1 )
			{
				nodes_to_process[nodes_to_process.size() - 2].m_selfTime -= root->m_timeOfCreation;
			}
			
			nodes_to_process.pop_back();
			nodes_complete.push_back(node);
		}
		else
		{
			ReportNode node;
			strcpy_s(node.m_label, root->m_name);
			node.m_elapsedTime = root->m_timeOfCreation;
			node.m_selfTime = 0.0f - root->m_timeOfCreation;
			node.m_numCalls = 1;
			
			nodes_to_process.push_back(node);

			if(nodes_to_process.size() > 1 )
			{
				nodes_to_process[nodes_to_process.size() - 2].m_selfTime += root->m_timeOfCreation;
			}
		}

		root = root->m_next;
	}

	return nodes_complete;
}

void Reports::SortByTotal(std::vector<ReportNode>& nodes_to_sort, bool desc)
{
	if(desc)
	{
		sort(nodes_to_sort.begin(), nodes_to_sort.end(), 
			[](const ReportNode & a, const ReportNode & b) -> bool
		{ 
			return a.m_elapsedTime > b.m_elapsedTime; 
		});
	}
	else
	{
		sort(nodes_to_sort.begin(), nodes_to_sort.end(), 
			[](const ReportNode & a, const ReportNode & b) -> bool
		{ 
			return a.m_elapsedTime < b.m_elapsedTime; 
		});
	}
}



