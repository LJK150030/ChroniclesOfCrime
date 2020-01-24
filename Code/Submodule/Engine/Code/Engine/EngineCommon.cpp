#include "Engine/EngineCommon.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/Log.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/ImGUISystem.hpp"
#include "Engine/Memory/Allocator.hpp"
#include "Engine/Tools/Profiler.hpp"
#include "Engine/Async/Dispatcher.hpp"

#define WIN32_LEAN_AND_MEAN		// Always #define this before #including <windows.h>
#include <Windows.h>			// #include this (massive, platform-specific) header in very few places

#include <DbgHelp.h>
#pragma comment (lib, "DbgHelp.lib")


NamedStrings g_gameConfigBlackboard;
RandomNumberGenerator g_randomNumberGenerator(15);
WindowContext* g_theWindow = nullptr;
RenderContext* g_theRenderer = nullptr;
AudioSystem* g_theAudio = nullptr;
EventSystem* g_theEventSystem = nullptr;
DevConsole* g_theDevConsole = nullptr;
DebugRender* g_theDebugRenderer = nullptr;
Clock* g_theClock = nullptr;
LogSystem* g_theLogSystem = nullptr;
AsyncRingBuffer* g_messageBuffer = nullptr;
ImGUISystem* g_imGUI = nullptr;
Profiler* g_profiler = nullptr;

int WINDOW_RES_X = 1280;
int WINDOW_RES_Y = 720;

bool DEBUG_RENDER_IN_USE = false;
bool DEBUG_RENDER_LOG_CLEAR = false;

bool DEV_CONSOLE_IN_USE = false;

UnitTest g_allTests[MAX_TESTS];
unsigned int g_testCount = 0;

static bool DevConLog(EventArgs& args);
static bool DevConLogEnableAll(EventArgs& args);
static bool DevConLogDisableAll(EventArgs& args);
static bool DevConLogEnable(EventArgs& args);
static bool DevConLogDisable(EventArgs& args);
static bool LogThreadedTest(EventArgs& args);
static bool DevConsLogFlush(EventArgs& args);

std::string LOG_DIR = "Data/Log/current.txt";
std::string SCREENSHOT_DIR = "Data/Images/Screenshots/";

// Engine
void EngineStartup() 
{
	// mem/track/debug init
	// may need to mkdir from code - std::filesystem yay!
	//LogSystemInit( "Data/Log/current.txt" );
	//Logf("test", "Test Message");
	
	g_profiler = new Profiler(512, 10);

	g_theEventSystem = new EventSystem;
	g_theDevConsole = new DevConsole;
	g_theDebugRenderer = new DebugRender;
	g_theClock = new Clock(Clock::g_theMasterClock);
	g_theAudio = new AudioSystem;

	g_theEventSystem->SubscribeEventCallbackFunction("log", DevConLog);
	g_theEventSystem->SubscribeEventCallbackFunction("logEnbAll", DevConLogEnableAll);
	g_theEventSystem->SubscribeEventCallbackFunction("logDisAll", DevConLogDisableAll);
	g_theEventSystem->SubscribeEventCallbackFunction("logEnb", DevConLogEnable);
	g_theEventSystem->SubscribeEventCallbackFunction("logDis", DevConLogDisable);
	g_theEventSystem->SubscribeEventCallbackFunction("logTest", LogThreadedTest);
	g_theEventSystem->SubscribeEventCallbackFunction("logFlush", DevConsLogFlush);

	g_theEventSystem->Startup();
	g_theRenderer->Startup();
	g_theDebugRenderer->Startup(g_theRenderer);
	g_theDevConsole->Startup(g_theClock);
	
	g_imGUI = new ImGUISystem(g_theRenderer);
	g_imGUI->Startup();

	Dispatcher::Init(7);
}

void EngineShutdown()
{
	delete g_imGUI;
	g_imGUI = nullptr;
	
	// shutdown common
	g_theDebugRenderer->Shutdown();
	g_theDevConsole->ShutDown();
	g_theRenderer->Shutdown();
	g_theEventSystem->Shutdown();

	delete g_theAudio;
	g_theAudio = nullptr;

	delete g_theClock;
	g_theClock = nullptr;

	delete g_theDebugRenderer;
	g_theDebugRenderer = nullptr;

	delete g_theDevConsole;
	g_theDevConsole = nullptr;

	delete g_theEventSystem;
	g_theEventSystem = nullptr;

	//LogSystemShutdown(); 
}

void ParseXmlFileToNamedString(NamedStrings& container, const std::string& file_path)
{
	tinyxml2::XMLDocument xmlDoc;
	xmlDoc.LoadFile(file_path.c_str());

	if (xmlDoc.ErrorID() != tinyxml2::XML_SUCCESS)
	{
		std::string error_message = "\n";

		error_message.append(Stringf(">> ERROR loading XML doc \"%s\"\n", file_path.c_str()));
		error_message.append(Stringf(">> errorID = %i\n", xmlDoc.ErrorID()));
		error_message.append(Stringf(">> errorLineNum = %i\n", xmlDoc.ErrorLineNum()));
		error_message.append(Stringf(">> errorName = \"%s\"\n", xmlDoc.ErrorName()));

		ERROR_AND_DIE(error_message);
	}

	tinyxml2::XMLElement* rootElement = xmlDoc.RootElement();
	if (rootElement)
	{
		container.PopulateFromXmlElementAttributes(*rootElement);
	}
}

void UnitTestsRunAllCategories( unsigned int priority ) 
{
	unsigned int total = 0; 
	unsigned int passed = 0; 
	for (UnitTest test : g_allTests) 
	{
		if (test.m_priority <= priority && test.m_workCallback != nullptr) {
			++total; 
			bool succeeds = test.m_workCallback(); 
			if (!succeeds) {
				DebuggerPrintf( "Test ['%s'] Failed", test.m_name ); 
			} else {
				++passed; 
			}
		}
	}

	//  print success/failure if you fancy
	ASSERT_OR_DIE( total == passed, "Not all unit tests passed!" ); 
}

void UnitTestsRun(char const* category, unsigned priority)
{
	unsigned int total = 0; 
	unsigned int passed = 0;

	for (UnitTest test : g_allTests) 
	{
		if (test.m_priority <= priority && test.m_category == category && test.m_workCallback != nullptr) {
			++total; 
			bool succeeds = test.m_workCallback(); 
			if (!succeeds) {
				DebuggerPrintf( "Test ['%s'] Failed", test.m_name ); 
			} else {
				++passed; 
			}
		}
	}

	//  print success/failure if you fancy
	ASSERT_OR_DIE( total == passed, "Not all unit tests passed!" ); 
}

std::vector<std::string> ToStringCollection(Callstack cs)
{
	std::vector<std::string> cs_strings;

	cs_strings.push_back(Stringf("** CALLSTACK ******************************** \n"));

	HANDLE process = GetCurrentProcess();
	bool is_init = SymInitialize( process, nullptr, TRUE );
	UNUSED(is_init);
	
	for (unsigned int trace_idx = 0; trace_idx < cs.m_depth; ++trace_idx) 
	{
		// Initialize symbol information retrieval structures.
		const int max_name_length = 256;
		DWORD_PTR frame = reinterpret_cast<DWORD_PTR>(cs.m_trace[trace_idx]);

		ULONG64 symbol_buffer[(sizeof(SYMBOL_INFO) + max_name_length * sizeof(wchar_t) +
			sizeof(ULONG64) - 1) / sizeof(ULONG64)];
		memset(symbol_buffer, 0, sizeof(symbol_buffer));

		DWORD64 sym_displacement = 0;
		PSYMBOL_INFO symbol = reinterpret_cast<PSYMBOL_INFO>(&symbol_buffer[0]);
		symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
		symbol->MaxNameLen = max_name_length - 1;
		BOOL has_symbol = SymFromAddr(process, frame, 
			&sym_displacement, symbol);
		UNUSED(has_symbol)

		// Attempt to retrieve line number information.
		DWORD line_displacement = 0;
		IMAGEHLP_LINE64 line = {};
		line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
		BOOL has_line = SymGetLineFromAddr64(process, frame, 
			&line_displacement, &line);
		UNUSED(has_line)
		
			cs_strings.push_back(Stringf("%s(%i): %s \n", line.FileName, line.LineNumber, symbol->Name));
	}

	SymCleanup( process );
	cs_strings.push_back(Stringf("********************************************* \n"));

	return cs_strings;
}

void DebugOutputPrintCollection( std::vector<std::string>& collection )
{
	for( int string_idx = 0; string_idx < collection.size(); string_idx++)
	{
		DebuggerPrintf(collection[string_idx].c_str());
	}
}

void FileStreamCollection( std::vector<std::string>& collection, FILE* output_file  )
{
	for( int string_idx = 0; string_idx < collection.size(); string_idx++)
	{
		fwrite( collection[string_idx].c_str(), 1, 
			collection[string_idx].size(), output_file ); 
	}
}

////////////////////////////////////////////////////////////////
// dev console functions for global objects //

STATIC bool DevConLog(EventArgs& args)
{
	std::string filter = args.GetValue(std::string("tag"), std::string("DevCons"));
	std::string message = args.GetValue(std::string("msg"), std::string("DevCons"));

	Logf(filter.c_str(), message.c_str());
	return false;
}

// log_enable_all
STATIC bool DevConLogEnableAll(EventArgs& args)
{
	UNUSED (args);
	LogEnableAll();
	return false;
}

// log_disable_all
STATIC bool DevConLogDisableAll(EventArgs& args)
{
	UNUSED (args);
	LogDisableAll();
	return false;
}

// log_enable
STATIC bool DevConLogEnable(EventArgs& args)
{
	std::string filter = args.GetValue(std::string("tag"), std::string("DevCons"));
	LogEnable(filter.c_str());
	return false;
}

// log_disable
STATIC bool DevConLogDisable(EventArgs& args)
{
	std::string filter = args.GetValue(std::string("tag"), std::string("DevCons"));
	LogDisable(filter.c_str());
	return false;
}

// log_thread_test
#define LOG_MESSAGES_PER_THREAD_TEST   (1024)
STATIC void LogTest( uint start_count ) 
{
	std::thread::id this_id = std::this_thread::get_id(); 
	size_t hash_id = std::hash<std::thread::id>{}( this_id );
	char const* format = "Thread[%llu]: Printing Message %u";

	for (unsigned int i = 0; i < LOG_MESSAGES_PER_THREAD_TEST; ++i) {
		const float coin_flip = g_randomNumberGenerator.GetRandomFloatZeroToOne();
		if (coin_flip > 0.5f)
		{
			LogCallstackf( "debug", format, hash_id, i ); 
		}
		else 
		{
			Logf( "debug", format, hash_id, i + start_count ); 
		}
	}
}

STATIC bool LogThreadedTest(EventArgs& args)
{
	// leave one thread free (main thread)
	UNUSED(args);
	uint core_count = std::thread::hardware_concurrency() - 1; 
	for (uint i = 0; i < core_count; ++i) {
		std::thread test_thread( LogTest, i * LOG_MESSAGES_PER_THREAD_TEST ); 
		test_thread.detach(); 
	}

	return true;
}

// log_flush_test
// UNITTEST("LogFlushTest", "LoggingSystem", 0)
// {
// 	Logf("PrintFilter", "I am a Logf call");
// 	Logf("FlushFilter", "I am now calling flush");
// 	LogFlush();
// 	return true;
// }

STATIC bool DevConsLogFlush(EventArgs& args)
{
	// leave one thread free (main thread)
	// 	Logf("PrintFilter", "I am a Logf call");
	// 	Logf("FlushFilter", "I am now calling flush");
	LogFlush();
	return true;
}

// log_hook_dev_console
void OutputLog(const LogMessageHeaderT* log_info)
{
	char filter[256];
	char message[256];

	memcpy(filter, log_info->filter, log_info->size_of_filter);
	memcpy(message, log_info->message, log_info->size_of_message);
	
	DebuggerPrintf("%s: %s \n", filter, message);
}