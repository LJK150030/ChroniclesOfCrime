#include "Game/Condition.hpp"


Condition::Condition(Trigger* the_setup): m_trigger(the_setup)
{
	
};


Condition::~Condition()
{
	
}


bool Condition::Test()
{
	return false;
}

//-------------------------------------------------------------------
