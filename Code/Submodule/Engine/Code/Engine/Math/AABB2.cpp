#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Segment2.hpp"

const STATIC Vec2 AABB2::ALIGN_TOP_CENTER(0.5f, 1.0f);
const STATIC Vec2 AABB2::ALIGN_CENTERED(0.5f, 0.5f);
const STATIC Vec2 AABB2::ALIGN_BOTTOM_CENTER(0.5f, 0.0f);
const STATIC Vec2 AABB2::ALIGN_TOP_RIGHT(1.0f, 1.0f);
const STATIC Vec2 AABB2::ALIGN_MIDDLE_RIGHT(1.0f, 0.5f);
const STATIC Vec2 AABB2::ALIGN_BOTTOM_RIGHT(1.0f, 0.0f);
const STATIC Vec2 AABB2::ALIGN_TOP_LEFT(0.0f, 1.0f);
const STATIC Vec2 AABB2::ALIGN_MIDDLE_LEFT(0.0f, 0.5f);
const STATIC Vec2 AABB2::ALIGN_BOTTOM_LEFT(0.0f, 0.0f);
STATIC AABB2 AABB2::UNIT_BOX(0.0f, 0.0f, 1.0f, 1.0f);

AABB2::AABB2(const float initial_min_x, const float initial_min_y, const float initial_max_x, const float initial_max_y)
{
	mins = Vec2(initial_min_x, initial_min_y);
	maxs = Vec2(initial_max_x, initial_max_y);
	center = GetCenter();
}

void AABB2::GrowToIncludePos(const Vec2& p)
{
	if (p.x < mins.x)
		mins.x = p.x;
	else if (p.y < mins.y)
		mins.y = p.y;
	else if (p.x > maxs.x)
		maxs.x = p.x;
	else if (p.y > maxs.y)
		maxs.y = p.y;
}

void AABB2::Translate(const float uniform_scale, const float z_rotation_degrees, const Vec2& direction)
{
	mins = TransformPosition(mins, uniform_scale, z_rotation_degrees, direction);
	maxs = TransformPosition(maxs, uniform_scale, z_rotation_degrees, direction);
}

void AABB2::Resize(const Vec2& new_dimensions, const Vec2& pivot_anchor_alignment)
{
	Vec2 current_dimensions = maxs - mins;

	if(!FLIP_TEXTURE_VERTICALLY)
		current_dimensions.y *= -1.0f;

 	const Vec2 scale_vector = new_dimensions - current_dimensions;

	mins.x -= pivot_anchor_alignment.x * scale_vector.x;
	mins.y -= pivot_anchor_alignment.y * scale_vector.y;

	maxs.x += (1.0f - pivot_anchor_alignment.x) * scale_vector.x;
	maxs.y += (1.0f - pivot_anchor_alignment.y) * scale_vector.y;
}

//I want to align this within the given box
void AABB2::AlignWithinBox(const AABB2& box, const Vec2& alignment)
{
	float other_box_height = box.maxs.y - box.mins.y;
	float our_box_height = maxs.y - mins.y;
	float inner_box_height = other_box_height - our_box_height;

	float other_box_width = box.maxs.x - box.mins.x;
	float our_box_width = maxs.x - mins.x;
	float inner_box_width = other_box_width - our_box_width;

	mins.x = box.mins.x + alignment.x * inner_box_width;
	mins.y = box.mins.y + alignment.y * inner_box_height;

	maxs.x = mins.x + our_box_width;
	maxs.y = mins.y + our_box_height;
}

AABB2 AABB2::GetBoxWithin(const Vec2& dimensions, const Vec2& alignment) const
{
	const Vec2 current_dimensions = maxs - mins;
	const Vec2 scale_vector = dimensions - current_dimensions;
	AABB2 new_box(*this);

	new_box.mins.x -= alignment.x * scale_vector.x;
	new_box.mins.y -= alignment.y * scale_vector.y;

	new_box.maxs.x += (1.0f - alignment.x) * scale_vector.x;
	new_box.maxs.y += (1.0f - alignment.y) * scale_vector.y;

	return new_box;
}

AABB2 AABB2::GetBoxAtLeft(float fraction_of_width, float additional_width) const
{
	const float total_width = maxs.x - mins.x;
	AABB2 new_box(*this);
	float carved_width = total_width * fraction_of_width;
	carved_width += additional_width;
	//carvedWidth = Clamp(minwidth, maxwidth);
	carved_width = ClampFloat(carved_width, 0.0f, total_width);

	new_box.maxs.x = mins.x + carved_width;
	return new_box;
}

AABB2 AABB2::GetBoxAtRight(float fraction_of_width, float additional_width) const
{
	const float total_width = maxs.x - mins.x;
	AABB2 new_box(*this);
	float carved_width = total_width * fraction_of_width;
	carved_width += additional_width;
	//carvedWidth = Clamp(minwidth, maxwidth);
	carved_width = ClampFloat(carved_width, 0.0f, total_width);

	new_box.mins.x = mins.x + carved_width;
	return new_box;
}

AABB2 AABB2::GetBoxAtBottom(float fraction_of_height, float additional_height) const
{
	const float total_height = maxs.y - mins.y;
	AABB2 new_box(*this);
	float carved_height = total_height * fraction_of_height;
	carved_height += additional_height;
	//carvedWidth = Clamp(min_height, max_height);
	carved_height = ClampFloat(carved_height, 0.0f, total_height);

	new_box.maxs.y = mins.y + carved_height;
	return new_box;
}

AABB2 AABB2::GetBoxAtTop(float fraction_of_height, float additional_height) const
{
	const float total_height = maxs.y - mins.y;
	AABB2 new_box(*this);
	float carved_height = total_height * fraction_of_height;
	carved_height += additional_height;
	//carvedWidth = Clamp(min_height, max_height);
	carved_height = ClampFloat(carved_height, 0.0f, total_height);

	new_box.mins.y = mins.y + carved_height;
	return new_box;
}

AABB2 AABB2::CarveBoxOffLeft(float fraction_of_width, float additional_width)
{
	const float total_width = maxs.x - mins.x;
	AABB2 carved_box(*this);
	float carved_width = total_width * fraction_of_width;
	carved_width += additional_width;
	//carvedWidth = Clamp(minwidth, maxwidth);
	carved_width = ClampFloat(carved_width, 0.0f, total_width);
	carved_box.maxs.x = mins.x + carved_width;
	mins.x = carved_box.maxs.x;
	return carved_box;
}

AABB2 AABB2::CarveBoxOffRight(float fraction_of_width, float additional_width)
{
	const float total_width = maxs.x - mins.x;
	AABB2 carved_box(*this);
	float carved_width = total_width * fraction_of_width;
	carved_width += additional_width;
	//carvedWidth = Clamp(minwidth, maxwidth);
	carved_width = ClampFloat(carved_width, 0.0f, total_width);
	carved_box.mins.x = mins.x + carved_width;
	maxs.x = carved_box.mins.x;
	return carved_box;
}

AABB2 AABB2::CarveBoxOffBottom(float fraction_of_height, float additional_height)
{
	const float total_height = maxs.y - mins.y;
	AABB2 carved_box(*this);
	float carved_height = total_height * fraction_of_height;
	carved_height += additional_height;
	//carvedWidth = Clamp(min_height, max_height);
	carved_height = ClampFloat(carved_height, 0.0f, total_height);
	carved_box.maxs.y = mins.y + carved_height;
	mins.y = carved_box.maxs.y;
	return carved_box;
}

AABB2 AABB2::CarveBoxOffTop(float fraction_of_height, float additional_height)
{
	const float total_height = maxs.y - mins.y;
	AABB2 carved_box(*this);
	float carved_height = total_height * fraction_of_height;
	carved_height += additional_height;
	//carvedWidth = Clamp(min_height, max_height);
	carved_height = ClampFloat(carved_height, 0.0f, total_height);
	carved_box.mins.y = mins.y + carved_height;
	maxs.y = carved_box.mins.y;
	return carved_box;
}

Vec2 AABB2::GetUVFromPoint(const Vec2& point) const
{
	const float x_diff = point.x - mins.x;
	const float total_x_diff = maxs.x - mins.x;
	const float y_diff = point.y - mins.y;
	const float total_y_diff = maxs.y - mins.y;
	const float uv_x = x_diff / total_x_diff;
	const float uv_y = y_diff / total_y_diff;
	return Vec2(uv_x, uv_y);
}

Vec2 AABB2::GetPointFromUV(const Vec2& normalized_local_position) const
{
	const float x = mins.x + (normalized_local_position.x * (maxs.x - mins.x));
	const float y = mins.y + (normalized_local_position.y * (maxs.y - mins.y));
	return Vec2(x, y);
}

Vec2 AABB2::GetCenter() const
{
	const float x_mid = mins.x + (maxs.x - mins.x) / 2.0f;
	const float y_mid = mins.y + (maxs.y - mins.y) / 2.0f;

	return Vec2(x_mid, y_mid);
}

Vec2 AABB2::GetTopLeft() const
{
	return Vec2(mins.x, maxs.y);
}

Vec2 AABB2::GetTopRight() const
{
	return maxs;
}

Vec2 AABB2::GetBottomLeft() const
{
	return mins;
}

Vec2 AABB2::GetBottomRight() const
{
	return Vec2(maxs.x, mins.y);
}

Vec2 AABB2::GetClosestPoint(const Vec2& reference_pos) const
{
	return GetClosestPointOnAABB2(reference_pos, *this);
}

Vec2 AABB2::GetClosestEdgePoint(const Vec2& reference_pos) const
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

void AABB2::GetBoundingDisc(Vec2& world_center, float& radius) const
{
	world_center = GetCenter();
	radius = Vec2(maxs - GetCenter()).GetLength();
}

bool AABB2::IsOverlappingWithLineSegment2D(const Segment2& line)
{
	return DoesAABB2OverlapLineSegment2D(*this, line.m_start, line.m_end);
}

bool AABB2::IsOverlappingWithAABB2(const AABB2& other_box)
{
	return DoAABB2SOverlap(*this, other_box);
}

AABB2 AABB2::GetIntersection(const AABB2& other_box) const
{
	const Vec2 min = MaxVec2(mins, other_box.mins);
	const Vec2 max = MinVec2(maxs, other_box.maxs);
	return AABB2(min, max);
}

bool AABB2::IsPointInside(const Vec2& p) const
{
	return (mins.x <= p.x && p.x <= maxs.x) && (mins.y <= p.y && p.y <= maxs.y);
}
