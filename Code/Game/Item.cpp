// #include "Engine/EngineCommon.hpp"
// #include "Game/Item.hpp"
// 
// STATIC ItemList		Item::s_items = ItemList();
// STATIC LookupTable	Item::s_itemLookup = LookupTable();
// STATIC StringList	Item::s_unknownItemLine = StringList();
// 
// STATIC void Item::LoadInScenarioItems()
// {
// 	ManuallySetItems();
// 	SetupItemLookup();
// }
// 
// STATIC void Item::ManuallySetItems()
// {
// 	// loading scenario search lines
// 	s_unknownItemLine.emplace_back("oh, where did I put it...");
// 	s_unknownItemLine.emplace_back("I know it is here somewhere.");
// 	s_unknownItemLine.emplace_back("what was I looking for again?");
// 
// 	// Items used in the game
// 	StringList money_nicknames;
// 	money_nicknames.emplace_back("cash");
// 	money_nicknames.emplace_back("dough");
// 	money_nicknames.emplace_back("wad");
// 	std::string money_des = "A wad of cash";
// 	s_items.emplace_back("money", home_nicknames, home_des);
// 
// 	s_Items[ITEM_2] = Item(
// 		ITEM_2,
// 		"badge",
// 		"A Security badge"
// 	);
// 
// 	s_Items[ITEM_4] = Item(
// 		ITEM_4,
// 		"poison",
// 		"A bottle of hydrochloric acid"
// 	);
// }
// 
// STATIC void Item::ReadItemsXml()
// {
// }
// 
// STATIC void Item::SetupItemLookup()
// {
// }
// 
// STATIC std::string Item::InvestigateItem(const char* name)
// {
// 	//TODO: how can we look up faster with just the name and not the category
// 	for (int item_idx = 0; item_idx < NUM_ITEMS; ++item_idx)
// 	{
// 		if (s_Items[item_idx].m_name.compare(name) == 0)
// 		{
// 			s_Items[item_idx].m_found = true;
// 			return g_itemHeader + s_Items[item_idx].m_description;
// 		}
// 	}
// 
// 	const int random_dialog_idx = g_randomNumberGenerator.GetRandomIntInRange(0, 2);
// 	return s_unknownItemLine[random_dialog_idx];
// }
// 
// 
// Item::Item() { m_type = CARD_ITEM; }
// Item::Item(Game* the_game) : Card(the_game, CARD_ITEM) { }
// Item::Item(Game* the_game, const String& name, const StringList& list_of_nicknames,
// 	const String& desc) : Card(the_game, CARD_ITEM, name, list_of_nicknames, desc) { }
