#pragma once
#include "Engine/Math/Vec2.hpp"

struct Segment2;

struct AABB2
{
public:
	Vec2 mins;
	Vec2 maxs;
	Vec2 center;
	const static Vec2 ALIGN_TOP_CENTER;
	const static Vec2 ALIGN_CENTERED;
	const static Vec2 ALIGN_BOTTOM_CENTER;
	const static Vec2 ALIGN_TOP_RIGHT;
	const static Vec2 ALIGN_MIDDLE_RIGHT;
	const static Vec2 ALIGN_BOTTOM_RIGHT;
	const static Vec2 ALIGN_TOP_LEFT;
	const static Vec2 ALIGN_MIDDLE_LEFT;
	const static Vec2 ALIGN_BOTTOM_LEFT;
	static AABB2 UNIT_BOX;

public:
	AABB2(): mins(Vec2::ZERO), maxs(Vec2::ONE), center(Vec2(0.5f, 0.5f))
	{
	}

	AABB2(const Vec2& initial_min, const Vec2& initial_max): mins(initial_min), maxs(initial_max)
	{
	};
	AABB2(float initial_min_x, float initial_min_y, float initial_max_x, float initial_max_y);
	~AABB2() = default;

	void GrowToIncludePos(const Vec2& p);
	void Translate(float uniform_scale, float z_rotation_degrees, const Vec2& direction);

	void Resize(const Vec2& new_dimensions, const Vec2& pivot_anchor_alignment = ALIGN_CENTERED);
	void AlignWithinBox(const AABB2& box, const Vec2& alignment);
	AABB2 GetBoxWithin(const Vec2& dimensions, const Vec2& alignment) const;

	AABB2 GetBoxAtLeft(float fraction_of_width, float additional_width = 0.f) const;
	AABB2 GetBoxAtRight(float fraction_of_width, float additional_width = 0.f) const;
	AABB2 GetBoxAtBottom(float fraction_of_height, float additional_height = 0.f) const;
	AABB2 GetBoxAtTop(float fraction_of_height, float additional_height = 0.f) const;

	AABB2 CarveBoxOffLeft(float fraction_of_width, float additional_width = 0.f);
	AABB2 CarveBoxOffRight(float fraction_of_width, float additional_width = 0.f);
	AABB2 CarveBoxOffBottom(float fraction_of_height, float additional_height = 0.f);
	AABB2 CarveBoxOffTop(float fraction_of_height, float additional_height = 0.f);

	Vec2 GetUVFromPoint(const Vec2& point) const;
	Vec2 GetPointFromUV(const Vec2& normalized_local_position) const;
	Vec2 GetCenter() const;
	Vec2 GetTopLeft() const;
	Vec2 GetTopRight() const;
	Vec2 GetBottomLeft() const;
	Vec2 GetBottomRight() const;

	bool IsPointInside(const Vec2& p) const;
	Vec2 GetClosestPoint(const Vec2& reference_pos) const;
	Vec2 GetClosestEdgePoint(const Vec2& reference_pos) const;
	void GetBoundingDisc(Vec2& world_center, float& radius) const;

	bool IsOverlappingWithLineSegment2D(const Segment2& line);
	bool IsOverlappingWithAABB2(const AABB2& other_box);
	AABB2 GetIntersection(const AABB2& other_box) const;

};
