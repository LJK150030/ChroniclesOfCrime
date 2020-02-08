#pragma once
#include "Game/GameCommon.hpp"
#include "Game/GameNames.hpp"

#include <string>
#include <vector>
#include <map>

class Game;

class Card
{
public:
	Card();
	explicit Card(Game* the_game, CardType type);
	explicit Card(Game* the_game, CardType type, const String& name, const StringList& list_of_nicknames,
		const String& desc);
	~Card();

public:
	bool		IsDiscovered() const;
	String&		GetName();
	StringList&	GetListOfNicknames();
	String&		GetDescription();
	
protected:
	Game*		m_theGame = nullptr;
	CardType	m_type = UNKNOWN_CARD_TYPE;

	bool		m_found = false;
	String		m_name = "";
	StringList	m_nickNames;
	String		m_description = "";
};
