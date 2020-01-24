#pragma once
#include "Engine/EngineCommon.hpp"
#include "Engine/Memory/ObjectAllocator.hpp"

struct ProfilerNode
{
	char m_name[64] = "";

	double m_timeOfCreation = 0;

	ProfilerNode* m_next;
};

// A B E ) F K ) ) ) C ) D G ) H ) I ) J ) ) )
// "name" is a push
// "" is a pop
class Profiler
{
public:	
	explicit Profiler( uint num_alloc_blocks, double max_history_seconds );
	~Profiler();

	// cleanup trees that are older than seconds
	void ProfilerSetMaxHistoryTime( double seconds ) { m_maxHistorySeconds = seconds; };

	void ProfilerPause() { m_requestedToPause = true; };  // stop recording (disables creation of new trees, but current trees are allowed to finish)
	void ProfilerResume() { m_requestedToResume = true; }; // resume recording (new trees can be created)

	// RECORDING
	// Recording again should allocate no heap memory, and try to be fairly quick;
	// While active, profiler is always recording, and wants to do its best to not
	// slow down the calling thread; 
	void ProfilePush( char const* label );	// pushes a new child node, and marks it as the active node.  
	void ProfilePop(); 						// pops active node in the tree, or errors if no node is present

	void ProfileBeginFrame( char const* label = "frame" );  // 
	void ProfileEndFrame();

	ProfilerNode* GetRootAtPreviousFrame( uint frame = 0);
	
private:
	//HELPER
	void ClearTree( ProfilerNode* root );
	void UpdateHistory();
	
private:
	
	ObjectAllocator<ProfilerNode>* m_profilerPool = nullptr;
	std::vector<ProfilerNode*> m_treeRootList;

	double m_maxHistorySeconds = 0;
	ProfilerNode* m_activeNode;
	
	bool m_requestedToPause = false;
	bool m_requestedToResume = false;
	bool m_runningProfiler = false;

	std::shared_mutex m_lock; 
};
