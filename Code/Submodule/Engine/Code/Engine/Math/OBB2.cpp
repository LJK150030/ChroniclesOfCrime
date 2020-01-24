#include "Engine/Math/OBB2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Segment2.hpp"

OBB2::OBB2() {}

OBB2::OBB2( const Vec2& center, const Vec2& half_extents, const float rotation_degrees)
{
	m_center = center;
	m_halfExtents = half_extents;
	m_right.RotateDegrees(rotation_degrees);
	m_up = m_right.GetRotated90Degrees();
}

OBB2::OBB2( const AABB2& aabb )
{
	m_right = Vec2( 1.0f, 0.0f ); 
	m_up = Vec2( 0.0f, 1.0f ); 
	m_center = (aabb.mins + aabb.maxs) * 0.5f;
	m_halfExtents = (aabb.maxs - aabb.mins) * 0.5f; 
}

void OBB2::Transform(float uniform_scale, float z_rotation_degrees, const Vec2& direction)
{
	m_halfExtents = Vec2(m_halfExtents.x * uniform_scale, m_halfExtents.y * uniform_scale);
	m_center = TransformPosition(m_center, uniform_scale, z_rotation_degrees, direction);
	m_right.SetAngleDegrees(z_rotation_degrees);
	m_up = m_right.GetRotated90Degrees();
}

void OBB2::Translate(const Vec2& offset)
{
	m_center += offset;
}

void OBB2::MoveTo(const Vec2& position)
{
	m_center = position;
}

//--------------------------------------------------------------------------------
void OBB2::RotateBy(const float degrees )
{
	m_right.RotateDegrees( degrees ); 
	m_up = m_right.GetRotated90Degrees(); 
}

//--------------------------------------------------------------------------------
void OBB2::SetRotation(const float degrees )
{
	m_right = Vec2( CosDegrees(degrees), SinDegrees(degrees) );
	m_up = Vec2( -m_right.y, m_right.x ); 
}

float OBB2::GetRotation() const
{
	return  Atan2Degrees(m_right.y, m_right.x);
}

void OBB2::SetSize(const Vec2& size)
{
	m_halfExtents = size;
}


Vec2 OBB2::GetBottomLeft() const
{
	return m_center - m_halfExtents.x * GetRight() - m_halfExtents.y * GetUp();
}

Vec2 OBB2::GetBottomRight() const
{
	return m_center + m_halfExtents.x * GetRight() - m_halfExtents.y * GetUp();
}

Vec2 OBB2::GetTopLeft() const
{
	return m_center - m_halfExtents.x * GetRight() + m_halfExtents.y * GetUp();
}

Vec2 OBB2::GetTopRight() const
{
	return m_center + m_halfExtents.x * GetRight() + m_halfExtents.y * GetUp();
}

AABB2 OBB2::GetLocalAABB2() const
{
	return AABB2(-1.0f * m_halfExtents.x, -1.0f * m_halfExtents.y, 1.0f * m_halfExtents.x, 1.0f * m_halfExtents.y);
}

Vec2 OBB2::ToLocalPoint(const Vec2& world_point) const
{
	const Vec2 disp = world_point - GetCenter();  

	// world i = m_right;
	// world j = m_up
	const float local_i = DotProduct( disp, GetRight() ); 
	const float local_j = DotProduct( disp, GetUp() ); 

	return Vec2( local_i, local_j ); 
}

Vec2 OBB2::ToWorldPoint(const Vec2& local_point) const
{
	// world i = m_right;
	// world j = m_up

	return local_point.x * GetRight() + local_point.y * GetUp() + m_center; 
}

bool OBB2::Contains(const Vec2& world_point ) const
{
	const Vec2 local_point = ToLocalPoint( world_point ); 
	const Vec2 abs_local_point = Vec2(Abs(local_point.x), Abs(local_point.y)); 
	return (abs_local_point.x < m_halfExtents.x) && (abs_local_point.y < m_halfExtents.y); 
}

Vec2 OBB2::GetClosestPoint(const Vec2& world_point) const
{
	const Vec2 local_point = ToLocalPoint( world_point ); 
	const float clamped_point_x = ClampFloat( local_point.x, -m_halfExtents.x, m_halfExtents.x ); 
	const float clamped_point_y = ClampFloat( local_point.y, -m_halfExtents.y, m_halfExtents.y ); 

	return ToWorldPoint( Vec2(clamped_point_x, clamped_point_y) );
}

Vec2 OBB2::GetClosestEdgePoint(const Vec2& reference_pos) const
{
	Segment2 edges_to_test[] = {
		Segment2(GetTopLeft(), GetTopRight()), 
		Segment2(GetTopRight(), GetBottomRight()),
		Segment2(GetBottomRight(), GetBottomLeft()),
		Segment2(GetBottomLeft(), GetTopLeft())
	};

	int shortest_length_index = 0;
	for(int edge_id = 1;  edge_id < 4; ++edge_id)
	{
		float length_1 = edges_to_test[shortest_length_index].GetDistanceSquared(reference_pos);
		float length_2 = edges_to_test[edge_id].GetDistanceSquared(reference_pos);

		if(length_1 > length_2) shortest_length_index = edge_id;
	}

	Vec2 closest_point = edges_to_test[shortest_length_index].GetClosestPoint(reference_pos);
	return closest_point;
}

bool OBB2::IsIntersecting(const OBB2& other) const
{
	//TODO: create an IS Intersecting with OBB2
	UNUSED(other);
	return false;
}

bool OBB2::IsPointInside(const Vec2& reference_pos) const
{
	const Vec2 local_point = ToLocalPoint(reference_pos);
	return IsPointInAABB2(local_point, GetLocalAABB2());
}

bool OBB2::IsOverlappingWithLineSegment2D(const Segment2& line)
{
	const Segment2 local_line(ToLocalPoint(line.m_start), ToLocalPoint(line.m_end));
	return DoesAABB2OverlapLineSegment2D(GetLocalAABB2(), local_line.m_start, local_line.m_end);
}

AABB2 OBB2::GetBoundingAABB() const
{
	Vec2 bottom_left = GetBottomLeft();
	Vec2 bottom_right = GetBottomRight();
	Vec2 top_left = GetTopLeft();
	Vec2 top_right = GetTopRight();

	Vec2 min = bottom_left;
	Vec2 max = bottom_left;

	min.x = (min.x < bottom_right.x) ? min.x : bottom_right.x;
	min.x = (min.x < top_left.x) ? min.x : top_left.x;
	min.x = (min.x < top_right.x) ? min.x : top_right.x;

	min.y = (min.y < bottom_right.y) ? min.y : bottom_right.y;
	min.y = (min.y < top_left.y) ? min.y : top_left.y;
	min.y = (min.y < top_right.y) ? min.y : top_right.y;

	max.x = (max.x > bottom_right.x) ? max.x : bottom_right.x;
	max.x = (max.x > top_left.x) ? max.x : top_left.x;
	max.x = (max.x > top_right.x) ? max.x : top_right.x;

	max.y = (max.y > bottom_right.y) ? max.y : bottom_right.y;
	max.y = (max.y > top_left.y) ? max.y : top_left.y;
	max.y = (max.y > top_right.y) ? max.y : top_right.y;

	return AABB2(min, max);
}

void OBB2::GetBoundingDisc(Vec2& center, float& radius) const
{
	center = GetCenter();

	AABB2 box = GetLocalAABB2();

	radius = box.maxs.GetLength();
}
