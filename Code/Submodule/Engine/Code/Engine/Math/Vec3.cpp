#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

const STATIC Vec3 Vec3::ZERO(0.f, 0.f, 0.f);
const STATIC Vec3 Vec3::ONE(1.f, 1.f, 1.f);
const STATIC Vec3 Vec3::FORWARD(1.f, 0.f, 0.f);

//-----------------------------------------------------------------------------------------------
Vec3::Vec3(const Vec3& copy) = default;


//-----------------------------------------------------------------------------------------------
Vec3::Vec3(const float initial_x, const float initial_y, const float initial_z)
	: x(initial_x)
	  , y(initial_y)
	  , z(initial_z)
{
}

Vec3::Vec3(int initial_x, int initial_y, int initial_z)
{
	x = static_cast<float>(initial_x);
	y = static_cast<float>(initial_y);
	z = static_cast<float>(initial_z);
}

Vec3::Vec3(const Vec2& copy)
{
	x = copy.x;
	y = copy.y;
	z = 0.f;
}

Vec3::Vec3(const Vec2& initial_xy, float initial_z)
{
	x = initial_xy.x;
	y = initial_xy.y;
	z = initial_z;
}

Vec3::Vec3(const IntVec2& copy)
{
	x = static_cast<float>(copy.x);
	y = static_cast<float>(copy.y);
	z = 0.0f;
}

Vec3::Vec3(const Rgba& color)
{
	x = color.r;
	y = color.g;
	z = color.b;
}

Vec3::Vec3(const std::string& text)
{
	SetFromText(text.c_str());
}

Vec3::Vec3(const Vec4& vec4)
{
	x = vec4.x;
	y = vec4.y;
	z = vec4.z;
}

STATIC const Vec3 Vec3::MakeFromSphericalDegrees(const float yaw_degrees, const float pitch_degrees, const float length)
{
	return Vec3(CosDegrees(pitch_degrees) * CosDegrees(yaw_degrees), 
		SinDegrees(pitch_degrees), 
		CosDegrees(pitch_degrees)*SinDegrees(yaw_degrees)) * length ;
}

STATIC const Vec3 Vec3::MakeFromPSphericalRadians(const float yaw_radians, const float pitch_radians, const float length)
{
	return  MakeFromSphericalDegrees(ConvertRadiansToDegrees(yaw_radians), ConvertRadiansToDegrees(pitch_radians), length);
}

//-----------------------------------------------------------------------------------------------
float Vec3::GetLength() const
{
	return Sqrt(x * x + y * y + z * z);
}

//-----------------------------------------------------------------------------------------------
float Vec3::GetLengthXY() const
{
	return Sqrt(x * x + y * y);
}

//-----------------------------------------------------------------------------------------------
float Vec3::GetLengthSquared() const
{
	return x * x + y * y + z * z;
}

//-----------------------------------------------------------------------------------------------
float Vec3::GetLengthXYSquared() const
{
	return x * x + y * y;
}

float Vec3::GetAngleAboutXDegrees() const
{
	return Atan2Degrees(y, z);
}

float Vec3::GetAngleAboutYDegrees() const
{
	return Atan2Degrees(z, x);
}

//-----------------------------------------------------------------------------------------------
float Vec3::GetAngleAboutZDegrees() const
{
	return Atan2Degrees(y, x);
}

//-----------------------------------------------------------------------------------------------
float Vec3::GetAngleAboutZRadians() const
{
	return ConvertDegreesToRadians(GetAngleAboutZDegrees());
}

//-----------------------------------------------------------------------------------------------
const Vec3 Vec3::GetRotatedAboutZDegrees(const float degrees_to_rotate_about_z) const
{
	const Vec3 new_vec = Vec3(x * (CosDegrees(degrees_to_rotate_about_z))
	                          - y * (SinDegrees(degrees_to_rotate_about_z)),
	                          x * (SinDegrees(degrees_to_rotate_about_z))
	                          + y * (CosDegrees(degrees_to_rotate_about_z)), z);
	return new_vec;
}

//-----------------------------------------------------------------------------------------------
const Vec3 Vec3::GetRotatedAboutZRadians(const float radians_to_rotate_about_z) const
{
	const Vec3 new_vec = GetRotatedAboutZDegrees(ConvertRadiansToDegrees(radians_to_rotate_about_z));
	return new_vec;
}

//-----------------------------------------------------------------------------------------------
const Vec3 Vec3::operator +(const Vec3& vec_to_add) const
{
	return Vec3(
		x + vec_to_add.x,
		y + vec_to_add.y,
		z + vec_to_add.z
	);
}

//-----------------------------------------------------------------------------------------------
const Vec3 Vec3::operator-(const Vec3& vec_to_subtract) const
{
	return Vec3(
		x - vec_to_subtract.x,
		y - vec_to_subtract.y,
		z - vec_to_subtract.z
	);
}


//-----------------------------------------------------------------------------------------------
const Vec3 Vec3::operator*(const float uniform_scale) const
{
	return Vec3(
		x * uniform_scale,
		y * uniform_scale,
		z * uniform_scale
	);
}


//-----------------------------------------------------------------------------------------------
const Vec3 Vec3::operator/(const float inverse_scale) const
{
	return Vec3(
		x / inverse_scale,
		y / inverse_scale,
		z / inverse_scale
	);
}


//-----------------------------------------------------------------------------------------------
void Vec3::operator+=(const Vec3& vec_to_add)
{
	x += vec_to_add.x;
	y += vec_to_add.y;
	z += vec_to_add.z;
}


//-----------------------------------------------------------------------------------------------
void Vec3::operator-=(const Vec3& vec_to_subtract)
{
	x -= vec_to_subtract.x;
	y -= vec_to_subtract.y;
	z -= vec_to_subtract.z;
}


//-----------------------------------------------------------------------------------------------
void Vec3::operator*=(const float uniform_scale)
{
	x *= uniform_scale;
	y *= uniform_scale;
	z *= uniform_scale;
}


//-----------------------------------------------------------------------------------------------
void Vec3::operator/=(const float uniform_divisor)
{
	x /= uniform_divisor;
	y /= uniform_divisor;
	z /= uniform_divisor;
}


//-----------------------------------------------------------------------------------------------
void Vec3::operator=(const Vec3& copy_from)
{
	x = copy_from.x;
	y = copy_from.y;
	z = copy_from.z;
}


//-----------------------------------------------------------------------------------------------
const Vec3 operator*(const float uniform_scale, const Vec3& vec_to_scale)
{
	return Vec3(
		vec_to_scale.x * uniform_scale,
		vec_to_scale.y * uniform_scale,
		vec_to_scale.z * uniform_scale
	);
}


//-----------------------------------------------------------------------------------------------
bool Vec3::operator==(const Vec3& compare) const
{
	return (
		x == compare.x &&
		y == compare.y &&
		z == compare.z
	);
}


//-----------------------------------------------------------------------------------------------
bool Vec3::operator!=(const Vec3& compare) const
{
	return (
		x != compare.x ||
		y != compare.y ||
		z != compare.z
	);
}

std::string Vec3::GetAsString() const
{
	return Stringf("%g,%g,%g", x, y, z);
}

const Vec3 Vec3::GetNormalize() const
{
	Vec3 temp_vec(x, y, z);
	const float lengthsq = temp_vec.x*temp_vec.x + temp_vec.y*temp_vec.y + temp_vec.z*temp_vec.z;
	const bool IsZero = lengthsq == 0.0f;

	//ASSERT_RECOVERABLE(!IsZero, "attempting to normalize a zero vector");
	if(IsZero)
	{
		temp_vec.x = temp_vec.y = temp_vec.z = 0.0f;
		return temp_vec;
	}

	float recip = RecipSqrt( lengthsq );
	temp_vec.x *= recip;
	temp_vec.y *= recip;
	temp_vec.z *= recip;

	const Vec3 norm(temp_vec.x, temp_vec.y, temp_vec.z);
	return norm;
}

void Vec3::SetFromText(const char* text, char delimiter)
{
	std::vector<std::string> split_words = SplitStringOnDelimiter(text, delimiter);

	if(split_words[0] == "")
	{
		split_words.erase(split_words.begin());
	}

	if(split_words[split_words.size()-1] == "")
	{
		split_words.erase(split_words.end());
	}

	ASSERT_OR_DIE(split_words.size() >= 3, Stringf("Expecting a Vec3 from the following line, %s", text));

	x = static_cast<float>(atof(split_words[0].c_str()));
	y = static_cast<float>(atof(split_words[1].c_str()));
	z = static_cast<float>(atof(split_words[2].c_str()));
}

void Vec3::Normalize()
{
	const float lengthsq = x*x + y*y + z*z;
	const bool IsZero = lengthsq == 0.0f;

	//ASSERT_RECOVERABLE(!IsZero, "attempting to normalize a zero vector");
	if(IsZero)
	{
		x = y = z = 0.0f;
		return;
	}

	float recip = RecipSqrt( lengthsq );
	x *= recip;
	y *= recip;
	z *= recip;
}
