#include "Engine/Math/IntVec3.hpp"
#include "Engine/EngineCommon.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

const STATIC IntVec3 IntVec3::ZERO(0, 0, 0);
const STATIC IntVec3 IntVec3::ONE(1, 1, 1);

//-----------------------------------------------------------------------------------------------
IntVec3::IntVec3(const IntVec3& copy) = default;;

//-----------------------------------------------------------------------------------------------
IntVec3::IntVec3(const int initial_x, const int initial_y, const int initial_z): x(initial_x), y(initial_y), z(initial_z)
{
}

//-----------------------------------------------------------------------------------------------
int IntVec3::GetLengthSquared() const
{
	return x * x + y * y;
}

int IntVec3::GetArea() const
{
	return x * y;
}

std::string IntVec3::GetAsString() const
{
	return Stringf("%i,%i,%i", x, y, z);
}

void IntVec3::SetFromText(const char* text, char delimiter)
{
	std::vector<std::string> splitWords = SplitStringOnDelimiter(text, delimiter);
	bool handled = false;

	if(splitWords.size() == 1)
	{
		x = atoi(splitWords[0].c_str());
		y = atoi(splitWords[0].c_str());
		z = atoi(splitWords[0].c_str());
		handled = true;
	}
	else if((splitWords.size() == 3))
	{
		x = atoi(splitWords[0].c_str());
		y = atoi(splitWords[1].c_str());
		z = atoi(splitWords[2].c_str());
		handled = true;
	}
	
	ASSERT_OR_DIE(handled, Stringf("Expecting a IntVec3 from the following line, %s", text));
}

//-----------------------------------------------------------------------------------------------
const IntVec3 IntVec3::operator +(const IntVec3& vec_to_add) const
{
	return IntVec3(
		x + vec_to_add.x,
		y + vec_to_add.y,
		z + vec_to_add.z
	);
}

//-----------------------------------------------------------------------------------------------
const IntVec3 IntVec3::operator-(const IntVec3& vec_to_subtract) const
{
	return IntVec3(
		x - vec_to_subtract.x,
		y - vec_to_subtract.y,
		z - vec_to_subtract.z
	);
}


//-----------------------------------------------------------------------------------------------
const IntVec3 IntVec3::operator*(const int uniform_scale) const
{
	return IntVec3(
		x * uniform_scale,
		y * uniform_scale,
		z * uniform_scale
	);
}


//-----------------------------------------------------------------------------------------------
const IntVec3 IntVec3::operator/(const int inverse_scale) const
{
	return IntVec3(
		x / inverse_scale,
		y / inverse_scale,
		z / inverse_scale

	);
}


//-----------------------------------------------------------------------------------------------
void IntVec3::operator+=(const IntVec3& vec_to_add)
{
	x += vec_to_add.x;
	y += vec_to_add.y;
	z += vec_to_add.z;
}


//-----------------------------------------------------------------------------------------------
void IntVec3::operator-=(const IntVec3& vec_to_subtract)
{
	x -= vec_to_subtract.x;
	y -= vec_to_subtract.y;
	z -= vec_to_subtract.z;
}


//-----------------------------------------------------------------------------------------------
void IntVec3::operator*=(const int uniform_scale)
{
	x *= uniform_scale;
	y *= uniform_scale;
	z *= uniform_scale;
}


//-----------------------------------------------------------------------------------------------
void IntVec3::operator/=(const int uniform_divisor)
{
	x /= uniform_divisor;
	y /= uniform_divisor;
}


//-----------------------------------------------------------------------------------------------
void IntVec3::operator=(const IntVec3& copy_from)
{
	x = copy_from.x;
	y = copy_from.y;
	z = copy_from.z;
}


//-----------------------------------------------------------------------------------------------
const IntVec3 operator*(const int uniform_scale, const IntVec3& vec_to_scale)
{
	return IntVec3(
		vec_to_scale.x * uniform_scale,
		vec_to_scale.y * uniform_scale,
		vec_to_scale.z * uniform_scale

	);
}


//-----------------------------------------------------------------------------------------------
bool IntVec3::operator==(const IntVec3& compare) const
{
	return (
		x == compare.x &&
		y == compare.y &&
		z == compare.z
		);
}


//-----------------------------------------------------------------------------------------------
bool IntVec3::operator!=(const IntVec3& compare) const
{
	return (
		x != compare.x ||
		y != compare.y ||
		z != compare.z
		);
}
