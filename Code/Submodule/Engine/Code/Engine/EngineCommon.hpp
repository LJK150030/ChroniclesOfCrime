#pragma once
#pragma warning (error: 4172 ) // returning address of local variable or temporary
#pragma warning (error: 4715 ) // not all control paths return a value

#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Core/UnitTest.hpp"
#include "Engine/Core/Callstack.hpp"
#include "Engine/Core/Log.hpp"
#include <vector>

#define UNUSED(x) (void)(x);

#define _QUOTE(x) # x
#define QUOTE(x) _QUOTE(x)
#define __FILE__LINE__ __FILE__ "(" QUOTE(__LINE__) ") : "

#define PRAGMA(p)  __pragma( p )
#define NOTE( x )  PRAGMA( message(x) )
#define FILE_LINE  NOTE( __FILE__LINE__ )

// the important bits
#define TODO( x )  NOTE( __FILE__LINE__"\n"           \
       " --------------------------------------------------------------------------------------\n" \
       "|  TODO :   " ##x "\n" \
       " --------------------------------------------------------------------------------------\n" )

#define UNIMPLEMENTED()  TODO( "IMPLEMENT: " QUOTE(__FILE__) " (" QUOTE(__LINE__) ")" );

#define BIT_FLAG(b) ((1U) << b)

#define STATIC

#define COMBINE1(X,Y) X##Y  // helper macro
#define COMBINE(X,Y) COMBINE1(X,Y)

#define PROFILE_LOG_SCOPE( tag )  LogProfileScope COMBINE(__scopeLog, __LINE__) ## ( tag )

#define MACRO_COMBINE1(X,Y) X##Y  // helper macro
#define MACRO_COMBINE(X,Y) MACRO_COMBINE1(X,Y)

#define UNITTEST( name, cat, pri )	\
	static bool MACRO_COMBINE(__UnitTest_,__LINE__)(); \
	static UnitTest MACRO_COMBINE(__UnitTestObj_,__LINE__)( name, cat, pri, MACRO_COMBINE(__UnitTest_,__LINE__) ); \
	static bool MACRO_COMBINE(__UnitTest_,__LINE__)()

#define MAX_TESTS 1024
extern UnitTest g_allTests[MAX_TESTS];
extern unsigned int g_testCount; 
typedef unsigned int uint;

struct Vec2;
struct IntVec2;
struct Vec3;
struct Vec4;
struct Matrix44;
struct AABB2;
struct FloatRange;
struct IntRange;
struct Rgba;
struct Vertex_PCU;
struct Matrix44;

class Texture;
class DevConsole;
class RenderContext;
class InputSystem;
class Tags;
class EventSystem;
class WindowContext;
class DebugRender;
class Clock;
class LogSystem;
class AsyncRingBuffer;
class ImGUISystem;
class Profiler;

extern EventSystem* g_theEventSystem;
extern DevConsole* g_theDevConsole;
extern RenderContext* g_theRenderer;
extern InputSystem* g_theInput;
extern WindowContext* g_theWindow;
extern DebugRender* g_theDebugRenderer;
extern Clock* g_theClock;
extern LogSystem* g_theLogSystem;
extern AsyncRingBuffer* g_messageBuffer;
extern ImGUISystem* g_imGUI;
extern Profiler* g_profiler;


extern RandomNumberGenerator g_randomNumberGenerator;
extern NamedStrings g_gameConfigBlackboard; // declared in EngineCommon.hpp, defined in EngineCommon.cpp
// 
//Global Rgba variables
constexpr float WEB_COLOR_STRIDE = 0.0039215686274509803921568627451f;
constexpr float F_SQRT_3_OVER_3 = 0.577350269189625764509148780501957455647601751270126876018f;
constexpr float SQRT_3_OVER_2 = 0.866025403784438646763723170752936183471402626905190314027f;

//Global math variables
constexpr float EPSILON = 1.0e-6f;
constexpr float PI = 3.1415926535897932384626433832795f;
constexpr float HALF_PI = 1.5707963267948966192313216916398f;
constexpr float TWO_PI = 2.0f*PI;
constexpr float NONZERO_ZERO = 1.175494351e-38F;

//Global preferences
constexpr bool FLIP_TEXTURE_VERTICALLY = false;
extern int WINDOW_RES_X;
extern int WINDOW_RES_Y;

//Global debug variables
extern bool DEBUG_RENDER_IN_USE;
extern bool DEBUG_RENDER_LOG_CLEAR;
extern bool DEV_CONSOLE_IN_USE;

//Global file dir
extern std::string LOG_DIR;
extern std::string SCREENSHOT_DIR;


//Global helper functions
void ParseXmlFileToNamedString(NamedStrings& container, const std::string& file_path);
void UnitTestsRunAllCategories( unsigned int priority  = UINT_MAX );
void UnitTestsRun( char const* category, unsigned int priority = UINT_MAX );
std::vector<std::string> ToStringCollection( Callstack cs );
void DebugOutputPrintCollection( std::vector<std::string>& collection ) ;
void FileStreamCollection( std::vector<std::string>& collection, FILE* output_file  ) ;
void EngineStartup();
void EngineShutdown();

extern void OutputLog(const LogMessageHeaderT* log_info);

enum JobCategory
{
	JOB_UNKNOWN = -1,
	JOB_GENERAL,
	JOB_MAIN,
	JOB_RENDER,
	JOB_PHYSICS,

	NUM_JOB_CATEGORIES
};