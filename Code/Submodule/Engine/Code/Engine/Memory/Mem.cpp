#include "Engine/Memory/Mem.hpp"
#include "Engine/EngineCommon.hpp"
#include "Engine/Core/Callstack.hpp"
#include "Engine/Core/CustomAllocator.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include <malloc.h>
#include <atomic>
#include <mutex>
#include <map>
#include <algorithm>

std::atomic<unsigned int> g_AllocCount = 0; 
std::atomic<unsigned int> g_AllocByte = 0; 

struct MemTrackInfo
{
	void* original_pointer; 
	size_t byte_size; 
	Callstack callstack; 
};

struct MemGrouping
{
	size_t byte_size;
	unsigned int count;
	void* original_pointer; 

	MemGrouping(): byte_size(0), count(0), original_pointer(nullptr) {} ;
	MemGrouping(size_t size, unsigned int num, void* pointer) :
	byte_size(size), count(num), original_pointer(pointer) {};
};

struct CallstackComparator
{
	bool operator()(Callstack const& a, Callstack const& b) const
	{
		return a.m_hash < b.m_hash;
	}
};

static std::mutex& GetMemTrackerLock()
{
	static std::mutex g_TrackerLock;
	return g_TrackerLock;
}

static std::map<	void*,									// key
					MemTrackInfo,							// value
					std::less< void* >,						// less to use for map
					CustomAllocator< std::pair<	void* const, MemTrackInfo >>		// allocator  
				> & GetMemTrackers()
{
	static std::map<	void*,									// key
						MemTrackInfo,							// value
						std::less< void* >,						// less to use for map
						CustomAllocator< std::pair<	void* const, MemTrackInfo >>		// allocator  
					> g_memTrackers;
	return g_memTrackers;
}

//------------------------------------------------------------------------
// parse the size to a readable form
std::string GetSizeString(const size_t byte_count)
{
	const unsigned int bytes = static_cast<unsigned int>(byte_count);
	std::string return_string = "";
	if (bytes < 1024)
	{
		return_string += Stringf("%u B", bytes);
	}
	else if (bytes > 1024 && bytes < 1024 * 1024)
	{
		return_string += Stringf("%.3f KiB", static_cast<float>(bytes) / 1024);
	}
	else if (bytes > 1024 * 1024 && bytes < 1024 * 1024 * 1024)
	{
		return_string += Stringf("%.3f MiB", static_cast<float>(bytes) / (1024 * 1024));
	}
	else if (bytes > (1024 * 1024 * 1024))
	{
		return_string += Stringf("%.3f GiB", static_cast<float>(bytes) / (1024 * 1024 * 1024));
	}

	return return_string;
}

//------------------------------------------------------------------------
// custom new, not tracked
void* UntrackedAlloc(const size_t byte_count) 
{
	return ::malloc( byte_count ); 
}

//------------------------------------------------------------------------
// custom delete, not tracked
void UntrackedFree(void* ptr)
{
	return ::free( ptr ); 
}

//------------------------------------------------------------------------
// custom new, tracked
void* TrackedAlloc(size_t byte_count)
{
	// One suggestion and example on how to break up this function
	// based on build config; 
	#if !defined(MEM_TRACKING)
	{
		return UntrackedAlloc(byte_count);
	}
	#else
	{
		void* ptr = UntrackedAlloc(byte_count);
		
		#if MEM_TRACKING >= MEM_TRACK_ALLOC_COUNT
		{
			// track allocation count
			++g_AllocCount;
		}
		#endif
		
		#if MEM_TRACKING >= MEM_TRACK_VERBOSE
		{
			// track allocation count, byte count, and callstack
			MemTrackInfo info;
			
			// get callstack
			Callstack cs;
			info.callstack = cs;
			
			// save size
			info.byte_size = byte_count;

			// save original pointer
			info.original_pointer = ptr;
			
			// put this into map
			GetMemTrackerLock().lock(); 
			GetMemTrackers()[ptr] = info; 
			GetMemTrackerLock().unlock(); 

			g_AllocByte += static_cast<unsigned int>(byte_count);
		}
		#endif
		// return memory pointer to use
		return ptr;
	}
	#endif
}

//------------------------------------------------------------------------
// custom delete, tracked
void TrackedFree(void* ptr)
{
	// One suggestion and example on how to break up this function
	// based on build config; 
	#if !defined(MEM_TRACKING)
	{
		UntrackedFree(ptr);
	}
	#else
	{
		#if MEM_TRACKING >= MEM_TRACK_ALLOC_COUNT
		{
			// track allocation count
			--g_AllocCount;
		}
		#endif

		#if MEM_TRACKING >= MEM_TRACK_VERBOSE
		{
			GetMemTrackerLock().lock(); 
			g_AllocByte -= static_cast<unsigned int>(GetMemTrackers()[ptr].byte_size);
			GetMemTrackers().erase(ptr); 
			GetMemTrackerLock().unlock();
		}
		#endif

		UntrackedFree(ptr);

	}
	#endif
}

//------------------------------------------------------------------------
// get the total ref count of pointers in the list
size_t MemTrackGetLiveAllocationCount()
{
	return static_cast<size_t>(g_AllocCount);
}

//------------------------------------------------------------------------
// get the total size of each pointer in the list
size_t MemTrackGetLiveByteCount()
{
	return static_cast<size_t>(g_AllocByte);
}

//------------------------------------------------------------------------
// print the proper log information to the dev console
void MemTrackLogLiveAllocations()
{
	DebuggerPrintf("-- Mem Allocation: alloc count -------- \n");

	#if MEM_TRACKING == MEM_TRACK_ALLOC_COUNT

	DebuggerPrintf("Allocation Count: %u \n", MemTrackGetLiveAllocationCount());

	#elif MEM_TRACKING == MEM_TRACK_VERBOSE
	
	DebuggerPrintf("Allocation Count: %u, Allocated byte: %s \n", 
		MemTrackGetLiveAllocationCount(), 
		GetSizeString(MemTrackGetLiveByteCount()).c_str());

	std::map<	Callstack,								// key
				MemGrouping,							// value
				CallstackComparator,		// less to use for map
				CustomAllocator< std::pair<	Callstack const,
												MemGrouping >>		// allocator  
												> hashmap_container;
	g_TrackerLock.lock(); 
	for (auto mem_track_info : g_MemTrackers) {
		auto hash_to_compare = hashmap_container.find(mem_track_info.second.callstack);

		if(hash_to_compare == hashmap_container.end())
		{
			//add a new element into hashmap_container
			hashmap_container[mem_track_info.second.callstack] = 
				MemGrouping(mem_track_info.second.byte_size, 1, mem_track_info.first);
		}
		else
		{
			//update element
			hashmap_container[mem_track_info.second.callstack].byte_size += mem_track_info.second.byte_size;
			++hashmap_container[mem_track_info.second.callstack].byte_size;
		}
	}
	g_TrackerLock.unlock(); 

	std::vector<MemGrouping> sorted_mem;

	for(auto grouped_mem : hashmap_container)
	{
		sorted_mem.push_back(grouped_mem.second);
	}

	std::sort(sorted_mem.begin(), sorted_mem.end(), 
	[](MemGrouping const& mg1, MemGrouping const& mg2) -> bool 
	{ return mg1.byte_size > mg2.byte_size; });

	for(unsigned int mg_idx = 0; mg_idx < 10; ++mg_idx)
	{
		DebuggerPrintf("%s bites from %u allocations \n", 
			GetSizeString(sorted_mem[mg_idx].byte_size).c_str(), 
			sorted_mem[mg_idx].count);

		std::vector<std::string> callstack_output;
		callstack_output = ToStringCollection(g_MemTrackers[sorted_mem[mg_idx].original_pointer].callstack);
		DebugOutputPrintCollection(callstack_output);
	}
	
	#else
	DebuggerPrintf("-- Mem Allocation: no track -------- \n");
	#endif
	DebuggerPrintf("--------------------------------------- \n");

}

//------------------------------------------------------------------------
// overload new and delete to use tracked allocations
void* operator new( size_t size )
{
	return TrackedAlloc(size); 
}

//------------------------------------------------------------------------
void operator delete( void* ptr )
{
	TrackedFree(ptr); 
}

//------------------------------------------------------------------------
void DevConPrintMemTrackType()
{
	#if MEM_TRACKING == MEM_TRACK_ALLOC_COUNT
	
		g_theDevConsole->PrintString(Rgba::CYAN, "Tracked memory");
	
	#elif MEM_TRACKING == MEM_TRACK_VERBOSE
	
		g_theDevConsole->PrintString(Rgba::MAGENTA, "Verbos tracked memory");
	
	#else
	
		g_theDevConsole->PrintString(Rgba::MAGENTA, "Untracked memory");
	
	#endif
}

//------------------------------------------------------------------------
void DevConPrintMemTrack()
{
	g_theDevConsole->PrintString(Rgba::CYAN, "-- Mem Allocation: alloc count --------");
	std::string line;
#if MEM_TRACKING == MEM_TRACK_ALLOC_COUNT

	line = Stringf("Allocation Count: %u \n", MemTrackGetLiveAllocationCount());

#elif MEM_TRACKING == MEM_TRACK_VERBOSE

	line = 	Stringf("Allocation Count: %u, Allocated byte: %s", 
		MemTrackGetLiveAllocationCount(), 
		GetSizeString(MemTrackGetLiveByteCount()).c_str());
	
#else


#endif
	
	g_theDevConsole->PrintString(Rgba::YELLOW, line);
	g_theDevConsole->PrintString(Rgba::CYAN, "---------------------------------------");

}
