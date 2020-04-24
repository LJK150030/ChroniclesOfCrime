#pragma once
#include "Game\Card.hpp"


struct ItemState
{
public:
	String		m_name = "";
	bool		m_addGameTime = false;
	//showUI = ClueFound or none
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
	String GetAsString() const;

	// MUTATORS
	void SetState(const String& starting_state);

private:
	void ImportItemStatesFromXml(const XmlElement* element);

private:
	ItemState			m_currentState;
	ItemStateList		m_states;

	const float ITEM_CARD_HEIGHT = 25.0f;
	const float ITEM_CARD_ASPECT_RATIO = 0.6108597285067873f;
};