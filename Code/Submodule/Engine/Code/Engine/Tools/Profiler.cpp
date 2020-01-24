#include "Engine/Tools/Profiler.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

Profiler::Profiler(const uint num_alloc_blocks, const double max_history_seconds ):
	m_maxHistorySeconds(max_history_seconds)
{
	m_profilerPool = new(TrackedAllocator::s_instance) ObjectAllocator<ProfilerNode>();
	m_profilerPool->Init(&TrackedAllocator::s_instance, num_alloc_blocks);
	m_treeRootList = std::vector<ProfilerNode*>();
}

Profiler::~Profiler()
{
	for(uint root_idx = 0; root_idx < m_treeRootList.size(); ++root_idx)
	{
		ClearTree(m_treeRootList[root_idx]);
	}

	m_treeRootList.clear();
}

void Profiler::ProfilePush(char const* label)
{
	if(!m_runningProfiler)
	{
		return;
	}
	
	ProfilerNode* child_node = static_cast<ProfilerNode*>(
 		m_profilerPool->Alloc(sizeof(ProfilerNode)));
	
	child_node->m_timeOfCreation = GetCurrentTimeSeconds();
	strcpy_s(child_node->m_name, label);
	child_node->m_next = nullptr;
	
		m_activeNode->m_next = child_node;
		m_activeNode = child_node;
}

void Profiler::ProfilePop()
{
	if(!m_runningProfiler)
	{
		return;
	}
	
	ProfilerNode* child_node = static_cast<ProfilerNode*>(
		m_profilerPool->Alloc(sizeof(ProfilerNode)));

	child_node->m_timeOfCreation = GetCurrentTimeSeconds();
	strcpy_s(child_node->m_name, "");
	child_node->m_next = nullptr;

	{
		std::scoped_lock<std::shared_mutex> lk( m_lock ); 
		m_activeNode->m_next = child_node;
		m_activeNode = child_node;
	}
}

void Profiler::ProfileEndFrame()
{
	if(!m_runningProfiler)
	{
		return;
	}

	ProfilerNode* child_node = static_cast<ProfilerNode*>(
		m_profilerPool->Alloc(sizeof(ProfilerNode)));

	child_node->m_timeOfCreation = GetCurrentTimeSeconds();
	strcpy_s(child_node->m_name, "");
	child_node->m_next = nullptr;

	{
		std::scoped_lock<std::shared_mutex> lk( m_lock ); 
		m_activeNode->m_next = child_node;
		m_activeNode = child_node;
	}
	
	UpdateHistory();
}

ProfilerNode* Profiler::GetRootAtPreviousFrame(uint frame)
{
	ASSERT_OR_DIE(frame < m_treeRootList.size(), "Attempting to get a frame too far back")
	return m_treeRootList.back() - frame;
}

void Profiler::ClearTree(ProfilerNode* root)
{
	//recursively call this function till all children are free
	if(root == nullptr)
	{
		return;
	}

	ClearTree(root->m_next);
	m_profilerPool->Free(root);
}

void Profiler::UpdateHistory()
{
	double current_time = GetCurrentTimeSeconds();

	std::vector<ProfilerNode*>::iterator root_it = m_treeRootList.begin();
	while (root_it != m_treeRootList.end())
	{
		double age = current_time - (*root_it)->m_timeOfCreation;
		if (age > m_maxHistorySeconds) {

			ClearTree(*root_it);

			root_it = m_treeRootList.erase(root_it);
		}
		// Notice that iterator is incremented only on the else part (why?)
		else {
			++root_it;
		}
	}
}

void Profiler::ProfileBeginFrame(char const* label)
{
	if(m_requestedToResume)
	{
		m_requestedToResume = false;
		m_runningProfiler = true;
	}
	else if(m_requestedToPause)
	{
		m_requestedToPause = false;
		m_runningProfiler = false;
	}

	if(!m_runningProfiler)
	{
		return;
	}
	
	ProfilerNode* root = static_cast<ProfilerNode*>(
		m_profilerPool->Alloc(sizeof(ProfilerNode)));

	root->m_timeOfCreation = GetCurrentTimeSeconds();
	strcpy_s(root->m_name, label);
	root->m_next = nullptr;
	
	m_treeRootList.push_back(root);
	m_activeNode = root;
}
