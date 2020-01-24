#pragma once
#include "Engine/Memory/Allocator.hpp"

typedef uint8_t byte;

struct block_t
{
	byte* m_next;
};

struct chunk_t
{
	byte* m_next;
};

class BlockAllocator : public Allocator
{
public:
	virtual ~BlockAllocator() = default;
	// different ways to initialize the pool
	// one takes a base allocator to sub-allocate out of 
	// (which means it can grow as long as the base can allocate)
	bool Init( Allocator* base, 
		size_t block_size, 
		size_t alignment, 
		uint blocks_per_chunk );

// 	// takes a static buffer, and is not allowed to grow; 
	bool Init( void* buffer, 
		size_t buffer_size,  
		size_t block_size, 
		size_t alignment );

	void DeInit(); 

	// interface implementation
	void* Alloc( size_t byte_size ) override; // works as long as size <= block_size
	void Free( void* ptr ) override; 

	// unique to a block allocator
	// allocates and frees a single block 
	void* AllocBlock();
	void FreeBlock( void* ptr );  

	
private:
	// allocates a single chunk of memory
	// that is divided into blocks - will fail
	// if no base allocator is provided
	bool AllocateChunk();
	bool IsPointerInAlignment( byte* ptr );

private: // members - implementation specific
	Allocator* m_base	= nullptr; // Allocator we're sub-allocating from
	
	byte* m_firstChunk	= nullptr;	// keep track of free blocks 
	byte* m_head		= nullptr;
	byte* m_tail		= nullptr;
	
	size_t m_alignment		= 8; 
	size_t m_blockSize		= 0; 
	size_t m_chunkSize		= 0;
	size_t m_blocksPerChunk = 0;

	std::mutex m_blockLock; // when allocating a single block
};