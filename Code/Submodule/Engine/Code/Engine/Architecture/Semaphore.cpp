#include "Engine/Architecture/Semaphore.hpp"

#define WIN32_LEAN_AND_MEAN		// Always #define this before #including <windows.h>
#include <Windows.h>			// #include this (massive, platform-specific) header in very few places


Semaphore::Semaphore( unsigned int initial_count, unsigned int max_count )
{
	Create(initial_count, max_count);
}

Semaphore::~Semaphore()
{
	Destroy();
}

void Semaphore::Create( unsigned int initial_count, unsigned int max_count ) 
{
	// Creating/Initializing
	m_semaphore = ::CreateSemaphore( nullptr,	// security attributes - ignore 
		initial_count,							// count this starts at
		max_count,					// max count this semaphore can reach
		nullptr );							// name, if used across processes
}

// note: should probably call from deconstructor
void Semaphore::Destroy() 
	{
		if (m_semaphore != nullptr) {
			::CloseHandle( m_semaphore ); 
			m_semaphore = nullptr; 
		}
	}

// Acquire a Seamphore
// this will block until the semaphore becomes invalid (destroyed)
// or succeeds
void Semaphore::Acquire() 
{
	::WaitForSingleObject( m_semaphore,		// object to wait on
		INFINITE );              // time to wait in milliseconds
}

// may or may not succeed
// if returns true, the counter was decremented
// if returns false, the counter was 0 and unable to be decremented
bool Semaphore::TryAcquire() 
{
	DWORD result = ::WaitForSingleObject( m_semaphore, 0 ); 
	return (result == WAIT_OBJECT_0); // we successfully waited on the first object (m_semaphroe)
}

// releases the semaphore - ie, adds to the counter up to max
void Semaphore::Release( unsigned int count ) 
{
	::ReleaseSemaphore( m_semaphore, 
		count,      // how many to add to the semaphore
		nullptr );  // out for previous count
}
