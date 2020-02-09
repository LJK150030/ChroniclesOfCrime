#pragma once
#include "Game\Card.hpp"


class Evidence : public Card
{
public:
	Evidence();
	explicit Evidence(Scenario* the_setup);
	explicit Evidence(Scenario* the_setup, const std::string& name, const std::vector<std::string>& list_of_nicknames, const std::string& desc);
	explicit Evidence(Scenario* the_setup, const XmlElement* element);
	~Evidence() = default;

	// ACCESSORS

	// MUTATORS

};
