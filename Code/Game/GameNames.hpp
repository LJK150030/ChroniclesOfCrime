#pragma once

inline char g_locationHeader[]	= "Heading to: ";
inline char g_characterHeader[] = "Interrogating: ";
inline char g_itemHeader[]		= "Found Item: ";

inline char g_locationCommand[]		= "GOTO";
inline char g_characterCommand[]	= "ASK";
inline char g_itemCommand[]			= "VIEW";

inline String g_unknownLocation = "Location cards represent London's districts. Without an address or a precise goal you cannot go there.";

enum CardType
{
	UNKNOWN_CARD_TYPE = -1,

	CARD_LOCATION,
	CARD_CHARACTER,
	CARD_ITEM,

	NUM_CARD_TYPES
};

enum LogType
{
	LOG_ERROR = -1,

	LOG_ECHO,
	LOG_MESSAGE,
	LOG_LOCATION,
	LOG_CHARACTER,
	LOG_ITEM,

	NUM_LOG_TYPES
};