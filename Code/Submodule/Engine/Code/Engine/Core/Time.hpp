//-----------------------------------------------------------------------------------------------
// Time.hpp
//
#pragma once
#include <cstdint>


//-----------------------------------------------------------------------------------------------
double GetCurrentTimeSeconds();
float GetCurrentTimeSecondsF();
uint64_t GetCurrentHPC();
double GetSecondsPerHPC();
double HPCToSeconds( uint64_t hpc ) ;
