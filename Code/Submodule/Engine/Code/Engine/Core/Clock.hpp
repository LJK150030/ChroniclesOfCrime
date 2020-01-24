#pragma once
#include <vector>


class Clock
{  
public:
	static Clock g_theMasterClock; 

public:
	Clock();

	Clock( Clock *parent );

	~Clock();

	void Step( double dt );

	void SetFrameLimit( double time ); 
	void Dilate( const double dilation ); 

	void Pause(); 
	void Resume(); 
	void ForcePause();
	void ForceResume();
	bool IsPaused();

	void SetParent( Clock* newParent ) ;
	void RemoveChild( Clock* child);

public:
	uint64_t m_frameCount   = 0; 
	double m_totalTime      = 0.0; 
	double m_frameTime      = 0.0; 

	double m_frameLimit     = INFINITY;  
	double m_dilation       = 1.0f; 
	int m_pauseCount        = 0U;
	bool m_isPaused			= false;

	Clock *m_parent         = nullptr; 
	std::vector<Clock*> m_children; 

public:
	
};
