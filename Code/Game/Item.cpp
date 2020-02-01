#include "Engine/EngineCommon.hpp"
#include "Game/Item.hpp"

STATIC Item Item::s_Items[NUM_ITEMS];
STATIC std::string Item::s_unknownItemLine[3];

STATIC void Item::LoadInScenarioItems()
{
	// leading scenario cards
	s_Items[ITEM_1] = Item(
		ITEM_1,
		"money",
		"A wad of money"
	);

	s_Items[ITEM_2] = Item(
		ITEM_2,
		"badge",
		"A Security badge"
	);

	s_Items[ITEM_4] = Item(
		ITEM_4,
		"poison",
		"A bottle of hydrochloric acid"
	);

	// loading scenario search lines
	s_unknownItemLine[0] = "oh, where did I put it...";
	s_unknownItemLine[1] = "I know it is here somewhere.";
	s_unknownItemLine[2] = "what was I looking for again?";
}

STATIC std::string Item::InvestigateItem(const char* name)
{
	//TODO: how can we look up faster with just the name and not the category
	for (int item_idx = 0; item_idx < NUM_ITEMS; ++item_idx)
	{
		if (s_Items[item_idx].m_name.compare(name) == 0)
		{
			s_Items[item_idx].m_found = true;
			return g_itemHeader + s_Items[item_idx].m_description;
		}
	}

	const int random_dialog_idx = g_randomNumberGenerator.GetRandomIntInRange(0, 2);
	return s_unknownItemLine[random_dialog_idx];
}

Item::Item(ItemID item_id, const std::string& name, const std::string& desc) :
	m_category(item_id), m_name(name), m_description(desc)
{
}


