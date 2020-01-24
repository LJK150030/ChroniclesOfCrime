#pragma once
#include "Engine/Memory/BlockAllocator.hpp"

template <typename OBJ_TYPE>
class ObjectAllocator : public BlockAllocator
{
public:
	 virtual ~ObjectAllocator() = default;

	 bool Init( Allocator* base, uint blocks_per_chunk )
	 {
		 return BlockAllocator::Init( base, sizeof(OBJ_TYPE), 
			 alignof(OBJ_TYPE), blocks_per_chunk ); 
	 }

	 void* Alloc( size_t size ) override final { return BlockAllocator::Alloc( size ); }
	 void Free( void* ptr ) override final  { BlockAllocator::Free( ptr ); }
};
