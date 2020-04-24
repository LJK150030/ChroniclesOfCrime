#include "Game/VictoryCondition.hpp"
#include "Game/Scenario.hpp"
#include "Game/Location.hpp"
#include "Game/Character.hpp"
#include "Game/Item.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"

VictoryCondition::VictoryCondition(Scenario* the_scenario, const XmlElement* element):
	m_scenario(the_scenario)
{
	// Get all the attributes from the Location element
	for (const XmlAttribute* attribute = element->FirstAttribute();
		attribute;
		attribute = attribute->Next()
		)
	{
		String attribute_name = StringToLower(attribute->Name());

		if (attribute_name == "cardtype")
		{
			String value = StringToLower(attribute->Value());

			if(value == "location")
			{
				m_cardType = CARD_LOCATION;
			}
			else if (value == "character")
			{
				m_cardType = CARD_CHARACTER;
			}
			else if (value == "item")
			{
				m_cardType = CARD_ITEM;
			}
			else
			{
				ERROR_AND_DIE(Stringf("Error in victory condition file. %s is not a valid card type", attribute->Value()))
			}
		}
		else if(attribute_name == "cardname")
		{
			m_cardName = StringToLower(attribute->Value());
		}
		else if(attribute_name == "cardstate")
		{
			m_requiredState = StringToLower(attribute->Value());
		}
		else
		{
			ERROR_AND_DIE(Stringf("Error in victory condition file. %s is not a valid attribute", attribute->Name()))
		}
	}
}


VictoryCondition::~VictoryCondition()
{
}


bool VictoryCondition::HasConditionsBeenMet() const
{
	return m_hasBeenMet;
}


void VictoryCondition::TestCondition()
{
	if(!m_hasBeenMet)
	{
		switch (m_cardType)
		{
			case CARD_LOCATION: 
			{
				LookupItr loc_itr;
				const bool in_scenario = m_scenario->IsLocationInLookupTable(loc_itr, m_cardName);
				ASSERT_OR_DIE(in_scenario, Stringf("Could not find the location %s for condition check.", m_cardName.c_str()));

				Location* loc_card = m_scenario->GetLocationFromList(loc_itr->second);
				const String loc_state_name = StringToLower(loc_card->GetLocationState().m_name);

				m_hasBeenMet = loc_state_name == m_requiredState;
				break;
			}
			case CARD_CHARACTER: 
			{
				LookupItr char_itr;
				const bool in_scenario = m_scenario->IsCharacterInLookupTable(char_itr, m_cardName);
				ASSERT_OR_DIE(in_scenario, Stringf("Could not find the character %s for condition check.", m_cardName.c_str()));

				Character* char_card = m_scenario->GetCharacterFromList(char_itr->second);
				const String char_state_name = StringToLower(char_card->GetCharacterState().m_name);

				m_hasBeenMet = char_state_name == m_requiredState;
				break;
			}
			case CARD_ITEM: 
			{
				LookupItr item_itr;
				const bool in_scenario = m_scenario->IsItemInLookupTable(item_itr, m_cardName);
				ASSERT_OR_DIE(in_scenario, Stringf("Could not find the item %s for condition check.", m_cardName.c_str()));

				Item* item_card = m_scenario->GetItemFromList(item_itr->second);
				const String item_state_name = StringToLower(item_card->GetItemState().m_name);

				m_hasBeenMet = item_state_name == m_requiredState;
				break;
				break;
			}
			default:
			{
				ERROR_AND_DIE(Stringf("Card Type was not set for %s condition", m_cardName.c_str()))
				break;
			}
		}
	}
}
