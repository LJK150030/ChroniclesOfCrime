#include "Engine/Math/Capsule3.hpp"

Capsule3::Capsule3(): start(Vec3::ZERO), end(Vec3::ONE), radius(0)
{
}

Capsule3::Capsule3(const Vec3& world_start, const Vec3& world_end, float radius): start(world_start), end(world_end), radius(radius)
{
}

Capsule3::~Capsule3() = default;

Vec3 Capsule3::GetDirection() const
{
	Vec3 direction = end - start;
	return direction;
}

float Capsule3::GetLength() const
{
	Vec3 direction = end - start;
	return direction.GetLength();
}

float Capsule3::GetLengthSquared() const
{
	Vec3 direction = end - start;
	return direction.GetLengthSquared();
}
