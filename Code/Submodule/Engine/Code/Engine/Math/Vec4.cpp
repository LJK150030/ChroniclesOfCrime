#include "Engine/Math/Vec4.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

const STATIC Vec4 Vec4::ZERO(0.f, 0.f, 0.0f, 0.0f);
const STATIC Vec4 Vec4::ONE(1.f, 1.f, 1.0f, 1.0f);


Vec4::Vec4(const Vec4& copy): x(copy.x), y(copy.y), z(copy.z), w(copy.w)
{
}

Vec4::Vec4(const float initial_x, const float initial_y, const float initial_z, const float initial_w): x(initial_x),
                                                                                                        y(initial_y),
                                                                                                        z(initial_z),
                                                                                                        w(initial_w)
{
}

float Vec4::GetLength() const
{
	return Sqrt(x * x + y * y + z * z + w * w);
}

float Vec4::GetLengthXY() const
{
	return Sqrt(x * x + y * y);
}

float Vec4::GetLengthSquared() const
{
	return x * x + y * y + z * z + w * w;
}

float Vec4::GetLengthXYSquared() const
{
	return x * x + y * y;
}

Vec4 Vec4::GetNormalize() const
{
	Vec4 temp_vec(x, y, z, w);
	const float lengthsq = temp_vec.x*temp_vec.x + temp_vec.y*temp_vec.y + temp_vec.z*temp_vec.z + temp_vec.w*temp_vec.w;
	const bool IsZero = (lengthsq == 0.0f);

	//ASSERT_RECOVERABLE(!IsZero, "attempting to normalize a zero vector");
	if(IsZero)
	{
		temp_vec.x = temp_vec.y = temp_vec.z = temp_vec.w = 0.0f;
		return temp_vec;
	}

	float recip = RecipSqrt( lengthsq );
	temp_vec.x *= recip;
	temp_vec.y *= recip;
	temp_vec.z *= recip;
	temp_vec.w *= recip;

	const Vec4 norm(temp_vec.x, temp_vec.y, temp_vec.z, temp_vec.w);
	return norm;
}

float Vec4::GetAngleAboutZDegrees() const
{
	return Atan2Degrees(y, x);
}

float Vec4::GetAngleAboutZRadians() const
{
	return ConvertDegreesToRadians(GetAngleAboutZDegrees());
}

const Vec4 Vec4::GetRotatedAboutZDegrees(const float degrees_to_rotate_about_z) const
{
	const Vec4 new_vec = Vec4((x * CosDegrees(degrees_to_rotate_about_z)) - (y * SinDegrees(degrees_to_rotate_about_z)),
	                          (x * (SinDegrees(degrees_to_rotate_about_z))) + (y * (CosDegrees(
		                          degrees_to_rotate_about_z))),
	                          z, w);
	return new_vec;
}

const Vec4 Vec4::GetRotatedAboutZRadians(const float radians_to_rotate_about_z) const
{
	const Vec4 new_vec = GetRotatedAboutZDegrees(ConvertRadiansToDegrees(radians_to_rotate_about_z));
	return new_vec;
}

std::string Vec4::GetAsString() const
{
	return Stringf("%g,%g,%g,%g", x, y, z, w);
}

void Vec4::SetFromText(const char* text)
{
	std::vector<std::string> split_words = SplitStringOnDelimiter(text, ',');

	ASSERT_OR_DIE(split_words.size() == 4, Stringf("Expecting a Vec4 from the following line, %s", text));

	x = static_cast<float>(atof(split_words[0].c_str()));
	y = static_cast<float>(atof(split_words[1].c_str()));
	z = static_cast<float>(atof(split_words[2].c_str()));
	w = static_cast<float>(atof(split_words[3].c_str()));
}

void Vec4::Normalize()
{
	const float lengthsq = x*x + y*y + z*z + w*w;
	const bool IsZero = lengthsq == 0.0f;

	//ASSERT_RECOVERABLE(!IsZero, "attempting to normalize a zero vector");
	if(IsZero)
	{
		x = y = z = w = 0.0f;
		return;
	}

	float recip = RecipSqrt( lengthsq );
	x *= recip;
	y *= recip;
	z *= recip;
	w *= recip;
}

const Vec4 Vec4::operator+(const Vec4& vec_to_add) const
{
	return Vec4(
		x + vec_to_add.x,
		y + vec_to_add.y,
		z + vec_to_add.z,
		w + vec_to_add.w
	);
}

const Vec4 Vec4::operator-(const Vec4& vec_to_subtract) const
{
	return Vec4(
		x - vec_to_subtract.x,
		y - vec_to_subtract.y,
		z - vec_to_subtract.z,
		w - vec_to_subtract.w
	);
}

const Vec4 Vec4::operator*(const float uniform_scale) const
{
	return Vec4(
		x * uniform_scale,
		y * uniform_scale,
		z * uniform_scale,
		w * uniform_scale
	);
}

const Vec4 Vec4::operator/(const float inverse_scale) const
{
	return Vec4(
		x / inverse_scale,
		y / inverse_scale,
		z / inverse_scale,
		w / inverse_scale
	);
}

void Vec4::operator+=(const Vec4& vec_to_add)
{
	x += vec_to_add.x;
	y += vec_to_add.y;
	z += vec_to_add.z;
	w += vec_to_add.w;
}

void Vec4::operator-=(const Vec4& vec_to_subtract)
{
	x -= vec_to_subtract.x;
	y -= vec_to_subtract.y;
	z -= vec_to_subtract.z;
	w -= vec_to_subtract.w;
}

void Vec4::operator*=(const float uniform_scale)
{
	x *= uniform_scale;
	y *= uniform_scale;
	z *= uniform_scale;
	w *= uniform_scale;
}

void Vec4::operator/=(const float uniform_divisor)
{
	x /= uniform_divisor;
	y /= uniform_divisor;
	z /= uniform_divisor;
	w /= uniform_divisor;
}

void Vec4::operator=(const Vec4& copy_from)
{
	x = copy_from.x;
	y = copy_from.y;
	z = copy_from.z;
	w = copy_from.w;
}

bool Vec4::operator==(const Vec4& compare) const
{
	return (
		x == compare.x &&
		y == compare.y &&
		z == compare.z &&
		w == compare.w
	);
}

bool Vec4::operator!=(const Vec4& compare) const
{
	return (
		x != compare.x ||
		y != compare.y ||
		z != compare.z ||
		w != compare.w
	);
}

const Vec4 operator*(const float uniform_scale, const Vec4& vec_to_scale)
{
	return Vec4(
		vec_to_scale.x * uniform_scale,
		vec_to_scale.y * uniform_scale,
		vec_to_scale.z * uniform_scale,
		vec_to_scale.w * uniform_scale
	);
}
