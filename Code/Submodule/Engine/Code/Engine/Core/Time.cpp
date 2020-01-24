//-----------------------------------------------------------------------------------------------
// Time.cpp
//	

//-----------------------------------------------------------------------------------------------
#include "Engine/Core/Time.hpp"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>


//-----------------------------------------------------------------------------------------------
double InitializeTime(LARGE_INTEGER& out_initial_time)
{
	LARGE_INTEGER counts_per_second;
	QueryPerformanceFrequency(&counts_per_second);
	QueryPerformanceCounter(&out_initial_time);
	return (1.0 / static_cast<double>(counts_per_second.QuadPart));
}


//-----------------------------------------------------------------------------------------------
double GetCurrentTimeSeconds()
{
	static LARGE_INTEGER initial_time;
	static double seconds_per_count = InitializeTime(initial_time);
	LARGE_INTEGER current_count;
	QueryPerformanceCounter(&current_count);
	const LONGLONG elapsed_counts_since_initial_time = current_count.QuadPart - initial_time.QuadPart;

	const double current_seconds = static_cast<double>(elapsed_counts_since_initial_time) * seconds_per_count;
	return current_seconds;
}

float GetCurrentTimeSecondsF()
{
	return static_cast<float>(GetCurrentTimeSeconds());
}

uint64_t GetCurrentHPC()
{
	LARGE_INTEGER current_count;
	QueryPerformanceCounter(&current_count);

	return current_count.QuadPart;
}

double GetSecondsPerHPC()
{
	LARGE_INTEGER counts_per_second;
	QueryPerformanceFrequency(&counts_per_second);

	return (1.0 / static_cast<double>(counts_per_second.QuadPart));
}

double HPCToSeconds(const uint64_t hpc)
{
	static double seconds_per_hpc = GetSecondsPerHPC(); 
	return static_cast<double>(hpc) * seconds_per_hpc;
}

