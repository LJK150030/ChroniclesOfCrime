#pragma once
#include "Engine/Core/Callstack.hpp"
#include "Engine/Architecture/Semaphore.hpp"
#include "Engine/Architecture/AsyncRingBuffer.hpp"

#include <shared_mutex>
#include <set>

class AsyncRingBuffer;
extern AsyncRingBuffer* g_messageBuffer;
static bool g_loggerSetup = false;

struct LogMessageHeaderT 
{
	char* filter = nullptr;
	size_t size_of_filter = 0;
	char* message = nullptr;
	size_t size_of_message = 0;
	Callstack callstack; // callstack if there was one
	bool print_callstack = false;
};

using LogHookCallback = void(*)(const LogMessageHeaderT*);

static bool IsFiltered( char const* tag );
static void LogThread();
static void ProcessLogMessage( LogMessageHeaderT* msg, FILE* output_file ) ;


class LogSystem
{
public:
	std::thread						m_thread; 
	bool							m_blackListMode = true;
	bool							m_flushRequest = false;
	bool							m_isRunning = true;

	std::vector<LogHookCallback>	m_logHooks;
	std::set<std::string>			m_filtersToIgnore;
	std::string						m_filename;

	
	// other things this need
	// if just async queue
	AsyncRingBuffer		m_messages; 
	Semaphore			m_signal; 
	std::shared_mutex	m_sharedMutex;
	
public:
	void	WaitForWork() 						{ m_signal.Acquire(); }
	void	SignalWork()						{ m_signal.Release(1); }
	bool	HasBeenInit()						{ return g_loggerSetup; }
	void	SetInit()							{ g_loggerSetup = true; }

	bool IsRunning() const 		{ return m_isRunning; }
	void Stop() 				{ m_isRunning = false; }
};


// NOTE: As with ConsolePrintf, you may use variadic templates instead if you prefer
bool LogSystemInit( char const* log_file ); 
void LogSystemShutdown(); // will do a final flush of the log file and properly close donw


// commands exposed; 
// log
// log_enable_all
// log_disable_all
// log_enable
// log_disable
// log_flush_test
// log_thread_test
// log_hook_dev_console 

// Logs a message with the given filter
// TODO: remove nullptr from filter
void Logf( char const* filter = nullptr, char const* format = nullptr, ... ); 

// Logs a message and includes the callstack on the next line(s)
void LogCallstackf( char const* filter, char const* format = nullptr, ... ); 

void LogV( char const* filter = nullptr, bool print_callstack = false, char const* format = nullptr, va_list args = nullptr );
void LogToOutput();

// confirms all messages have been processed (committed to disk)
void LogFlush(); 


// Filtering
void LogEnableAll();  // all messages log
void LogDisableAll(); // not messages log
void LogEnable( char const* filter );     // this filter will start to eappear in the log
void LogDisable( char const* filter );    // this filter will no longer appear in the log


// Listener Hooks
// Allow other systems to get message events (from the logger thread)
// so it can do somethign with them
// Example would be to call DebuggerPrintf - or to enable the dev console to print them; 

// NOTE - this message is only valid for the lifetime of the callback
// and is not guaranteed to valid after your hook returns, so be sure to 
// copy data you need!
void LogHook( LogHookCallback cb ); 
void LogUnhook( LogHookCallback cb ); 
void RunAllHooks( LogMessageHeaderT* msg );