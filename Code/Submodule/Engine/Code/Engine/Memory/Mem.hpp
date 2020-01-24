#pragma once

#define MEM_TRACK_ALLOC_COUNT 	0
#define MEM_TRACK_VERBOSE		0

// Have MEM_TRACKING undefined means no tracking happens
#if defined(_DEBUG)
#define MEM_TRACKING MEM_TRACK_VERBOSE
#elif defined(_RELEASE)
#define MEM_TRACKING MEM_TRACK_ALLOC_COUNT
#endif

#include <string>
#include "Engine/EngineCommon.hpp"

#include <string>
#include <atomic>
#include <thread>
#include <chrono>
#include "Engine/Async/AsyncQueue.hpp"
#include <wtypes.h>

// take a byte count and conver to a more human-parseable
// string 
// ex...
// 123 B would return "123 B"
// 1234 B would return "1.234 KB"
// 123456 B would return "123.456 KB"
// 12345678 B would return "12.346 MB"
std::string GetSizeString( size_t byte_count ); 

// allocations that mem tracking will ignore
// thin wrapper around ::malloc and ::free
void* UntrackedAlloc( size_t byte_count ); 
void UntrackedFree( void* ptr ); 

// allocations will be tracked (if the config is set)
// use this instead of "malloc" and "free" 
void* TrackedAlloc( size_t byte_count ); 
void TrackedFree( void* ptr ); 

// tracking - work is conditional based on config
void TrackAllocation( void* allocation, size_t byte_count ); 
void UntrackAllocation( void* allocation ); 

// report methods
size_t MemTrackGetLiveAllocationCount(); 
size_t MemTrackGetLiveByteCount(); 
void MemTrackLogLiveAllocations();

void DevConPrintMemTrackType();
void DevConPrintMemTrack();

#if defined(MEM_TRACKING)
#define MEMTEST_ITER_PER_THREAD 10'000
#define MEMTEST_ALLOC_BYTE_SIZE 128

static void AllocTest(AsyncQueue<void*>& mem_queue, std::atomic<unsigned int>& running_count)
{
	for (unsigned int i = 0; i < MEMTEST_ITER_PER_THREAD; ++i) 
	{
		const float coin_flip = g_randomNumberGenerator.GetRandomFloatZeroToOne();
		if (coin_flip > 0.5f)
		{
			byte* ptr = (byte*)TrackedAlloc(MEMTEST_ALLOC_BYTE_SIZE);

			for (unsigned int j = 0; j < MEMTEST_ALLOC_BYTE_SIZE; ++j) 
			{
				ptr[j] = (byte)j;
			}

			mem_queue.Enqueue(ptr);
		}
		else 
		{
			void* ptr;
			if (mem_queue.Dequeue(&ptr)) 
			{
				TrackedFree(ptr);
			}
		}
	}

	--running_count; 
}

UNITTEST("A02", "mem", 0)
{
	size_t pre_allocations = MemTrackGetLiveAllocationCount();
	{
		AsyncQueue<void*> mem_queue;
		const unsigned int core_count = std::thread::hardware_concurrency();
		std::atomic<unsigned int> live_count = core_count;

		for (unsigned int i = 0; i < core_count; i++) 
		{
			std::thread test_thread( [&]() { AllocTest( mem_queue, live_count ); } );
			test_thread.detach();
		}

		while (live_count.load() > 0) 
		{
			//std::this_thread::sleep_for(100ms);
			std::chrono::milliseconds(100);
		}

		void* ptr;
		while (mem_queue.Dequeue(&ptr)) 
		{
			TrackedFree(ptr);
		}
	}

	// check we're back to where we started; 
	size_t post_allocations = MemTrackGetLiveAllocationCount();

	// if done right, allocations at the start
	// should be allocations at the end; 
	return (pre_allocations == post_allocations);
}
#endif


