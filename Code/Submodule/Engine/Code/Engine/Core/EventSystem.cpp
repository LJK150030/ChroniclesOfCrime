#include "Engine/EngineCommon.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/StringUtils.hpp"

EventSubscription::EventSubscription(const EventCallbackFunction callback_function_ptr)
{
	m_callbackFunctionPtr = callback_function_ptr;
}

bool EventSubscription::operator==(const EventSubscription& compare) const
{
	return m_callbackFunctionPtr == compare.m_callbackFunctionPtr;
}

EventSystem::EventSystem() = default;

EventSystem::~EventSystem() = default;

void EventSystem::Startup()
{
}

void EventSystem::Shutdown()
{
}

void EventSystem::BeginFrame()
{
}

void EventSystem::EndFrame()
{
}

void EventSystem::SubscribeEventCallbackFunction(const std::string& event_name, const EventCallbackFunction callback)
{
	m_allSubscriptions[event_name].push_back(new EventSubscription(callback));
}

void EventSystem::UnsubscribeEventCallbackFunction(const std::string& event_name, const EventCallbackFunction callback)
{
	std::map<std::string, SubscribersList>::iterator all_subs_iterator = m_allSubscriptions.find(event_name);
	if (all_subs_iterator == m_allSubscriptions.end()) return;

	SubscribersList& sub_list = all_subs_iterator->second;

	for (int callback_id = 0; callback_id < sub_list.size(); ++callback_id)
	{
		if (sub_list[callback_id]->m_callbackFunctionPtr == callback)
		{
			sub_list.erase(sub_list.begin() + callback_id);
			return;
		}
	}
}

int EventSystem::FireEvent(const std::string& event_name)
{
	NamedProperties args;
	return FireEvent(event_name, args);
}

int EventSystem::FireEvent(const std::string& event_name, EventArgs& args)
{
	const std::map<std::string, SubscribersList>::const_iterator all_subs_iterator = m_allSubscriptions.
		find(event_name);

	if (all_subs_iterator == m_allSubscriptions.end())
	{
		g_theDevConsole->PrintString(DevConsole::CONSOLE_ERROR,
		                             Stringf("Event, \"%s\", does not exist", event_name.c_str()));
		return 0;
	}

	int num_functions_called = 0;

	for (int sub_id = 0; sub_id < static_cast<int>(all_subs_iterator->second.size()); ++sub_id)
	{
		const bool stop_loop = all_subs_iterator->second[sub_id]->m_callbackFunctionPtr(args);
		num_functions_called++;
		if (stop_loop) break;
	}

	return num_functions_called;
}

int EventSystem::GetNumSubscribersForCommand(const std::string& event_name) const
{
	const std::map<std::string, SubscribersList>::const_iterator all_subs_iterator = m_allSubscriptions.
		find(event_name);
	return static_cast<int>(all_subs_iterator->second.size());
}

void EventSystem::GetSubscribedEventsList(std::vector<std::string>& event_names_with_subscribers) const
{
	for (std::map<std::string, SubscribersList>::const_iterator event_name_iterator = m_allSubscriptions.begin();
	     event_name_iterator != m_allSubscriptions.end(); ++event_name_iterator)
	{
		if (event_name_iterator->second.empty()) continue;
		event_names_with_subscribers.push_back(event_name_iterator->first);
	}
}
