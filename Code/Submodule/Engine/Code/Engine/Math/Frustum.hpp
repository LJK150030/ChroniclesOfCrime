#pragma once
#include "Engine/Math/Plane3.hpp"

struct Vec3;
struct Ray3;
struct Sphere;
struct Capsule3;

struct Frustum
{
public:
	Plane3 m_top;
	Plane3 m_bottom;
	Plane3 m_left;
	Plane3 m_right;
	Plane3 m_near;
	Plane3 m_far;

public:
	Frustum();
	~Frustum();

	void GenerateFromCorners( const Vec3& left_bottom_front, const Vec3& right_bottom_front,
		const Vec3& left_top_front, const Vec3& right_top_front, const Vec3& left_bottom_back,
		const Vec3& right_bottom_back, const Vec3& left_top_back, const Vec3& right_top_back);

	float IntersectionAt(const Ray3& ray) const;
	bool IntersectionTest(const Vec3& point) const;
	bool IntersectionTest(const Sphere& sphere) const;
	bool IntersectionTest(const Capsule3& capsule) const;
};


