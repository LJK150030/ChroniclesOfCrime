#pragma once
#include "Engine/EngineCommon.hpp"

template <typename T>
struct CustomAllocator 
{
	CustomAllocator() = default; 

	template <class U>
	explicit constexpr CustomAllocator( CustomAllocator<U> const& ) noexcept {}

	// allocator needs to define these types;
	// the "type" is not as important as the name
	// the stdlib is expecting these names - remember templates are a form of duck-typing
	// these three are fairly standard
	typedef T               value_type; 
	typedef size_t          size_type; 
	typedef std::ptrdiff_t  difference_type; 

	// give hints on how the allocator is implemented so that containers can optmize around it 
	typedef std::true_type  propagate_on_container_move_assignment;   // when moving - does the allocator local state move with it?
	typedef std::true_type  is_always_equal;                          // can optimize some containers (allocator of this type is always equal to others of its type)                         

																	  // 
	T* allocate( size_t byte_size )              { UNUSED(byte_size) return (T*) ::malloc( sizeof(T)  ); }
	void deallocate( T* ptr, size_t byte_size )  { UNUSED(byte_size) ::free(ptr); }
};

template<typename T, class U>
bool operator==( CustomAllocator<T> const&, CustomAllocator<U> const& ) { return true; }

template<typename T, class U>
bool operator!=( CustomAllocator<T> const&, CustomAllocator<U> const& ) { return false; }