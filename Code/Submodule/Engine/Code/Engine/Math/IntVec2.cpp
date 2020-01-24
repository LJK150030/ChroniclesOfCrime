#include "Engine/Math/IntVec2.hpp"
#include "Engine/EngineCommon.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

const STATIC IntVec2 IntVec2::ZERO(0, 0);
const STATIC IntVec2 IntVec2::ONE(1, 1);

//-----------------------------------------------------------------------------------------------
IntVec2::IntVec2(const IntVec2& copy) = default;

IntVec2::IntVec2(const Vec2& copy)
{
	x = static_cast<int>(copy.x);
	y = static_cast<int>(copy.y);
}

IntVec2::IntVec2(const std::string& text)
{
	SetFromText(text.c_str());
};

//-----------------------------------------------------------------------------------------------
IntVec2::IntVec2(const int initial_x, const int initial_y): x(initial_x), y(initial_y)
{
}

//-----------------------------------------------------------------------------------------------
int IntVec2::GetLengthSquared() const
{
	return x * x + y * y;
}

int IntVec2::GetArea() const
{
	return x * y;
}

//-----------------------------------------------------------------------------------------------
const IntVec2 IntVec2::GetRotated90Degrees() const
{
	const IntVec2 new_vec = IntVec2(-y, x);
	return new_vec;
}

//-----------------------------------------------------------------------------------------------
const IntVec2 IntVec2::GetRotatedMinus90Degrees() const
{
	const IntVec2 new_vec = IntVec2(y, -x);
	return new_vec;
}

std::string IntVec2::GetAsString() const
{
	return Stringf("%i,%i", x, y);
}

void IntVec2::SetFromText(const char* text, char delimiter)
{
	std::vector<std::string> splitWords = SplitStringOnDelimiter(text, delimiter);

	ASSERT_OR_DIE(splitWords.size() == 2, Stringf("Expecting a IntVec2 from the following line, %s", text));

	x = atoi(splitWords[0].c_str());
	y = atoi(splitWords[1].c_str());
}

//-----------------------------------------------------------------------------------------------
const IntVec2 IntVec2::operator +(const IntVec2& vec_to_add) const
{
	return IntVec2(
		x + vec_to_add.x,
		y + vec_to_add.y
	);
}

//-----------------------------------------------------------------------------------------------
const IntVec2 IntVec2::operator-(const IntVec2& vec_to_subtract) const
{
	return IntVec2(
		x - vec_to_subtract.x,
		y - vec_to_subtract.y
	);
}


//-----------------------------------------------------------------------------------------------
const IntVec2 IntVec2::operator*(const int uniform_scale) const
{
	return IntVec2(
		x * uniform_scale,
		y * uniform_scale
	);
}


//-----------------------------------------------------------------------------------------------
const IntVec2 IntVec2::operator/(const int inverse_scale) const
{
	return IntVec2(
		x / inverse_scale,
		y / inverse_scale
	);
}


//-----------------------------------------------------------------------------------------------
void IntVec2::operator+=(const IntVec2& vec_to_add)
{
	x += vec_to_add.x;
	y += vec_to_add.y;
}


//-----------------------------------------------------------------------------------------------
void IntVec2::operator-=(const IntVec2& vec_to_subtract)
{
	x -= vec_to_subtract.x;
	y -= vec_to_subtract.y;
}


//-----------------------------------------------------------------------------------------------
void IntVec2::operator*=(const int uniform_scale)
{
	x *= uniform_scale;
	y *= uniform_scale;
}


//-----------------------------------------------------------------------------------------------
void IntVec2::operator/=(const int uniform_divisor)
{
	x /= uniform_divisor;
	y /= uniform_divisor;
}


//-----------------------------------------------------------------------------------------------
void IntVec2::operator=(const IntVec2& copy_from)
{
	x = copy_from.x;
	y = copy_from.y;
}


//-----------------------------------------------------------------------------------------------
const IntVec2 operator*(const int uniform_scale, const IntVec2& vec_to_scale)
{
	return IntVec2(
		vec_to_scale.x * uniform_scale,
		vec_to_scale.y * uniform_scale
	);
}


//-----------------------------------------------------------------------------------------------
bool IntVec2::operator==(const IntVec2& compare) const
{
	return (
		x == compare.x &&
		y == compare.y
	);
}


//-----------------------------------------------------------------------------------------------
bool IntVec2::operator!=(const IntVec2& compare) const
{
	return (
		x != compare.x ||
		y != compare.y
	);
}

