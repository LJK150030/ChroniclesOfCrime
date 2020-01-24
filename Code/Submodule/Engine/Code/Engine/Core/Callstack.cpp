#include "Engine/Core/Callstack.hpp"
#include "Engine/Core/StringUtils.hpp"

#define WIN32_LEAN_AND_MEAN		// Always #define this before #including <windows.h>
#include <Windows.h>			// #include this (massive, platform-specific) header in very few places

Callstack::Callstack( const unsigned int skip_frames )
{
	m_depth = 0;
	CaptureStackBackTrace(static_cast<ULONG>(skip_frames), 
		static_cast<ULONG>(MAX_TRACE), 
		m_trace, 
		reinterpret_cast<PDWORD>(&m_hash));
}

Callstack::~Callstack() = default;

	