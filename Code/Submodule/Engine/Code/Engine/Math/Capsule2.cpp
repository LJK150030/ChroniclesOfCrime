#include "Engine/Math/Capsule2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/OBB2.hpp"
#include "Engine/Renderer/DebugRender.hpp"

Capsule2::Capsule2(): m_p0(Vec2::ZERO), m_p1(Vec2::ZERO), m_radius(0.0f) {}

Capsule2::Capsule2(const Vec2& pos): m_p0(pos), m_p1(pos), m_radius(0.0f) {}

Capsule2::Capsule2(const Vec2& center, float radius): m_p0(center), m_p1(center), m_radius(radius) {}

Capsule2::Capsule2(const Vec2& start, const Vec2& end, float radius): m_p0(start), m_p1(end), m_radius(radius) {}

Capsule2::Capsule2(float start_x, float start_y, float end_x, float end_y, float radius)
	:m_p0(Vec2(start_x, start_y)), m_p1(Vec2(end_x, end_y)), m_radius(radius){}

Capsule2::~Capsule2() {}

// special care here;   Use the center of the line as position, but maintain shape
void Capsule2::SetPosition(Vec2 pos)
{
	Vec2 center = GetCenter();
	Vec2 displacement = pos - center;
	m_p0 += displacement;
	m_p1 += displacement;
}

void Capsule2::SetPositions(Vec2 start, Vec2 end)
{
	m_p0 = start;
	m_p1 = end;
}

Vec2 Capsule2::GetClosestPoint(const Vec2& reference_pos)
{
	return GetClosestPointOnCapsule2D(reference_pos, m_p0, m_p1, m_radius);
}

bool Capsule2::IsPointInside(const Vec2& reference_pos)
{
	return IsPointInCapsule2D(reference_pos, m_p0, m_p1, m_radius);
}

bool Capsule2::IsOverlappingWithLineSegment2D(const Segment2& line)
{
	//TODO: IsOverlappingWithLineSegment2D
	UNUSED(line);
	return false;
}

Vec2 Capsule2::GetCenter() const
{
	Vec2 total = m_p1 + m_p0;
	return total * 0.5f;
}

float Capsule2::GetRotation() const
{
	const Vec2 displacement = m_p1 - m_p0;
	const float theta = Atan2Degrees(displacement.y, displacement.x);
	return theta;
}

AABB2 Capsule2::GetBoundingAABB() const
{
	Vec2 displacement = m_p1 - m_p0;
	float theta = Atan2Degrees(displacement.y, displacement.x);
	Vec2 halfextence(displacement.GetLength() * 0.5f, m_radius);
	OBB2 base_box(GetCenter(), halfextence, theta);

	displacement.SetLength(m_radius);
	AABB2 obb_bound = base_box.GetBoundingAABB();
	Vec2 min = obb_bound.GetBottomLeft() + (displacement * -1.0f);
	Vec2 max = obb_bound.GetTopRight() + (displacement * 1.0f);

	return AABB2(min, max);
}

void Capsule2::GetBoundingDisc(Vec2& center, float& radius) const
{
	const Vec2 centerpoint = GetCenter();
	Vec2 half_direction = m_p1 - centerpoint;
	const float half_length = half_direction.GetLength() + m_radius;

	center = centerpoint;
	radius = half_length;
}

OBB2 Capsule2::GetOBB() const
{
	Vec2 displacement = m_p1 - m_p0;
	float theta = Atan2Degrees(displacement.y, displacement.x);
	Vec2 halfextence(displacement.GetLength() * 0.5f, m_radius);
	OBB2 base_box(GetCenter(), halfextence, theta);

	return base_box;
}

void Capsule2::Transform(float uniform_scale, float z_rotation_degrees, const Vec2& direction)
{
	m_radius *= uniform_scale;
	m_p0 = TransformPosition(m_p0, uniform_scale, z_rotation_degrees, direction);
	m_p1 = TransformPosition(m_p1, uniform_scale, z_rotation_degrees, direction);
}

void Capsule2::Translate(const Vec2& direction)
{
	m_p0 += direction;
	m_p1 += direction;
}

void Capsule2::RotateBy(const float degrees)
{
	m_p0.RotateDegrees(degrees);
	m_p1.RotateDegrees(degrees);
}


