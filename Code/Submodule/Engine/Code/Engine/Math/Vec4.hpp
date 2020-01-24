#pragma once
#include <string>


//-----------------------------------------------------------------------------------------------
struct Vec4
{
public:
	const static Vec4 ZERO;
	const static Vec4 ONE;
	
public:
	// Construction/Destruction
	~Vec4() = default; // destructor: do nothing (for speed)

	Vec4(): x(0.f), y(0.f), z(0.f), w(0.f)
	{
	} // default constructor: do nothing (for speed)
	Vec4(const Vec4& copy); // copy constructor (from another Vec3)
	explicit Vec4(float initial_x, float initial_y, float initial_z, float initial_w);
	// explicit constructor (from x, y, z)

	// Accessors
	float GetLength() const;
	float GetLengthXY() const;
	float GetLengthSquared() const;
	float GetLengthXYSquared() const;
	Vec4 GetNormalize() const;
	float GetAngleAboutZDegrees() const;
	float GetAngleAboutZRadians() const;
	const Vec4 GetRotatedAboutZDegrees(float degrees_to_rotate_about_z) const;
	const Vec4 GetRotatedAboutZRadians(float radians_to_rotate_about_z) const;
	std::string GetAsString() const;

	// Mutator
	void SetFromText(const char* text); // Parses “6,4,5” or “ -.3 , 0.05 , 5.5” to (x,y,z)
	void Normalize();

	// Operators
	const Vec4 operator+(const Vec4& vec_to_add) const; // Vec3 + Vec3
	const Vec4 operator-(const Vec4& vec_to_subtract) const; // Vec3 - Vec3
	const Vec4 operator*(float uniform_scale) const; // Vec3 * float
	const Vec4 operator/(float inverse_scale) const; // Vec3 / float
	void operator+=(const Vec4& vec_to_add); // Vec3 += Vec3
	void operator-=(const Vec4& vec_to_subtract); // Vec3 -= Vec3
	void operator*=(float uniform_scale); // Vec3 *= float
	void operator/=(float uniform_divisor); // Vec3 /= float
	void operator=(const Vec4& copy_from); // Vec3 = Vec3
	bool operator==(const Vec4& compare) const; // Vec3 == Vec3
	bool operator!=(const Vec4& compare) const; // Vec3 != Vec3

	friend const Vec4 operator*(float uniform_scale, const Vec4& vec_to_scale); // float * Vec3

public: // NOTE: this is one of the few cases where we break both the "m_" naming rule AND the avoid-public-members rule
	float x;
	float y;
	float z;
	float w;
};
