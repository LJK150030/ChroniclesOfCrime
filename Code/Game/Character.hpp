#pragma once
#include "Game/GameNames.hpp"
#include <string>

class Character
{
public:
	static Character		s_characters[NUM_CHARS];
	static std::string		s_missingCharacterLine[3];

public:
	static void LoadInScenarioCharacters();
	static std::string InterrogateCharacter(const char* name);

public:
	Character() = default;
	Character(CharacterID char_id, const std::string& name, const std::string& desc);

	~Character() = default;

private:
	CharacterID			m_category = UNKNOWN_CHAR;
	bool				m_found = false;
	std::string			m_name = "";
	std::string			m_description = "";

};