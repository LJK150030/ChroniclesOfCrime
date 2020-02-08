// #pragma once
// #include "Game\Card.hpp"
// 
// //TODO: this card is only given to a player
// class Item : public Card
// {
// public:
// 	static ItemList		s_items;
// 	static LookupTable	s_itemLookup;
// 	static StringList	s_unknownItemLine;
// 
// public:
// 	// Setup Scenario
// 	static void LoadInScenarioItems();
// 	static void ManuallySetItems();
// 	static void ReadItemsXml();
// 	static void SetupItemLookup();
// 
// 	// Scenario interaction functions
// 	static String InvestigateItem(const char* name);
// 
// public:
// 	Item();
// 	explicit Item(Game* the_game);
// 	explicit Item(Game* the_game, const String& name, const StringList& list_of_nicknames, const String& desc);
// 	~Item() = default;
// 
// 	// ACCESSORS
// 
// 	// MUTATORS 
// 
// };