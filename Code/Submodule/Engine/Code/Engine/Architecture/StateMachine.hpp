#pragma once
#include "Engine/Architecture/State.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

template <class T>
class StateMachine
{
public:

	explicit	StateMachine(T* owner);
	virtual		~StateMachine();
	void		SetCurrentState(State<T>* state);
	void		SetAnytimeState(State<T>* state);
	void		SetPreviousState(State<T>* state);
	void		Update()const;
	void		ChangeState(State<T>* new_state);
	void		RevertToPreviousState();
	bool		IsInState(const State<T>& state)const;
	State<T>*	CurrentState() const;
	State<T>*	GlobalState()   const;
	State<T>*	PreviousState() const;

private:
	T*          m_owner;
	State<T>*   m_currentState; 
	State<T>*   m_previousState;
	State<T>*   m_anytimeState;
};

template <class T>
StateMachine<T>::StateMachine(T* owner):m_owner(owner), m_currentState(nullptr),	
	m_previousState(nullptr), m_anytimeState(nullptr)
{
}

template <class T>
StateMachine<T>::~StateMachine(){}

template <class T>
void StateMachine<T>::SetCurrentState(State<T>* state)
{
	m_currentState = state;
}

template <class T>
void StateMachine<T>::SetAnytimeState(State<T>* state)
{
	m_anytimeState = state;
}

template <class T>
void StateMachine<T>::SetPreviousState(State<T>* state)
{
	m_previousState = state;
}

template <class T>
void StateMachine<T>::Update() const
{
	if(m_anytimeState)   m_anytimeState->Execute(m_owner);
	if (m_currentState) m_currentState->Execute(m_owner);
}

template <class T>
void StateMachine<T>::ChangeState(State<T>* new_state)
{
	ASSERT_OR_DIE(new_state, "<StateMachine::ChangeState>: trying to change to NULL state");

	m_previousState = m_currentState;
	m_currentState->Exit(m_owner);
	m_currentState = new_state;
	m_currentState->Enter(m_owner);
}

template <class T>
void StateMachine<T>::RevertToPreviousState()
{
	ChangeState(m_previousState);
}

template <class T>
bool StateMachine<T>::IsInState(const State<T>& state) const
{
	return typeid(*m_currentState) == typeid(state);
}

template <class T>
State<T>* StateMachine<T>::CurrentState() const
{
	return m_currentState;
}

template <class T>
State<T>* StateMachine<T>::GlobalState() const
{
	return m_anytimeState;
}

template <class T>
State<T>* StateMachine<T>::PreviousState() const
{
	return m_previousState;
}
