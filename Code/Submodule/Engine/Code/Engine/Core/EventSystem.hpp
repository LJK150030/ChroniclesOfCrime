#pragma once
#include <vector>
#include <map>
//#include "Engine/Core/NamedStrings.hpp"
#include "NamedProperties.hpp"

// Type definition nickname for:
// "any (standalone or static) function which takes an EventArgs by (non-const) refrence, and returns a bool"
//

typedef NamedProperties EventArgs;
typedef bool (*EventCallbackFunction)(EventArgs& args);

//
// ....0r, in C++11 and beyond, we can instead say:
// using EventCallbackFunction = bool (*)(EventArgs& args);

class EventSubscription
{
	friend class EventSystem;

private:
	explicit EventSubscription(EventCallbackFunction callback_function_ptr);
	bool operator==(const EventSubscription& compare) const;

private:
	EventCallbackFunction m_callbackFunctionPtr = nullptr;
};

class EventSystem
{
	typedef std::vector<EventSubscription*> SubscribersList;

public:
	EventSystem();
	~EventSystem();
	void Startup();
	void Shutdown();
	void BeginFrame();
	void EndFrame();

	void SubscribeEventCallbackFunction(const std::string& event_name, EventCallbackFunction callback);
	void UnsubscribeEventCallbackFunction(const std::string& event_name, EventCallbackFunction callback);
	int FireEvent(const std::string& event_name);
	int FireEvent(const std::string& event_name, EventArgs& args);
	int GetNumSubscribersForCommand(const std::string& event_name) const;
	void GetSubscribedEventsList(std::vector<std::string>& event_names_with_subscribers) const;

private:
	std::map<std::string, SubscribersList> m_allSubscriptions;
};
