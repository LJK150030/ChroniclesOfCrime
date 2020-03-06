#pragma once
#include "Game\Card.hpp"

struct CharacterState
{
public:
	String		m_name = "";
	bool		m_addGameTime = false;
	ContextMode	m_contextMode = CONTEXT_NONE;
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
	void ImportCharacterStatesFromXml(const XmlElement* element);
	void ImportCharacterDialogueFromXml(const XmlElement* element);
	const CharacterState& GetCharacterState() const;
	
	// MUTATORS
	void SetState(const String& starting_state);
	
private:
	CharacterState			m_currentState;
	CharStateList			m_states;
};