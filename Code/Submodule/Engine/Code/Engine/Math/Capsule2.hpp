#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/OBB2.hpp"

struct Capsule2
{
public:
	Vec2 m_p0;
	Vec2 m_p1;
	float m_radius;

public:
	Capsule2();
	Capsule2( const Vec2& pos ); // a point
	Capsule2( const Vec2& center, float radius );
	explicit Capsule2(const Vec2& start, const Vec2& end, float radius);
	explicit Capsule2(float start_x, float start_y, float end_x, float end_y, float radius);
	~Capsule2();

	void SetPosition( Vec2 pos ); 
	void SetPositions( Vec2 start, Vec2 end); 
	void Transform(float uniform_scale, float z_rotation_degrees, const Vec2& direction);
	void Translate(const Vec2& direction);
	void RotateBy( float degrees );

	Vec2 GetClosestPoint(const Vec2& reference_pos);
	bool IsPointInside(const Vec2& reference_pos);
	bool IsOverlappingWithLineSegment2D(const Segment2& line);


	Vec2 GetCenter() const;
	float GetRotation() const;
	AABB2 GetBoundingAABB() const;
	void GetBoundingDisc(Vec2& center, float& radius) const;
	OBB2 GetOBB() const;
};
