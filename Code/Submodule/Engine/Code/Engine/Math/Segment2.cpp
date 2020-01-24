#include "Engine/Math/Segment2.hpp"
#include "Engine/Math/MathUtils.hpp"

Segment2::Segment2(): m_start(Vec2::ZERO), m_end(Vec2::ZERO) {}

Segment2::Segment2(const Vec2& start, const Vec2& end): m_start(start), m_end(end) {}

Segment2::~Segment2() {}

Vec2 Segment2::GetCenter() const
{
	Vec2 total = m_start + m_end;
	return total * 0.5f;
}

Vec2 Segment2::GetDisplacement() const
{
	return m_end - m_start;
}

float Segment2::GetLength() const
{
	return Sqrt(GetLengthSqr());
}

float Segment2::GetLengthSqr() const
{
	const Vec2 displacement = GetDisplacement();
	return displacement.x * displacement.x + displacement.y * displacement.y;
}

float Segment2::GetRotation() const
{
	const Vec2 displacement = GetDisplacement();
	return Atan2Degrees(displacement.y, displacement.x);
}

AABB2 Segment2::GetBoundingAABB2() const
{
	Vec2 min = MinVec2(m_start, m_end);
	Vec2 max = MaxVec2(m_start, m_end);

	return AABB2(min, max);
}

void Segment2::SetLength(float amount)
{
	Vec2 center = GetCenter();

	Vec2 c_2_s = m_start - center;
	c_2_s.Normalize();

	if(IsZero(c_2_s.x) && IsZero(c_2_s.y))
		c_2_s = Vec2(1.0f, 0.0f);

	Vec2 c_2_e = m_end - center;
	c_2_e.Normalize();

	if(IsZero(c_2_e.x) && IsZero(c_2_e.y))
		c_2_e = Vec2(-1.0f, 0.0f);

	m_start = center + c_2_s * (0.5f * amount);
	m_end = center + c_2_e * (0.5f * amount);
}

Vec2 Segment2::GetClosestPoint( const Vec2& reference_pos ) const
{
	return GetClosestPointOnLineSegment2D(reference_pos, m_start, m_end);
}

float Segment2::GetDistanceSquared(const Vec2& reference_pos)
{
	return GetDistanceSquaredFromLineSegment2D(reference_pos, m_start, m_end);
}

// https://stackoverflow.com/questions/563198/how-do-you-detect-where-two-line-segments-intersect
bool Segment2::IsIntersecting(const Segment2& line)
{
	const Vec2 CmP(line.m_start.x - m_start.x, line.m_start.y - m_start.y);
	const Vec2 r(m_end.x - m_start.x, m_end.y - m_start.y);
	const Vec2 s(line.m_end.x - line.m_start.x, line.m_end.y - line.m_start.y);

	const float CmP_cross_r = CmP.x * r.y - CmP.y * r.x;
	const float CmP_cross_s = CmP.x * s.y - CmP.y * s.x;
	const float r_cross_s = r.x * s.y - r.y * s.x;

	if (IsZero(CmP_cross_r))
	{
		// Lines are collinear, and so intersect if they have any overlap
		return ((line.m_start.x - m_start.x < 0.0f) != (line.m_start.x - m_end.x < 0.0f))
			|| ((line.m_start.y - m_start.y < 0.0f) != (line.m_start.y - m_end.y < 0.0f));
	}

	if (IsZero(r_cross_s))
		return false; // Lines are parallel.

	const float r_cross_sr = 1.0f / r_cross_s;
	const float t = CmP_cross_s * r_cross_sr;
	const float u = CmP_cross_r * r_cross_sr;

	return (t >= 0.0f) && (t <= 1.0f) && (u >= 0.0f) && (u <= 1.0f);
} 

bool Segment2::IsPointOnLine(const Vec2& point) {
	// Find the slope
	float dy = (m_end.y - m_start.y);
	float dx = (m_end.x - m_start.x);
	float M = dy / dx;
	// Find the Y-Intercept
	float B = m_start.y - M * m_start.x;
	// Check line equation
	
	return AreEqual(point.y, M * point.x + B);
}