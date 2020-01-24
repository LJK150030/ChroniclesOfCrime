#include "Engine/Memory/Allocator.hpp"

STATIC TrackedAllocator TrackedAllocator::s_instance;
STATIC UntrackedAllocator UntrackedAllocator::s_instance;

void* TrackedAllocator::Alloc(size_t size)
{
	return TrackedAlloc( size ); 
}

void TrackedAllocator::Free(void* ptr)
{	
	TrackedFree( ptr ); 
}

void* UntrackedAllocator::Alloc(size_t size)
{
	return UntrackedAlloc( size ); 
}

void UntrackedAllocator::Free(void* ptr)
{	
	UntrackedFree( ptr ); 
}


