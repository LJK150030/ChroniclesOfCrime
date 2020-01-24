#pragma once
#include "Mem.hpp"

// Different systems support allocators different;
// - C++ std uses static classes and basically singleton allocators
// - Some use a simple struct with function pointers
//   - Very common in 3rd party tools
// - Previous companies I've worked use interfaces
//   - This allows using the same pattern with different blocks of memeory
//   - Lets us give systems memory blocks/pools, and letting the system decide how to divvy them up; 
class Allocator 
{
public:
	virtual ~Allocator() = default;
	
	// get memory
	virtual void* Alloc( size_t size )     = 0; 

	// free memory; 
	virtual void Free( void* ptr )         = 0; 

	// virtual void* realloc( void* ptr, size_t size ) = 0; // if you want, though most allocator types we'll write are unable to do this; 

// 	// some helpers I tend to like to have as convenience
// 	template <typename T, typename ...ARGS>
// 	T* Create( ARGS&& ...args )       
// 	{
// 		// for a allocator meant for modern C++, the below is a bit simplistic
// 		// and you may want to "perfect-foward"
// 		// see: http://cpptruths.blogspot.com/2012/06/perfect-forwarding-of-parameter-groups.html
// 
// 		void* mem = this->Alloc( sizeof(T) ); 
// 		if (mem != nullptr) {
// 			// new in place
// 			new (mem) T( args... ); 
// 		}
// 
// 		return (T*) mem; 
// 	}
// 
// 	template <typename T>
// 	void Destroy( T* obj ) 
// 	{
// 		if (obj != nullptr) {
// 			obj->~T(); 
// 			this->Free( obj ); 
// 		}
// 	}

}; 

// note, you do not pay the cost of "virtual" if the object is known not to be inherited from (using an object by value, or the type it is
// at is 'final'.  So good use case for that here; 

// news that take an allocator to use; 
inline void* operator new( size_t size, Allocator& pool ) { return pool.Alloc(size); }
inline void operator delete( void* ptr, Allocator& pool ) { pool.Free(ptr); }
//
// new (my_allocator) SomeObject();
// delete (my_allocator) SomeObject(); // error
// my_allocator.destroy( obj ); // non-error

// To make an allocator for our tracked memory, it is as easy as this; 
class TrackedAllocator : public Allocator 
{
public:
	static TrackedAllocator s_instance; 

public:
	// Singleton with Double-Checked Locking for thread safety
	virtual ~TrackedAllocator() = default;
	
	void*				Alloc( size_t size ) final;
	void				Free( void* ptr ) final;
	
	// Also, TrackedAlloator is the "default" allocator of the sytem;	
};


// similar for UntrackedAllocator 
class UntrackedAllocator : public Allocator 
{
public:
	static UntrackedAllocator s_instance; 

public:
	virtual ~UntrackedAllocator() = default;

	void* Alloc( size_t size ) final;
	void Free( void* ptr ) final;

};


