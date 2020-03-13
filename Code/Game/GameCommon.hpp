#pragma once
#include "Engine/EngineCommon.hpp"
#include "Engine/Math/AABB2.hpp"

//--------------------------------------------------
//Engine related globals
//--------------------------------------------------
class RenderContext;
class InputSystem;
class AudioSystem;


//--------------------------------------------------
//Game related globals
//--------------------------------------------------
class App;

// global game components
extern App* g_theApp;
extern EventSystem* g_theDialogueEventSystem;

// key codes
constexpr int SHIFT_KEY = 16;
constexpr int ESC_KEY = 27;
constexpr int SPACE_BAR = 32;
constexpr int LEFT_ARROW = 37;
constexpr int UP_ARROW = 38;
constexpr int RIGHT_ARROW = 39;
constexpr int DOWN_ARROW = 40;
constexpr int NUM_0_KEY = 48;
constexpr int NUM_1_KEY = 49;
constexpr int NUM_2_KEY = 50;
constexpr int NUM_3_KEY = 51;
constexpr int NUM_4_KEY = 52;
constexpr int NUM_5_KEY = 53;
constexpr int NUM_6_KEY = 54;
constexpr int NUM_7_KEY = 55;
constexpr int NUM_8_KEY = 56;
constexpr int NUM_9_KEY = 57;
constexpr int A_KEY = 65;
constexpr int B_KEY = 66;
constexpr int C_KEY = 67;
constexpr int D_KEY = 68;
constexpr int E_KEY = 69;
constexpr int F_KEY = 70;
constexpr int G_KEY = 71;
constexpr int H_KEY = 72;
constexpr int I_KEY = 73;
constexpr int J_KEY = 74;
constexpr int K_KEY = 75;
constexpr int L_KEY = 76;
constexpr int M_KEY = 77;
constexpr int N_KEY = 78;
constexpr int O_KEY = 79;
constexpr int P_KEY = 80;
constexpr int Q_KEY = 81;
constexpr int R_KEY = 82;
constexpr int S_KEY = 83;
constexpr int T_KEY = 84;
constexpr int U_KEY = 85;
constexpr int V_KEY = 86;
constexpr int W_KEY = 87;
constexpr int X_KEY = 88;
constexpr int Y_KEY = 89;
constexpr int Z_KEY = 90;
constexpr int F1_KEY = 112;
constexpr int F2_KEY = 113;
constexpr int F3_KEY = 114;
constexpr int F4_KEY = 115;
constexpr int F5_KEY = 116;
constexpr int F6_KEY = 117;
constexpr int F7_KEY = 118;
constexpr int F8_KEY = 119;
constexpr int F9_KEY = 110;
constexpr int F10_KEY = 111;
constexpr int F11_KEY = 112;
constexpr int F12_KEY = 113;
constexpr int TILDE_KEY = 192;

//Window config
constexpr float WORLD_ASPECT = 1.77777777777777777777777777778f;
constexpr float WORLD_HEIGHT = 100.0f;
constexpr float WORLD_WIDTH = WORLD_HEIGHT * WORLD_ASPECT;
constexpr float WORLD_CENTER_X = 0.0f;
constexpr float WORLD_CENTER_Y = 0.0f;

const Vec2 WORLD_BL_CORNER(-0.5f * WORLD_WIDTH, -0.5f * WORLD_HEIGHT);
const Vec2 WORLD_TR_CORNER(0.5f * WORLD_WIDTH, 0.5f * WORLD_HEIGHT);
const AABB2 WORLD_BOUNDS(WORLD_BL_CORNER, WORLD_TR_CORNER);


//Global game const
constexpr int	MAX_INPUT = 64;
constexpr int	MAX_COMMAND_LENGTH = 16;


//Global functions
int		StringCompare(const char* str1, const char* str2);
int		StringNCompare(const char* str1, const char* str2, int first_n_chars);
void	StringTrim(char* str);

//--------------------------------------------------
//Scenario related globals
//--------------------------------------------------
class Card;
class Location;
class Item;
class Character;
class EventSystem;
struct Dialogue;
struct LocationState;
struct CharacterIntro;
struct CharacterState;

typedef std::string							String;
typedef std::vector<String>					StringList;
typedef std::pair<String, int>				Lookup;
typedef std::map<String, int>				LookupTable;
typedef std::map<String, int>::iterator		LookupItr;
typedef std::vector<Location>				LocationList;
typedef std::vector<Item>					ItemList;
typedef std::vector<Character>				CharacterList;
typedef std::vector<LocationState>			LocStateList;
typedef std::vector<CharacterIntro>		LocCharacterIntro;
typedef std::vector<CharacterState>			CharStateList;


inline char g_locationHeader[] = "Heading to: ";
inline char g_characterHeader[] = "Interrogating: ";
inline char g_itemHeader[] = "Found Item: ";


inline char g_locationCommand[] = "GOTO";
inline char g_characterCommand[] = "ASK";
inline char g_itemCommand[] = "VIEW";


inline String g_unknownLocation = "Location cards represent London's districts. Without an address or a precise goal you cannot go there.";
inline String g_sameLocation = "You are already in this location. The location is indicated in the top left corner.";
inline String g_sameCharacter = "You are already talking to this character.";


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

enum ContextMode
{
	CONTEXT_NONE = -1,
	CONTEXT_INTERROGATION,
	CONTEXT_PLAYER_CHARACTER
};
