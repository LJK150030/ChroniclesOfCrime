#include "Engine/EngineCommon.hpp"
#include "Game/Character.hpp"

STATIC Character Character::s_characters[NUM_CHARS];
STATIC std::string Character::s_missingCharacterLine[3];

STATIC void Character::LoadInScenarioCharacters()
{
	// leading scenario cards
	s_characters[CHAR_2] = Character(
		CHAR_2,
		"jannet",
		"I was just walking home when I heard a scream!"
	);

	s_characters[CHAR_4] = Character(
		CHAR_4,
		"becky",
		"AH! I just needed more time!"
	);


	// loading scenario search lines
	s_missingCharacterLine[0] = "Sorry, give me a minute";
	s_missingCharacterLine[1] = "I'm at a lost for words";
	s_missingCharacterLine[2] = "............";
}

STATIC std::string Character::InterrogateCharacter(const char* name)
{
	//TODO: how can we look up faster with just the name and not the category
	//TODO: make sure to set everything to lower or upper
	for (int character_idx = 0; character_idx < NUM_CHARS; ++character_idx)
	{
		if (s_characters[character_idx].m_name.compare(name) == 0)
		{
			s_characters[character_idx].m_found = true;
			return g_characterHeader + s_characters[character_idx].m_description;
		}
	}

	const int random_dialog_idx = g_randomNumberGenerator.GetRandomIntInRange(0, 2);
	return s_missingCharacterLine[random_dialog_idx];
}

Character::Character(CharacterID char_id, const std::string& name, const std::string& desc) :
	m_category(char_id), m_name(name), m_description(desc)
{
}


