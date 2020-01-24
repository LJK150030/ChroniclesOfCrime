#pragma once
#include "Engine/EngineCommon.hpp"
#include "Engine/Math/Vec3.hpp"

struct Plane3
{
public:
	Vec3 m_normal;
	float m_signedDistance;

	Plane3();
	explicit Plane3(const Vec3& forward_dir, float distance_from_origin);
	explicit Plane3(const Vec3& point_one, const Vec3& point_two, const Vec3& point_three);
	~Plane3();

	bool IsPointOnPlane( const Vec3& point ) const;
	Vec3 ClosestPointToOrigin() const;
};