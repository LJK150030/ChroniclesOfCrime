#pragma once
#include "Engine//Math/Vec2.hpp"
#include "Engine/Math/AABB2.hpp"

struct Segment2 
{
public:
	Segment2();
	explicit Segment2( const Vec2& start, const Vec2& end );
	~Segment2();

	inline Vec2 const& GetStart() const        { return m_start; }
	inline Vec2 const& GetEnd() const          { return m_end; }
	Vec2 GetCenter() const; 
	Vec2 GetDisplacement() const;
	float GetLength() const;
	float GetLengthSqr() const;
	float GetRotation() const;
	AABB2 GetBoundingAABB2() const;

	inline void SetStart(const Vec2& pos) { m_start = pos;}
	inline void SetEnd(const Vec2& pos) { m_end = pos;}
	void SetLength(float amount);

	// This is what we actually need; 
	// returns closest point on line segment (voronoi regions.  Either on the segment, or one of the end points)
	Vec2 GetClosestPoint( const Vec2& reference_pos ) const; 
	float GetDistanceSquared(const Vec2& reference_pos);

	bool IsPointOnLine(const Vec2& point);
	bool IsIntersecting( const Segment2& line);

public:
	Vec2 m_start;
	Vec2 m_end; 
};
