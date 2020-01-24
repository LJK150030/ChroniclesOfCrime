#include "Engine/Core/Log.hpp"
#include "Engine/EngineCommon.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Architecture/AsyncRingBuffer.hpp"
#include <stdio.h>
#include <stdarg.h> 
#include <thread>

//------------------------------------------------------------------------
// filtering is done using a bool and a container of filters
// bool isWhitelist;
// container<string> filters; 
//
// Filters use a "reader-writer lock", also referred to as a "shared_mutex" in C++11+; 

// Enabling or Disabling All changes the mode you're in, effecting the behaviour of
static bool IsFiltered( char const* tag )
{
	// shared lock (not editing)
	std::shared_lock lock(g_theLogSystem->m_sharedMutex);
	
	// depending on mode and if it exists will tell us if we're filter
	auto itr_found = g_theLogSystem->m_filtersToIgnore.find(tag);

	if(itr_found == g_theLogSystem->m_filtersToIgnore.end())
	{
		return !g_theLogSystem->m_blackListMode;
	}
	
	return g_theLogSystem->m_blackListMode;
}

void LogEnableAll()
{
	std::unique_lock lock(g_theLogSystem->m_sharedMutex);
	// Enabling all is setting yourself to a "blacklist" mode.
	// meaning filters you're tracking are targs you want to ignore
	g_theLogSystem->m_blackListMode = true;
	g_theLogSystem->m_filtersToIgnore.clear();
	
}

void LogDisableAll()
{
	std::unique_lock lock(g_theLogSystem->m_sharedMutex);
	// Disable All sets yourself to a "whitelist" mode, meaning your 
	// tracked filters are now the tags you allow through; 
	g_theLogSystem->m_blackListMode = false;
	g_theLogSystem->m_filtersToIgnore.clear();

}

// LogEnable & LogDisable behaviour depends on which mode we're currently in...

void LogEnable( char const* filter )
{
	// shared lock (not editing)
	std::unique_lock lock(g_theLogSystem->m_sharedMutex);

	for(std::set<std::string>::iterator filter_it=g_theLogSystem->m_filtersToIgnore.begin(); 
		filter_it != g_theLogSystem->m_filtersToIgnore.end(); 
		++filter_it)
	{
		if(strcmp(filter_it->c_str(), filter) == 0)
		{
			g_theLogSystem->m_filtersToIgnore.erase(filter);
			return;
		}
	}

	//could not find the filter
	g_theLogSystem->m_filtersToIgnore.insert(filter);
}

void LogDisable( char const* filter )
{
	// shared lock (not editing)
	std::unique_lock lock(g_theLogSystem->m_sharedMutex);

	for(std::set<std::string>::iterator filter_it=g_theLogSystem->m_filtersToIgnore.begin(); 
		filter_it != g_theLogSystem->m_filtersToIgnore.end(); 
		++filter_it)
	{
		if(!g_theLogSystem->m_blackListMode)
		{
			if(strcmp(filter_it->c_str(), filter) == 0)
			{
				g_theLogSystem->m_filtersToIgnore.erase(filter);
				return;
			}
		}
	}

	//could not find the filter
	if(g_theLogSystem->m_blackListMode)
		g_theLogSystem->m_filtersToIgnore.insert(filter);
}

void LogHook( LogHookCallback cb )
{
	g_theLogSystem->m_logHooks.push_back(cb);
}

void LogUnhook(LogHookCallback cb)
{
	std::vector<LogHookCallback>::iterator hook_itr =
		std::find(g_theLogSystem->m_logHooks.begin(), g_theLogSystem->m_logHooks.end(), cb);

	if(hook_itr == g_theLogSystem->m_logHooks.end())
	{
		return;
	}
	
	g_theLogSystem->m_logHooks.erase(hook_itr);
}

void RunAllHooks(LogMessageHeaderT* msg)
{
	for(std::vector<LogHookCallback>::iterator hook_itr=g_theLogSystem->m_logHooks.begin(); 
		hook_itr != g_theLogSystem->m_logHooks.end(); 
		++hook_itr)
	{
		//Dereference the itr and then pass arguments to the function pointer
		//Dereferencing itr is what gives us the function pointer
		(*hook_itr)(msg);
	}
}

//------------------------------------------------------------------------
static void LogThread()
{
	// Create log file (logs/log.txt) (write mode)
	// TODO extra: give log file a timestamp
	// I prefer "log.YYYYMMDD_HHmmss.txt"
	// which allows you to sort alphabetically; 
	// ...
	if(g_theLogSystem == nullptr)
	{
		return;
	}
	
	FILE* file = fopen( g_theLogSystem->m_filename.c_str(), "wb" ); 
	ASSERT_OR_DIE(file != nullptr, "Unable to write to file")

	while (g_theLogSystem->IsRunning()) 
	{
		void* msg_ptr = nullptr;
		g_theLogSystem->WaitForWork();
		
		size_t out_size;
		msg_ptr = g_theLogSystem->m_messages.TryLockRead(&out_size);
		
		while ( msg_ptr != nullptr) 
		{
			ProcessLogMessage( (LogMessageHeaderT*) msg_ptr, file );
			RunAllHooks( (LogMessageHeaderT*) msg_ptr);
			g_theLogSystem->m_messages.UnlockRead(msg_ptr);
			msg_ptr = g_theLogSystem->m_messages.TryLockRead(&out_size);
		}
	
		if(g_theLogSystem->m_flushRequest)
		{
			fflush(file);
			g_theLogSystem->m_flushRequest = false;
		}

		
	}
	fclose( file ); 
}

//------------------------------------------------------------------------
static void ProcessLogMessage( LogMessageHeaderT* msg, FILE* output_file ) 
{
	// Order of these two don't matter...

	//TODO: add time stamp
	//DebuggerPrintf("%s: %s \n", msg->filter, msg->message);

	// give every hook a shot at it...
	// ...

	// write to the log file...
	
	//fwrite( msg->filter, 1, msg->size_of_filter - 1, output_file ); 
	//fwrite( ": ", 1, 2, output_file ); 
	fwrite( msg->message, 1, msg->size_of_message - 1, output_file ); 
	fwrite( "\n", 1, 1, output_file );

	if(msg->print_callstack)
	{
		std::vector<std::string> callstack_output;
		callstack_output = ToStringCollection(msg->callstack);
		FileStreamCollection(callstack_output, output_file);
	}
}

//------------------------------------------------------------------------
bool LogSystemInit( char const* log_file )
{
	// TODO extra:  Find log files in log_file folder, and delete anything but the LOG_HISTORY_COUNT newest
	// TODO this structure is a singleton and should have functions that represent this.
	ASSERT_OR_DIE(!g_loggerSetup, "Attempting to init a second log system")
	
	g_theLogSystem = new LogSystem(); 
	g_theLogSystem->m_filename = log_file;
	g_theLogSystem->m_signal.Create( 0, 1 ); 
	g_theLogSystem->m_filtersToIgnore = std::set<std::string>();
	g_theLogSystem->m_logHooks = std::vector<LogHookCallback>();
	g_theLogSystem->SetInit();
	
	// setup the ring buffer the system will use; 
	bool buff_init = g_theLogSystem->m_messages.Init( 8'192);
	
	// Start log thread;
	g_theLogSystem->m_thread = std::thread( LogThread );
	g_theLogSystem->m_isRunning = true;

	return g_theLogSystem != nullptr && buff_init;
}

//------------------------------------------------------------------------
void LogSystemShutdown()
{
	if (g_theLogSystem == nullptr) 
	{
		return; 
	}
	
	g_theLogSystem->Stop(); 

	Logf( "test", "Shutting down log system..." ); 
	g_theLogSystem->SignalWork(); 

	g_theLogSystem->m_thread.join();
	
	// potentially other cleanup; 
	delete g_theLogSystem;
}

//------------------------------------------------------------------------
void Logf( char const* filter, char const* format, ... )
{
	// do not work if the message is currently filtered; 
	// you could alternatively do this on the thread if you're
	// allowing for seperate filters for console/files/etc...
	bool is_filtered = IsFiltered(filter);
	if (is_filtered || g_theLogSystem == nullptr) 
	{
		return; 
	}

	va_list args;
	va_start (args, format);
	LogV(filter, false, format, args);
	va_end (args);
	g_theLogSystem->SignalWork();
	
}

void LogCallstackf(char const* filter, char const* format, ...)
{
	// do not work if the message is currently filtered; 
	// you could alternatively do this on the thread if you're
	// allowing for seperate filters for console/files/etc...
	if (IsFiltered(filter) || g_theLogSystem == nullptr) 
	{
		return; 
	}
	
	va_list args;
	va_start (args, format);
	LogV(filter, true, format, args);
	va_end (args);
	g_theLogSystem->SignalWork(); 
}

void LogV(char const* filter, bool print_callstack, char const* format, va_list args)
{

	// lock enough space to write log information; 
	// look into `vsnprintf` for determining buffer size needs
	// for the format/arguments...
	int size_of_filter = static_cast<int>(strlen(filter)) + 1;
	int size_of_message = vsnprintf (nullptr, 0,format, args) + 1;

	// need space for the log_message_t, the format and filter; 
	// for callstack version - also enough room for a callstack
	size_t bytes_log_header = sizeof(LogMessageHeaderT);
	size_t bytes_filter = size_of_filter * sizeof(char);
	size_t bytes_message = size_of_message * sizeof(char);
	size_t bytes_needed = bytes_log_header + bytes_filter + bytes_message;

	// allocate it using a thread-safe ring-buffer
	byte* buf = (byte*) g_theLogSystem->m_messages.LockWrite( bytes_needed );

	// sub allocate and finalize the message
	LogMessageHeaderT header;
	header.filter = (char*) memcpy(	buf + bytes_log_header, 
												filter, 
												bytes_filter);
	header.size_of_filter = bytes_filter;

	char full_message[512];
	vsnprintf (full_message, (size_t) size_of_message, format, args);

	header.message = (char*) memcpy(	buf + bytes_log_header + bytes_filter, 
												full_message, 
												bytes_message);

	header.size_of_message = bytes_message;

	if(print_callstack)
	{
		header.print_callstack = true;
	}
	
	memcpy(buf, &header, bytes_log_header);
	
	// mark us as finished writing - so worker thread can pick it up
	g_theLogSystem->m_messages.UnlockWrite( buf );
}

//------------------------------------------------------------------------
void LogFlush()
{
	// request a flush
	g_theLogSystem->m_flushRequest = true; 
	g_theLogSystem->SignalWork();
	// signal smeaphore and wait until the flush has been serviced
	// Be sure to flush the file operations using fflush or similar
	// on the thread... once done, mark the flush as finished
	// ...

	// wait until the thread marks this back to false when it is done
	// (not perfect, but good enough for a debug tool)
	while (g_theLogSystem->m_flushRequest) {
		std::this_thread::yield(); 
	}

	// we can get a lot fancier here if you want practice
	// One option is to pass a local bool as a message to the log thread, 
	// allowing each flush request to be serviced explicitly...

	// you can also pass or use a semaphore for signaling it is finished so we can wait on it.  
	// But again, for a debug tool, just spinning will be fine...
}