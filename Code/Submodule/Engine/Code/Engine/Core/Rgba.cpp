#include "Engine/EngineCommon.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Vec3.hpp"

const STATIC Rgba Rgba::WHITE(1.f, 1.f, 1.f, 1.f);
const STATIC Rgba Rgba::GRAY(0.5f, 0.5f, 0.5f, 1.f);
const STATIC Rgba Rgba::BLACK(0.f, 0.f, 0.f, 1.f);
const STATIC Rgba Rgba::RED(1.f, 0.f, 0.f, 1.f);
const STATIC Rgba Rgba::ORANGE(1.0f, 0.5f, 0.f, 1.f);
const STATIC Rgba Rgba::YELLOW(1.f, 1.f, 0.f, 1.f);
const STATIC Rgba Rgba::GREEN(0.f, 1.f, 0.f, 1.f);
const STATIC Rgba Rgba::BLUE(0.f, 0.f, 1.f, 1.f);
const STATIC Rgba Rgba::INDIGO(0.5f, 0.f, 1.f, 1.f);
const STATIC Rgba Rgba::VIOLET(1.f, 0.f, 0.5f, 1.f);
const STATIC Rgba Rgba::CYAN(0.f, 1.f, 1.f, 1.f);
const STATIC Rgba Rgba::MAGENTA(1.f, 0.f, 1.f, 1.f);
const STATIC Rgba Rgba::CLEAR(0.f, 0.f, 0.f, 0.f);
const STATIC Rgba Rgba::FLAT(0.5f, 0.5f, 1.0f, 1.0f);

//-----------------------------------------------------------------------------------------------
Rgba::Rgba(const float initial_r, const float initial_g, const float initial_b, const float initial_a)
	: r(initial_r)
	  , g(initial_g)
	  , b(initial_b)
	  , a(initial_a)
{
	// 	r_uchar = RoundToNearestInt(initial_r * 255.0f);
	// 	g_uchar = RoundToNearestInt(initial_g * 255.0f);
	// 	b_uchar = RoundToNearestInt(initial_b * 255.0f);
	// 	a_uchar = RoundToNearestInt(initial_a * 255.0f);
}

Rgba::Rgba(const Vec3& vec)
{
	r = vec.x;
	g = vec.y;
	b = vec.z;
	a = 1.0f;
}

Rgba::Rgba(const std::string& text)
{
	SetFromText(text.c_str());
}

Rgba::Rgba(unsigned hex)
{
	unsigned int temp_bit = hex;

	unsigned int red_hex = temp_bit >> 24;
	red_hex &= 255;

	temp_bit = hex;
	unsigned int green_hex = temp_bit >> 16;
	green_hex &= 255;

	temp_bit = hex;
	unsigned int blue_hex = temp_bit >> 8;
	blue_hex &= 255;

	temp_bit = hex;
	unsigned int alpha_hex = temp_bit;
	alpha_hex &= 255;

	r = static_cast<float>(red_hex) / 255.0f;  // Extract the RR byte
	g = static_cast<float>(green_hex) / 255.0f;   // Extract the GG byte
	b = static_cast<float>(blue_hex) / 255.0f;        // Extract the BB byte
	a = static_cast<float>(alpha_hex) / 255.0f;        // Extract the AA byte
}

std::string Rgba::GetAsString() const
{
	const int red_byte = RoundDownToNearestInt(r * 255.0f);
	const int green_byte = RoundDownToNearestInt(g * 255.0f);
	const int blue_byte = RoundDownToNearestInt(b * 255.0f);
	const int alpha_byte = RoundDownToNearestInt(a * 255.0f);

	return Stringf("%i,%i,%i,%i", red_byte, green_byte, blue_byte, alpha_byte);
}

void Rgba::SetFromText(const char* text)
{
	std::vector<std::string> splitWords = SplitStringOnDelimiter(text, ',');

	ASSERT_OR_DIE(splitWords.size() == 3 || splitWords.size() == 4,
		Stringf("Expecting an Rgba from the following line, %s", text));

	const int red_byte = atoi(splitWords[0].c_str());
	const int green_byte = atoi(splitWords[1].c_str());
	const int blue_byte = atoi(splitWords[2].c_str());
	int alpha_byte = 255;

	if (splitWords.size() == 4)
	{
		alpha_byte = atoi(splitWords[3].c_str());
	}

	// 	r_uchar = red_byte;
	// 	g_uchar = green_byte;
	// 	b_uchar = blue_byte;
	// 	a_uchar = alpha_byte;

	r = ClampFloat(static_cast<float>(red_byte) / 255.0f, 0.0f, 1.0f);
	g = ClampFloat(static_cast<float>(green_byte) / 255.0f, 0.0f, 1.0f);
	b = ClampFloat(static_cast<float>(blue_byte) / 255.0f, 0.0f, 1.0f);
	a = ClampFloat(static_cast<float>(alpha_byte) / 255.0f, 0.0f, 1.0f);
}

void Rgba::SetFromHex(const std::string& hex_string)
{
	//assuming the string will be "0xrrggbbaa"
	std::string start_hex = hex_string.substr(0, 2);

	ASSERT_OR_DIE(start_hex == "0x", "attempting to read a non-hex string for color, please make sure the hex is in the following format 0xrrggbbaa");

	std::string red_hex = hex_string.substr(2, 2);
	std::string green_hex = hex_string.substr(4, 2);
	std::string blue_hex = hex_string.substr(6, 2);
	std::string alpha_hex = hex_string.substr(8, 2);
	
	const unsigned int red_integer = std::stoi (start_hex + red_hex,nullptr,0);
	const unsigned int green_integer = std::stoi (start_hex + green_hex,nullptr,0);
	const unsigned int blue_integer = std::stoi (start_hex + blue_hex,nullptr,0);
	const unsigned int alpha_integer = std::stoi (start_hex + alpha_hex,nullptr,0);

	r = red_integer / 255.0f;  // Extract the RR byte
	g = green_integer / 255.0f;   // Extract the GG byte
	b = blue_integer / 255.0f;        // Extract the BB byte
	a = alpha_integer / 255.0f;        // Extract the AA byte
}

void Rgba::SetFromByte(const unsigned char red_byte, const unsigned char green_byte, const unsigned char blue_byte,
                       const unsigned char alpha_byte)
{
	// 	r_uchar = red_byte;
	// 	g_uchar = green_byte;
	// 	b_uchar = blue_byte;
	// 	a_uchar = alpha_byte;

	r = static_cast<float>(red_byte) / 255.0f; //, 0.0f, 1.0f;
	g = static_cast<float>(green_byte) / 255.0f; //, 0.0f, 1.0f;
	b = static_cast<float>(blue_byte) / 255.0f; //, 0.0f, 1.0f;
	a = static_cast<float>(alpha_byte) / 255.0f; //, 0.0f, 1.0f;
}

bool Rgba::operator==(const Rgba& compare) const
{
	// 	return fabs(r - compare.r) <= WEB_COLOR_STRIDE && fabs(g - compare.g) <= WEB_COLOR_STRIDE && fabs(b - compare.b) <=
	// 		WEB_COLOR_STRIDE;

	const int r_uchar_1 = RoundToNearestInt(r * 255.0f);
	const int g_uchar_1 = RoundToNearestInt(g * 255.0f);
	const int b_uchar_1 = RoundToNearestInt(b * 255.0f);

	const int r_uchar_2 = RoundToNearestInt(compare.r * 255.0f);
	const int g_uchar_2 = RoundToNearestInt(compare.g * 255.0f);
	const int b_uchar_2 = RoundToNearestInt(compare.b * 255.0f);

	return r_uchar_1 == r_uchar_2 && g_uchar_1 == g_uchar_2 && b_uchar_1 == b_uchar_2;
}

const Rgba Rgba::operator*(float uniform_scale) const
{
	return Rgba(
		r * uniform_scale,
		g * uniform_scale,
		b * uniform_scale,
		a * uniform_scale
	);
}
