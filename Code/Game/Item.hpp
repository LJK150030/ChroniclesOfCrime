#pragma once
#include "Game\Card.hpp"

//TODO: this card is only given to a player
class Item : public Card
{
public:
	Item();
	explicit Item(Scenario* the_setup);
	explicit Item(Scenario* the_setup, const std::string& name, const std::vector<std::string>& list_of_nicknames, const std::string& desc);
	explicit Item(Scenario* the_setup, const XmlElement* element);
	~Item() = default;

	// ACCESSORS

	// MUTATORS
};