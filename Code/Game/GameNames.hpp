#pragma once

inline char g_evidenceHeader[]	= "Found Evidence: ";
inline char g_locationHeader[]	= "Heading to: ";
inline char g_contactHeader[]	= "Incoming call: ";
inline char g_characterHeader[] = "Interrogating: ";
inline char g_itemHeader[]		= "Found Item: ";

inline char g_evidenceCommand[]		= "VIEW";
inline char g_locationCommand[]		= "GOTO";
inline char g_contactCommand[]		= "CALL";
inline char g_characterCommand[]	= "ASK";
inline char g_itemCommand[]			= "CHECK";

enum CardType
{
	UNKNOWN_CARD_TYPE = -1,

	CARD_LOCATION,
	CARD_ITEM,
	CARD_EVIDENCE,
	CARD_CHARACTER,
	CARD_CONTACT,

	NUM_CARD_TYPES
};

enum EvidenceCategory
{
	UNKNOWN_EVIDENCE = -1,

	EVIDENCE_COMPUTER_DEVICES,
	EVIDENCE_BLOOD_AND_ORGANS,
	EVIDENCE_PHOTOS_AND_CAMERAS,

	NUM_EVIDENCE
};

enum LocationDistrict
{
	UNKNOWN_LOC = -1,

	LOC_SCOTLAND_YARD,
	LOC_WESTMINSTER,
	LOC_THE_CITY,

	NUM_LOCATIONS
};

enum ContactSpecialist
{
	UNKNOWN_CONTACT = -1,

	CONT_HACKER,
	CONT_DOCTOR,
	CONT_CRIMINOLOGIST,
	CONT_SCIENTIST,

	NUM_CONTACTS	
};

enum CharacterID
{
	UNKNOWN_CHAR = -1,

	CHAR_1,
	CHAR_2,
	CHAR_3,
	CHAR_4,

	NUM_CHARS
};

enum ItemID
{
	UNKNOWN_ITEM = -1,

	ITEM_1,
	ITEM_2,
	ITEM_3,
	ITEM_4,
	
	NUM_ITEMS
};