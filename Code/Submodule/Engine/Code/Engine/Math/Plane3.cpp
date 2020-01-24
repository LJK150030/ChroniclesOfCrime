#include "Engine/Math/Plane3.hpp"
#include "Engine/Math/MathUtils.hpp"

Plane3::Plane3(): m_normal(Vec3::FORWARD), m_signedDistance(0)
{

}

Plane3::Plane3(const Vec3& forward_dir, float distance_from_origin): m_normal(forward_dir), m_signedDistance(distance_from_origin)
{
}

Plane3::Plane3(const Vec3& point_one, const Vec3& point_two, const Vec3& point_three)
{
	Vec3 vector_one_to_two = point_two - point_one;
	Vec3 vector_one_to_three = point_three - point_one;

	m_normal = Cross(vector_one_to_two, vector_one_to_three);
	m_normal.Normalize();
	m_signedDistance = DotProduct(m_normal, point_one); 
}

Plane3::~Plane3() = default;

bool Plane3::IsPointOnPlane(const Vec3& point) const
{
	const float value = m_normal.x * point.x + m_normal.y * point.y + m_normal.z * point.z;
	return AreEqual(value, -1.0f * m_signedDistance);
}

Vec3 Plane3::ClosestPointToOrigin() const
{
	return m_normal * m_signedDistance;
}
