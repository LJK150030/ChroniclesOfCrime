#pragma once
#include <cstdint>
#include <mutex>

typedef uint8_t byte;

class AsyncRingBuffer
{
public:
	// members needed
	byte* m_buffer				= nullptr; 
	size_t m_byteSize			= 0; 
	unsigned int m_writeHeadIdx   = 0; 
	unsigned int m_readHeadIdx    = 0; 
	std::mutex m_lock;
	bool m_doubelLockingTest = false;
	
public:
	AsyncRingBuffer(); 
	~AsyncRingBuffer(); 

	bool	Init( size_t size_in_bytes ); 
	void	Deinit();
	
	bool	Write( void const* address_of_data, size_t byte_size ); 
	bool	TryWrite( void const* address_of_data, size_t byte_size ); 
	void*	TryLockWrite( size_t size ); 
	void*	LockWrite( size_t size ); 
	void	UnlockWrite( void* ptr ); 

	bool	Read( void* out_data );
 	bool 	TryRead( void* out_data );
	void*	TryLockRead( size_t* out_size = nullptr ); 
	void*	LockRead( size_t* out_size = nullptr );      
	void	UnlockRead( void* ptr ); 

	size_t	GetWritableSpace() const;

private: 
	struct ringbuffer_header_t 
	{
	public:
		unsigned int size      : 31; // the size of the data  
		unsigned int ready     : 1;  // a ready to read bit
	}; 
};


