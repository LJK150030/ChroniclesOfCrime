#pragma once

inline char g_locationHeader[]	= "Heading to: ";
// inline char g_contactHeader[]	= "Incoming call: ";
// inline char g_characterHeader[] = "Interrogating: ";
// inline char g_evidenceHeader[]	= "Found Evidence: ";
// inline char g_itemHeader[]		= "Found Item: ";

inline char g_locationCommand[]		= "GOTO";
// inline char g_contactCommand[]		= "CALL";
// inline char g_characterCommand[]	= "ASK";
// inline char g_evidenceCommand[]		= "VIEW";
// inline char g_itemCommand[]			= "CHECK";

enum CardType
{
	UNKNOWN_CARD_TYPE = -1,

	CARD_LOCATION,
	CARD_CONTACT,
	CARD_CHARACTER,
	CARD_EVIDENCE,
	CARD_ITEM,

	NUM_CARD_TYPES
};