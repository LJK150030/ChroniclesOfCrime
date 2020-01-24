#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/MathUtils.hpp"

AABB3::AABB3(const Vec3& initial_min, const Vec3& initial_max): mins(initial_min), maxs(initial_max)
{
	center = GetCenter();
}

AABB3::AABB3(float initial_min_x, float initial_min_y, float initial_min_z, float initial_max_x, float initial_max_y,
	float initial_max_z)
{
	mins = Vec3(initial_min_x, initial_min_y, initial_min_z);
	maxs = Vec3(initial_max_x, initial_max_y, initial_max_z);
	center = GetCenter();
}

AABB3::AABB3(const AABB2& clone,  const Vec2& z_min_max)
{
	mins = Vec3(clone.mins.x, clone.mins.y, z_min_max.x);
	maxs = Vec3(clone.maxs.x, clone.maxs.y, z_min_max.y);
	center = GetCenter();
}

void AABB3::Translate(const float uniform_scale, const float z_rotation_degrees, const Vec2& direction)
{
	mins = TransformPosition(mins, uniform_scale, z_rotation_degrees, direction);
	maxs = TransformPosition(maxs, uniform_scale, z_rotation_degrees, direction);
}


void AABB3::Translate( const Vec3& translation )
{
	mins += translation;
	maxs += translation;
}


Vec3 AABB3::GetCenter() const
{
	const float x_mid = mins.x + (maxs.x - mins.x) * 0.5f;
	const float y_mid = mins.y + (maxs.y - mins.y) * 0.5f;
	const float z_mid = mins.z + (maxs.z - mins.z) * 0.5f;

	return Vec3(x_mid, y_mid, z_mid);
}

Vec3 AABB3::GetTopLeftFrontCorner() const
{
	return Vec3(mins.x, maxs.y, mins.z);
}

Vec3 AABB3::GetTopRightFrontCorner() const
{
	return Vec3(maxs.x, maxs.y, mins.z);
}

Vec3 AABB3::GetBottomLeftFrontCorner() const
{
	return Vec3(mins.x, mins.y, mins.z);
}

Vec3 AABB3::GetBottomRightFrontCorner() const
{
	return Vec3(maxs.x, mins.y, mins.z);
}

Vec3 AABB3::GetTopLeftBackCorner() const
{
	return Vec3(mins.x, maxs.y, maxs.z);
}

Vec3 AABB3::GetTopRightBackCorner() const
{
	return Vec3(maxs.x, maxs.y, maxs.z);
}

Vec3 AABB3::GetBottomLeftBackCorner() const
{
	return Vec3(mins.x, mins.y, maxs.z);
}

Vec3 AABB3::GetBottomRightBackCorner() const
{
	return Vec3(maxs.x, mins.y, maxs.z);
}
