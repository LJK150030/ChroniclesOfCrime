#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"

struct AABB2;

struct AABB3
{
public:
	Vec3 mins;
	Vec3 maxs;
	Vec3 center;

	AABB3(): mins(Vec3(0.0f, 0.0f, 0.0f)), maxs(Vec3(1.0f, 1.0f, 1.0f)), center(Vec3(0.5f, 0.5f, 0.5f)) {}

	AABB3(const Vec3& initial_min, const Vec3& initial_max);
	AABB3(float initial_min_x, float initial_min_y, float initial_min_z, float initial_max_x, float initial_max_y, float initial_max_z);
	AABB3(const AABB2& clone, const Vec2& z_min_max = Vec2::ZERO);
	~AABB3() = default;

	void Translate(float uniform_scale, float z_rotation_degrees, const Vec2& direction);
	void Translate( const Vec3& translation );

	Vec3 GetCenter() const;

	Vec3 GetTopLeftFrontCorner() const;
	Vec3 GetTopRightFrontCorner() const;
	Vec3 GetBottomLeftFrontCorner() const;
	Vec3 GetBottomRightFrontCorner() const;
	Vec3 GetTopLeftBackCorner() const;
	Vec3 GetTopRightBackCorner() const;
	Vec3 GetBottomLeftBackCorner() const;
	Vec3 GetBottomRightBackCorner() const;
};
