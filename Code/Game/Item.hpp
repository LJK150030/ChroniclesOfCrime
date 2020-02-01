#pragma once
#include "Game/GameNames.hpp"
#include <string>

//TODO: this card is only given to a player
class Item
{
public:
	static Item			s_Items[NUM_ITEMS];
	static std::string	s_unknownItemLine[3];

public:
	static void LoadInScenarioItems();
	static std::string InvestigateItem(const char* name);

public:
	Item() = default;
	Item(ItemID item_id, const std::string& name, const std::string& desc);

	~Item() = default;

private:
	ItemID				m_category = UNKNOWN_ITEM;
	bool				m_found = false;
	std::string			m_name = "";
	std::string			m_description = "";

};