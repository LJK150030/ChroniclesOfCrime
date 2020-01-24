#pragma once
#include "Engine/Math/Vec3.hpp"

struct Capsule3
{
public:
	Vec3 start;
	Vec3 end; 
	float radius;

	Capsule3();
	explicit Capsule3(const Vec3& world_start, const Vec3& world_end, float radius);
	~Capsule3();

	Vec3 GetDirection() const;
	float GetLength() const;
	float GetLengthSquared() const;
};
