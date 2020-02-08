// #pragma once
// #include "Game\Card.hpp"
// 
// class Character : public Card
// {
// public:
// 	static std::vector<Character>		s_characters;
// 	static LookupContainer				s_characterLookup;
// 	static std::vector<std::string>		s_missingCharacterLine;
// 
// public:
// 	static void LoadInScenarioCharacters();
// 	static std::string InterrogateCharacter(const char* name);
// 
// public:
// 	Character();
// 	explicit Character(Game* the_game);
// 	explicit Character(Game* the_game, const std::string& name, const std::vector<std::string>& list_of_nicknames, const std::string& desc);
// 	~Character() = default;
// 
// 	// ACCESSORS
// 
// 	// MUTATORS 
// 
// };