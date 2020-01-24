#pragma once
#include "Engine/EngineCommon.hpp"
#include "Engine/Math/Vec3.hpp"


struct Sphere
{
public:
	Vec3 point;
	float radius;

	Sphere();
	explicit Sphere(const Vec3& world_pos, float radius);
	~Sphere();
};