#pragma once
#include <string>
#include "Vec4.hpp"


struct IntVec2;
struct Rgba;
struct Vec2;

//-----------------------------------------------------------------------------------------------
struct Vec3
{
public:
	const static Vec3 ZERO;
	const static Vec3 ONE;
	const static Vec3 FORWARD;

public:
	// Construction/Destruction
	~Vec3() = default; // destructor: do nothing (for speed)
	Vec3(): x(0.f), y(0.f), z(0.f)
	{
	} // default constructor: do nothing (for speed)
	Vec3(const Vec3& copy); // copy constructor (from another Vec3)
	explicit Vec3(float initial_x, float initial_y, float initial_z); // explicit constructor (from x, y, z)
	explicit Vec3(int initial_x, int initial_y, int initial_z); // explicit constructor (from x, y, z)
	explicit Vec3(const Vec2& copy);
	explicit Vec3(const Vec2& initial_xy, float initial_z);
	explicit Vec3(const IntVec2& copy);
	explicit Vec3(const Rgba& color);
	explicit Vec3(const std::string& text);
	explicit Vec3(const Vec4& vec4);

	static const Vec3 MakeFromSphericalDegrees(float yaw_degrees, float pitch_degrees, float length = 1.f);
	static const Vec3 MakeFromPSphericalRadians(float yaw_radians, float pitch_radians, float length = 1.f);

	// Accessors
	float GetLength() const;
	float GetLengthXY() const;
	float GetLengthSquared() const;
	float GetLengthXYSquared() const;
	float GetAngleAboutXDegrees() const;
	float GetAngleAboutYDegrees() const;
	float GetAngleAboutZDegrees() const;
	float GetAngleAboutZRadians() const;
	const Vec3 GetRotatedAboutZDegrees(float degrees_to_rotate_about_z) const;
	const Vec3 GetRotatedAboutZRadians(float radians_to_rotate_about_z) const;
	std::string GetAsString() const;
	const Vec3 GetNormalize() const;

	// Mutator
	void SetFromText(const char* text, char delimiter = ','); // Parses “6,4,5” or “ -.3 , 0.05 , 5.5” to (x,y,z)
	void Normalize();

	// Operators
	const Vec3 operator+(const Vec3& vec_to_add) const; // Vec3 + Vec3
	const Vec3 operator-(const Vec3& vec_to_subtract) const; // Vec3 - Vec3
	const Vec3 operator*(float uniform_scale) const; // Vec3 * float
	const Vec3 operator/(float inverse_scale) const; // Vec3 / float
	void operator+=(const Vec3& vec_to_add); // Vec3 += Vec3
	void operator-=(const Vec3& vec_to_subtract); // Vec3 -= Vec3
	void operator*=(float uniform_scale); // Vec3 *= float
	void operator/=(float uniform_divisor); // Vec3 /= float
	void operator=(const Vec3& copy_from); // Vec3 = Vec3
	bool operator==(const Vec3& compare) const; // Vec3 == Vec3
	bool operator!=(const Vec3& compare) const; // Vec3 != Vec3

	friend const Vec3 operator*(float uniform_scale, const Vec3& vec_to_scale); // float * Vec3

public: // NOTE: this is one of the few cases where we break both the "m_" naming rule AND the avoid-public-members rule
	float x;
	float y;
	float z;
};
