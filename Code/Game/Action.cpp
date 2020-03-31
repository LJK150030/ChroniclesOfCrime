#include "Game/Action.hpp"
#include "Game/App.hpp"
#include "Game/Game.hpp"
#include "Game/Scenario.hpp"
#include "Game/DialogueSystem.hpp"
#include "Game/Location.hpp"
#include "Game/Character.hpp"
#include "Game/Item.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"

Action::Action(Scenario* the_setup): m_theScenario(the_setup)
{
}

Action::~Action()
{
}

void Action::Execute()
{
	ERROR_AND_DIE("Base class execute should never be called.");
}


//-----------------------------------------------------
DisplayText::DisplayText(Scenario* the_setup, const String& message): Action(the_setup), m_message(message)
{
}


DisplayText::~DisplayText()
{
}


void DisplayText::Execute()
{
	DialogueSystem* ds = g_theApp->GetTheGame()->GetDialogueSystem();
	ds->AddLog(LOG_LOCATION, m_message);
}


//-----------------------------------------------------
ChangeCardState::ChangeCardState(Scenario* the_setup, CardType type, const String& card_name, const String& from_state_name, const String& to_state_name):
	Action(the_setup), m_type(type), m_cardName(card_name), m_fromStateName(from_state_name), m_toStateName(to_state_name)
{
}


ChangeCardState::~ChangeCardState()
{
}


void ChangeCardState::Execute()
{
	ASSERT_OR_DIE(m_type != UNKNOWN_CARD_TYPE, "Attempting to change the state of an unknown card type");

	String current_state = "";
	
	switch(m_type)
	{
		case CARD_LOCATION:
		{
			LookupItr loc_itr;
			bool in_scenario = m_theScenario->IsLocationInLookupTable(loc_itr, m_cardName);
			if (in_scenario)
			{
				Location* loc_card = m_theScenario->GetLocationFromList(loc_itr->second);
				current_state = loc_card->GetLocationState().m_name;

				if(current_state == m_fromStateName)
				{
					loc_card->SetState(m_toStateName);
				}
			}
			break;
		}
		case CARD_CHARACTER:
		{
			LookupItr char_itr;
			bool in_scenario = m_theScenario->IsCharacterInLookupTable(char_itr, m_cardName);
			if (in_scenario)
			{
				Character* char_card = m_theScenario->GetCharacterFromList(char_itr->second);
				current_state = char_card->GetCharacterState().m_name;

				if (current_state == m_fromStateName)
				{
					char_card->SetState(m_toStateName);
				}
			}
			break;
		}
		case CARD_ITEM:
		{
			LookupItr item_itr;
			bool in_scenario = m_theScenario->IsItemInLookupTable(item_itr, m_cardName);
			if (in_scenario)
			{
				Item* item_card = m_theScenario->GetItemFromList(item_itr->second);
				current_state = item_card->GetItemState().m_name;

				if (current_state == m_fromStateName)
				{
					item_card->SetState(m_toStateName);
				}
			}
			break;
		}
	}
}
