#pragma once
#include "Game\Card.hpp"

class Character : public Card
{
public:
	Character();
	explicit Character(Scenario* the_setup);
	explicit Character(Scenario* the_setup, const std::string& name, const std::vector<std::string>& list_of_nicknames, const std::string& desc);
	~Character() = default;

	// ACCESSORS

	// MUTATORS
	
};