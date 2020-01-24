#pragma once

#define MAX_TRACE 64

class Callstack 
{
public:
	void* m_trace[MAX_TRACE]{};	// execution pointers representing where we are in code; 
	unsigned int m_depth = 0;	// how full is the above array
	size_t m_hash = 0;
	
public:
	explicit Callstack( unsigned int skip_frames = 0 );
	~Callstack();

	
};
