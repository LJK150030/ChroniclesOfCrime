#pragma once

typedef bool (*test_work_cb)(); 

class UnitTest
{
public:
	test_work_cb	m_workCallback; 
	char const*		m_name; 
	char const*		m_category; 
	unsigned int	m_priority; 

	UnitTest* next; 

public:
	UnitTest();
	UnitTest( char const* name, char const* category, unsigned int priority, test_work_cb cb );
	
};