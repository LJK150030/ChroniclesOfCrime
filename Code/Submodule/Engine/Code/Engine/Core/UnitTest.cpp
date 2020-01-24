#include "UnitTest.hpp"
#include "Engine/EngineCommon.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"


UnitTest::UnitTest() = default;

UnitTest::UnitTest( char const * name, char const * category, const unsigned int priority, const test_work_cb cb ):
	m_workCallback(cb), m_name(name), m_category(category), m_priority(priority)
{
	ASSERT_OR_DIE( g_testCount < MAX_TESTS, "Too many UnitTests" ); 

	++g_testCount;
	next = g_allTests; 
	g_allTests[g_testCount] = *this;
}
