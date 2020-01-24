#pragma once
#include <string>


//-----------------------------------------------------------------------------------------------
struct IntVec3
{
	const static IntVec3 ZERO;
	const static IntVec3 ONE;

public:
	// Construction/Destruction
	~IntVec3() = default; // destructor: do nothing (for speed)
	IntVec3(): x(0), y(0), z(0) {} // default constructor: do nothing (for speed)
	IntVec3(const IntVec3& copy); // copy constructor (from another vec2)
	explicit IntVec3(int initial_x, int initial_y, int initial_z); // explicit constructor (from x, y)

													// Accessors
													//int GetLength() const;
	int GetLengthSquared() const;
	int GetArea() const;
	std::string GetAsString() const;

	// Mutator
	void SetFromText(const char* text, char delimiter = ','); // Parses “6,4” to (x,y)

														// Operators
	const IntVec3 operator+(const IntVec3& vec_to_add) const; // vec2 + vec2
	const IntVec3 operator-(const IntVec3& vec_to_subtract) const; // vec2 - vec2
	const IntVec3 operator*(int uniform_scale) const; // vec2 * int
	const IntVec3 operator/(int inverse_scale) const; // vec2 / int
	void operator+=(const IntVec3& vec_to_add); // vec2 += vec2
	void operator-=(const IntVec3& vec_to_subtract); // vec2 -= vec2
	void operator*=(int uniform_scale); // vec2 *= int
	void operator/=(int uniform_divisor); // vec2 /= int
	void operator=(const IntVec3& copy_from); // vec2 = vec2
	bool operator==(const IntVec3& compare) const; // vec2 == vec2
	bool operator!=(const IntVec3& compare) const; // vec2 != vec2

	friend const IntVec3 operator*(int uniform_scale, const IntVec3& vec_to_scale); // int * vec3

public: // NOTE: this is one of the few cases where we break both the "m_" naming rule AND the avoid-public-members rule
	int x;
	int y;
	int z;
};
