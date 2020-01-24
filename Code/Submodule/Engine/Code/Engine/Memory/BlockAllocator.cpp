#include "Engine/Memory/BlockAllocator.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

bool BlockAllocator::Init(Allocator* base, size_t block_size, size_t alignment, uint blocks_per_chunk)
{
	std::scoped_lock<std::mutex> lock(m_blockLock);

	ASSERT_OR_DIE(block_size >= sizeof(block_t), "block_size is too small for BlockAllocator");

	m_base = base;
	m_blockSize = block_size;
	m_alignment = alignment;
	m_blocksPerChunk = blocks_per_chunk;
	m_chunkSize = sizeof(chunk_t) + block_size * blocks_per_chunk;
	
	AllocateChunk();
	return true;
}

bool BlockAllocator::Init(void* buffer, size_t buffer_size, size_t block_size, size_t alignment)
{
	std::scoped_lock<std::mutex> lock(m_blockLock);
	
	ASSERT_OR_DIE(block_size >= sizeof(block_t), "block_size is too small for BlockAllocator");
	ASSERT_OR_DIE(buffer_size % block_size == 0, "buffer_size does not alligne with block_size");

	m_blockSize = block_size;
	m_alignment = alignment;
	m_blocksPerChunk = buffer_size / block_size;
	m_chunkSize = buffer_size;
	m_head = m_tail = static_cast<byte*>(buffer);
	
	return true;
}

void BlockAllocator::DeInit()
{
	std::scoped_lock<std::mutex> lock(m_blockLock);
	
	// assume for now we only have the one chunk
	if(m_firstChunk != nullptr)
	{
		if(m_base != nullptr)
		{
			chunk_t* current_chunk = reinterpret_cast<chunk_t*>(m_firstChunk);

			//for each chunk
			do
			{
				chunk_t* next_chunk = reinterpret_cast<chunk_t*>(current_chunk->m_next);
				m_base->Free(static_cast<void*>(current_chunk));
				current_chunk = next_chunk;
			} while(current_chunk != nullptr);
		}
	}
	
	m_firstChunk = nullptr;
	m_alignment = 8;
	m_blockSize = 0;
	m_chunkSize = 0;	
}

void* BlockAllocator::Alloc(size_t byte_size)
{
	std::scoped_lock<std::mutex> lock(m_blockLock);
	
	// start off with a linear allocator
	ASSERT_OR_DIE(byte_size == m_blockSize, "Allocation size must be equal to chunk size");

	if(m_head == m_tail && m_base != nullptr) // this block is full
	{
		AllocateChunk();
	}
	
	block_t* block_to_use = reinterpret_cast<block_t*>(m_head);
	m_head = block_to_use->m_next;
	return block_to_use;
}

void BlockAllocator::Free(void* ptr)
{
	std::scoped_lock<std::mutex> lock(m_blockLock);
	
	// start off with a linear allocator
	byte* block_loc = static_cast<byte*>(ptr);
	ASSERT_OR_DIE(IsPointerInAlignment(block_loc), "ptr is not in allignment with BlockAllocator");

	block_t* tail_block = reinterpret_cast<block_t*>(m_tail);
	tail_block->m_next = block_loc;
	m_tail = block_loc;
}

void* BlockAllocator::AllocBlock()
{
	// start off with a linear allocator
	return Alloc(m_blockSize);
}

void BlockAllocator::FreeBlock(void* ptr)
{
	Free(ptr);
}


bool BlockAllocator::IsPointerInAlignment(byte* ptr)
{
	chunk_t* first_chunk = reinterpret_cast<chunk_t*>(m_firstChunk);
	chunk_t* next_chunk = first_chunk;

	//for each chunk
	while(next_chunk != nullptr)
	{
 		byte* chunk_location = reinterpret_cast<byte*>(next_chunk) + sizeof(chunk_t);

		//is the pointer inside the chunk
		size_t offset = ptr - chunk_location;
		if(offset < m_chunkSize)
		{
			//is the ptr alligned with the sliced blocks
			size_t remainder = offset % m_blockSize;
			if(remainder == 0) return true;
			else return false;
		}
		
		next_chunk = reinterpret_cast<chunk_t*>(next_chunk->m_next);
	}

	return false;
}

bool BlockAllocator::AllocateChunk()
{
	//determine if we are allocating a chunk for the first time or already have chunks
	byte* free_chunk = static_cast<byte*>(m_base->Alloc(m_chunkSize));
	chunk_t* chunk_sign = reinterpret_cast<chunk_t*>(free_chunk);
	chunk_sign->m_next = nullptr;
	
	if(m_firstChunk) // we have a chunk
	{
		chunk_t* first_chunk = reinterpret_cast<chunk_t*>(m_firstChunk);
		chunk_t* current_chunk = first_chunk;
		
		while(current_chunk->m_next != nullptr)
		{
			current_chunk = reinterpret_cast<chunk_t*>(current_chunk->m_next);
		}
		
		//continue
		current_chunk->m_next = free_chunk;

		block_t* tail_block = reinterpret_cast<block_t*>(m_tail);
		tail_block->m_next = free_chunk + sizeof(chunk_t);
	}
	else // first time allocation
	{
		//init the entire block of untracked or tracked memory
		m_firstChunk = free_chunk;
		m_head = free_chunk + sizeof(chunk_t);
	}

	byte* root = free_chunk + sizeof(chunk_t);
	//with out new chunk, slice it up into blocks
	for(unsigned int chunk_idx = 0; chunk_idx < m_blocksPerChunk; ++chunk_idx)
	{
		//create the sign
		byte* free_block = root + chunk_idx * m_blockSize;
		block_t* block_sign = reinterpret_cast<block_t*>(free_block);

		//sign points to the next block
		if(chunk_idx < m_blocksPerChunk - 1)
		{
			block_sign->m_next = root + (chunk_idx + 1) * m_blockSize;
		}
		else
		{
			block_sign->m_next = nullptr;
		}

		m_tail = free_block;
	}
	
	return true;
}
