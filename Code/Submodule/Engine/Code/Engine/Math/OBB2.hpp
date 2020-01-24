#pragma once
#include "Engine/Math/Vec2.hpp"

struct Segment2;
struct AABB2;

struct OBB2 
{
public:
	OBB2();
	explicit OBB2( const Vec2& center, const Vec2& half_extents = Vec2::ZERO, float rotation_degrees = 0.0f ); 
	explicit OBB2( const AABB2&  aabb ); 

	// Modification Utility
	void Transform(float uniform_scale, float z_rotation_degrees, const Vec2& direction);
	void Translate( const Vec2& offset );
	void MoveTo( const Vec2& position ); 

	void RotateBy( float degrees );           // A04
	void SetRotation( float degrees ); 
	float GetRotation() const;

	void SetSize( const Vec2& size ); 

	// Helpers for describing it; 
	inline Vec2 GetRight() const	{ return m_right; }
	inline Vec2 GetUp() const		{ return m_up; } 
	inline Vec2 GetCenter() const	{ return m_center; }; 
	inline Vec2 GetSize() const		{ return m_halfExtents; };

	inline Vec2 GetBottomLeft() const;
	Vec2 GetBottomRight() const; 
	Vec2 GetTopLeft() const; 
	Vec2 GetTopRight() const;
	AABB2 GetLocalAABB2() const;

	// void GetCorners( Vec2 *out ) const; // tl, tr, bl, br
	//void GetPlanes( Plane2 *out ) const; 

	// Collision Utility
	Vec2 ToLocalPoint( const Vec2& world_point ) const; 
	Vec2 ToWorldPoint( const Vec2& local_point ) const; 

	bool Contains( const Vec2& world_point ) const;
	Vec2 GetClosestPoint( const Vec2& world_point ) const;
	Vec2 GetClosestEdgePoint(const Vec2& reference_pos) const;

	bool IsIntersecting( const OBB2& other ) const;

	bool IsPointInside(const Vec2& reference_pos) const;
	bool IsOverlappingWithLineSegment2D(const Segment2& line);

	// Useful if you want to use AABB2 as a "broadphase" & "midphase" check
	// like checking if something fell outside the world
	AABB2 GetBoundingAABB() const; 
	void GetBoundingDisc(Vec2& center, float& radius) const;

public:
	Vec2 m_right         = Vec2( 1.0f, 0.0f ); 
	Vec2 m_up            = Vec2( 0.0f, 1.0f );   // not *needed* - Squirrel doesn't use it since you can easily derive it from "right"
	Vec2 m_center        = Vec2( 0.0f, 0.0f ); 
	Vec2 m_halfExtents       = Vec2( 0.0f, 0.0f );
}; 