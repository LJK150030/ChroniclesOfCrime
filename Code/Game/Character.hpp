#pragma once
#include "Game\Card.hpp"

struct CharacterState
{
public:
	String		m_name = "";
	bool		m_addGameTime = false;
	ContextMode	m_contextMode = CONTEXT_NONE;
};


struct CharacterDialogue
{
public:
	CharacterDialogue();
	~CharacterDialogue();

public:
	String m_characterState = "*";

	String m_locationName = "*";
	String m_locationState = "*";

	// either a character or item
	CardType m_cardType = UNKNOWN_CARD_TYPE;
	String m_cardName = "*";
	String m_cardState = "*";

	String m_line = "default dialogue for any card";
	ActionList m_actions = ActionList();
};


class Character : public Card
{
public:
	Character();
	explicit Character(Scenario* the_setup);
	explicit Character(Scenario* the_setup, const std::string& name, const std::vector<std::string>& list_of_nicknames, const std::string& desc);
	explicit Character(Scenario* the_setup, const XmlElement* element);
	~Character() = default;

	// ACCESSORS
	void					ImportCharacterStatesFromXml(const XmlElement* element);
	void					ImportCharacterDialogueFromXml(const XmlElement* element, CardType type);
	const CharacterState&	GetCharacterState() const;
	bool					AskAboutCharacter(String& out, const Location* location, const Character* character);
	bool					AskAboutItem(String& out, const Location* location, const Item* item);
	
	// MUTATORS
	void SetState(const String& starting_state);

private:
	CharacterState			m_currentState;
	CharStateList			m_states;

	CharacterDialogueList		m_dialogueAboutCharacter;
	CharacterDialogueList		m_dialogueAboutItem;

	const float CHAR_CARD_HEIGHT = 25.0f;
	const float CHAR_CARD_ASPECT_RATIO = 0.65675934803451581975071907957814f;
};