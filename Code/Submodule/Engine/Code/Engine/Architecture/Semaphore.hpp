#pragma once

class Semaphore 
{
public:

	void* m_semaphore; // HANDLE is a void*, if you want to use that in your header; 

public:
	Semaphore( unsigned int initial_count = 0, unsigned int max_count = 1);
	~Semaphore();
	void Create( unsigned int initial_count, unsigned int max_count );
	void Destroy();
	void Acquire();
	bool TryAcquire();
	void Release( unsigned int count = 1 );
	
	inline void Lock()      { Acquire(); }
	inline bool TryLock()  { return TryAcquire(); }; 
	inline void Unlock()    { Release(1); }
};
