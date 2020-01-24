#pragma once
#include <string>

struct Vec3;

struct Rgba
{
public:
	const static Rgba WHITE;
	const static Rgba GRAY;
	const static Rgba BLACK;
	const static Rgba RED;
	const static Rgba ORANGE;
	const static Rgba YELLOW;
	const static Rgba GREEN;
	const static Rgba BLUE;
	const static Rgba INDIGO;
	const static Rgba VIOLET;
	const static Rgba MAGENTA;
	const static Rgba CYAN;
	const static Rgba CLEAR;
	const static Rgba FLAT;


public:
	// Construction/Destruction
	~Rgba() = default; // destructor: do nothing (for speed)
	Rgba() = default; // default constructor: do nothing (for speed)
	Rgba(const Rgba& copy) = default; // copy constructor (from another Rgba)
	explicit Rgba(float initial_r, float initial_g, float initial_b, float initial_a = 1.0f);
	explicit Rgba(const Vec3& vec);
	explicit Rgba(const std::string& text);
	explicit Rgba(unsigned int hex);
	// explicit constructor (from r, g, b, a)
	std::string GetAsString() const;

	// Mutator
	void SetFromText(const char* text); // Parses “255,120,0” or “255,120,0,255”  to (r,g,b,a)
	void SetFromHex(const std::string& hex_string); // Parse "0xrrggbb" to (r,g,b,a), be
	void SetFromByte(unsigned char red_byte, unsigned char green_byte, unsigned char blue_byte,
	                 unsigned char alpha_byte = 255); // Parses “255,120,0” or “255,120,0,255”  to (r,g,b,a)

	//overload
	bool operator==(const Rgba& compare) const;
	const Rgba operator*(float uniform_scale) const; // Rgba * float

public: // NOTE: this is one of the few cases where we break both the "m_" naming rule AND the avoid-public-members rule
	float r = 1.0f;
	float g = 1.0f;
	float b = 1.0f;
	float a = 1.0f;
};
