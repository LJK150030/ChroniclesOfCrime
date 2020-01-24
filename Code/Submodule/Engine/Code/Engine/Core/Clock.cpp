#include "Engine/Core/Clock.hpp"
#include "Engine/EngineCommon.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "ErrorWarningAssert.hpp"

STATIC Clock Clock::g_theMasterClock(nullptr);

Clock::Clock()
{
	SetParent( &g_theMasterClock ); 
}

Clock::Clock(Clock* parent)
{
	SetParent( parent ); 
}

Clock::~Clock()
{		
	// two options;

	// option 1 - children should die before me;  Someone didn't clean up; 
	ASSERT_OR_DIE( m_children.empty(), "There are children here!!! (parent clock class still has children)" ); // other people should have cleaned up 
	SetParent(nullptr); 

	// option 2 - orphan and adopt
	/*
	if (m_parent != nullptr) 
	{
		while (m_children.size() > 0) 
		{
			m_children[0]->SetParent( m_parent ); 
		}
	}
	*/
}

void Clock::Step(double dt)
{
	double delta_time; 
	
	if(m_isPaused) delta_time = 0.0;
	else delta_time = dt * m_dilation;

	if (IsPaused()) {
		delta_time = 0.0; 
	}

	delta_time = ClampDouble( delta_time, 0.0,  m_frameLimit ); 

	++m_frameCount; 
	m_totalTime += delta_time; 
	m_frameTime = delta_time; 

	for (Clock *clockIter : m_children) {
		clockIter->Step( delta_time ); 
	}
}

void Clock::SetFrameLimit(double time)
{
	m_frameLimit = time;
}

void Clock::Dilate(const double dilation)
{
	m_dilation = dilation;
}

void Clock::Pause()
{
	m_pauseCount++;
	
	if(m_pauseCount > 0)
		m_isPaused = true;
	else
		m_isPaused = false;
}

void Clock::Resume()
{
	m_pauseCount--;
	
	if(m_pauseCount > 0)
		m_isPaused = true;
	else
		m_isPaused = false;
}

void Clock::ForcePause()
{
	m_pauseCount = 1u;
	m_isPaused = true;
}

void Clock::ForceResume()
{
	m_pauseCount = 0u;
	m_isPaused = false;
}

bool Clock::IsPaused()
{
	return m_isPaused;
}

void Clock::SetParent(Clock* newParent)
{
	// I have parent...
	if (m_parent != nullptr) {
		m_parent->RemoveChild(this);;
		m_parent = nullptr; 
	}

	m_parent = newParent; 
	if (m_parent != nullptr) {
		m_parent->m_children.push_back(this); 
	}
}

void Clock::RemoveChild(Clock* child)
{
	for(int child_id = 0; child_id < (int) m_children.size(); ++child_id)
	{
		if(m_children[child_id] == child)
		{
			m_children.erase(m_children.begin() + child_id);
			return;
		}
	}
}
