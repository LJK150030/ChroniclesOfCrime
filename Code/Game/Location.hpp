#pragma once
#include "Game/Card.hpp"

class Location: public Card
{
public:
	Location();
	explicit Location(Scenario* the_setup);
	explicit Location(Scenario* the_setup, const String& name, const StringList& list_of_nicknames,
		const String& desc);
	explicit Location(Scenario* the_setup, const XmlElement* element);
	~Location() = default;

	// ACCESSORS
	bool		IsCharacterInLocation(const Character* character) const;
	String		GetBestDialogue() const;
	
	// MUTATORS
	void AddCharacterToLocation(const Character* character);
	void RemoveCharacterFromLocation(const Character* character);
	
private:
	std::vector<const Character*> m_charsInLoc;
};