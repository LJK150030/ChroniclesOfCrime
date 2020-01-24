#include "Engine/Math/Sphere.hpp"

Sphere::Sphere(): point(Vec3::ZERO), radius(0)
{
}

Sphere::Sphere(const Vec3& world_pos, const float radius) : point(world_pos), radius(radius)
{
}

Sphere::~Sphere() = default;
