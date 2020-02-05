#pragma once
#include "Game/GameNames.hpp"
#include <string>

class Card
{
public:
	static Card*		s_decks[NUM_CARD_TYPES];
	static std::string	s_unknownCardLine[NUM_CARD_TYPES][3];
	// for each card type, there should be three (for now)
	// "I don't know" lines

public:
	static void LoadInScenarioCards();
	// TODO: LoadInScenarioCardsFromXML();

public:
	Card() = default;
	~Card() = default;

	std::string InteractWithCard(CardType type, const char* name);

private:
	//TODO: CardDefinition*		m_cardDef = nullptr;
	CardType			m_type = UNKNOWN_CARD_TYPE;
	//LocationDistrict	m_category = UNKNOWN_LOC; this is a subcategory

	bool				m_found = false;
	std::string			m_name = "";
	std::string			m_description = "";

};