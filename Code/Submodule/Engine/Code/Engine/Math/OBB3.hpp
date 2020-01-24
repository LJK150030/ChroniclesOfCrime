#pragma once
#include "Engine/Math/Vec3.hpp"
#include "AABB3.hpp"

struct Matrix44;

struct OBB3
{
	OBB3();
	~OBB3();
	explicit OBB3( const Vec3& right, const Vec3& up, const Vec3& position, const Vec3& half_extents);
	explicit OBB3( const Vec3& forward, const Vec3& position, const Vec3& half_extents);

	Vec3 ToLocalPoint(const Vec3& world_point) const;
	Vec3 ToWorldPoint(const Vec3& local_point) const;
	Vec3 ToLocalVector(const Vec3& world_vector) const;
	Vec3 ToWorldVector(const Vec3& local_vector) const;
	AABB3 GetLocalAABB3() const;

	Vec3 GetTopLeftFrontCorner() const;
	Vec3 GetTopRightFrontCorner() const;
	Vec3 GetBottomLeftFrontCorner() const;
	Vec3 GetBottomRightFrontCorner() const;
	Vec3 GetTopLeftBackCorner() const;
	Vec3 GetTopRightBackCorner() const;
	Vec3 GetBottomLeftBackCorner() const;
	Vec3 GetBottomRightBackCorner() const;
	Vec3 GetDimensions() const;

	void Translate( const Vec3& translation );
	void Rotate( const Matrix44& rotation_matrix );

	Vec3 m_right         = Vec3( 1.0f, 0.0f, 0.0f ); 
	Vec3 m_up            = Vec3( 0.0f, 1.0f, 0.0f );   // not *needed* - Squirrel doesn't use it since you can easily derive it from "right"
	Vec3 m_forward       = Vec3( 0.0f, 0.0f, 1.0f );   // not *needed* - Squirrel doesn't use it since you can easily derive it from "right"
	Vec3 m_center        = Vec3( 0.0f, 0.0f, 0.0f ); 
	Vec3 m_halfExtents   = Vec3( 0.0f, 0.0f, 0.0f );
};
