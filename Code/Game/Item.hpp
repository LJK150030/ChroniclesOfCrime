#pragma once
#include "Game\Card.hpp"


struct ItemState
{
public:
	String		m_name = "";
	bool		m_addGameTime = false;
};


class Item : public Card
{
public:
	Item();
	explicit Item(Scenario* the_setup);
	explicit Item(Scenario* the_setup, const std::string& name, const std::vector<std::string>& list_of_nicknames, const std::string& desc);
	explicit Item(Scenario* the_setup, const XmlElement* element);
	~Item() = default;

	// ACCESSORS
	const ItemState& GetItemState() const;

	// MUTATORS
	void SetState(const String& starting_state);

private:
	ItemState			m_currentState;
	ItemStateList		m_states;
};