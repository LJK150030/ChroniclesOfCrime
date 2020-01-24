#include "Engine/Math/OBB3.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/Matrix44.hpp"

OBB3::OBB3()
{
}

OBB3::~OBB3()
{
}

OBB3::OBB3(const Vec3& right, const Vec3& up, const Vec3& position, const Vec3& half_extents) : 
	m_right(right), m_up(up), m_center(position), m_halfExtents(half_extents)
{
	m_forward = Cross(m_up, m_right);
}

OBB3::OBB3(const Vec3& forward, const Vec3& position, const Vec3& half_extents):
	m_forward(forward), m_center(position), m_halfExtents(half_extents)
{
	//TODO: works for our current game, need to come back to this
	m_right = Cross(m_forward, Vec3(0.0f, 0.0f, -1.0f));
	m_up = Cross(m_right, m_forward );
}

Vec3 OBB3::ToLocalPoint(const Vec3& world_point) const
{
	const Vec3 disp = world_point - m_center;  

	const float local_i = DotProduct( disp, m_right ); 
	const float local_j = DotProduct( disp, m_up ); 
	const float local_k = DotProduct( disp, m_forward ); 

	return Vec3( local_i, local_j, local_k ); 
}

Vec3 OBB3::ToWorldPoint(const Vec3& local_point) const
{
	return local_point.x * m_right + local_point.y * m_up + local_point.z * m_forward + m_center; 
}

Vec3 OBB3::ToLocalVector(const Vec3& world_vector) const
{
	Vec3 projection_vector = Vec3( 
		DotProduct(m_right, world_vector), 
		DotProduct(m_up, world_vector), 
		DotProduct(m_forward, world_vector)
	);

	return projection_vector;
}

Vec3 OBB3::ToWorldVector(const Vec3& local_vector) const
{
	return local_vector.x * m_right + local_vector.y * m_up + local_vector.z * m_forward; 
}

AABB3 OBB3::GetLocalAABB3() const
{
	return AABB3(-1.0f * m_halfExtents.x, -1.0f * m_halfExtents.y, -1.0f * m_halfExtents.z, 1.0f * m_halfExtents.x, 1.0f * m_halfExtents.y, 1.0f * m_halfExtents.z);
}

Vec3 OBB3::GetTopLeftFrontCorner() const
{
	AABB3 local = GetLocalAABB3();
	Vec3 point = local.GetTopLeftFrontCorner();
	return ToWorldPoint(point);
	
	//return m_center - m_halfExtents.x * m_right + m_halfExtents.y * m_up + m_halfExtents.z * m_forward;
}

Vec3 OBB3::GetTopRightFrontCorner() const
{
	AABB3 local = GetLocalAABB3();
	Vec3 point = local.GetTopRightFrontCorner();
	return ToWorldPoint(point);
	
	//return m_center + m_halfExtents.x * m_right + m_halfExtents.y * m_up + m_halfExtents.z * m_forward;
}

Vec3 OBB3::GetBottomLeftFrontCorner() const
{
	AABB3 local = GetLocalAABB3();
	Vec3 point = local.GetBottomLeftFrontCorner();
	return ToWorldPoint(point);

	//return m_center - m_halfExtents.x * m_right - m_halfExtents.y * m_up + m_halfExtents.z * m_forward;
}

Vec3 OBB3::GetBottomRightFrontCorner() const
{
	AABB3 local = GetLocalAABB3();
	Vec3 point = local.GetBottomRightFrontCorner();
	return ToWorldPoint(point);
	//return m_center + m_halfExtents.x * m_right - m_halfExtents.y * m_up + m_halfExtents.z * m_forward;
}

Vec3 OBB3::GetTopLeftBackCorner() const
{
	AABB3 local = GetLocalAABB3();
	Vec3 point = local.GetTopLeftBackCorner();
	return ToWorldPoint(point);
	
	//return m_center - m_halfExtents.x * m_right + m_halfExtents.y * m_up - m_halfExtents.z * m_forward;
}

Vec3 OBB3::GetTopRightBackCorner() const
{
	AABB3 local = GetLocalAABB3();
	Vec3 point = local.GetTopRightBackCorner();
	return ToWorldPoint(point);
	//return m_center + m_halfExtents.x * m_right + m_halfExtents.y * m_up - m_halfExtents.z * m_forward;
}

Vec3 OBB3::GetBottomLeftBackCorner() const
{
	AABB3 local = GetLocalAABB3();
	Vec3 point = local.GetBottomLeftBackCorner();
	return ToWorldPoint(point);
	//return m_center - m_halfExtents.x * m_right - m_halfExtents.y * m_up - m_halfExtents.z * m_forward;
}

Vec3 OBB3::GetBottomRightBackCorner() const
{
	AABB3 local = GetLocalAABB3();
	Vec3 point = local.GetBottomRightBackCorner();
	return ToWorldPoint(point);
	//return m_center + m_halfExtents.x * m_right - m_halfExtents.y * m_up - m_halfExtents.z * m_forward;
}

Vec3 OBB3::GetDimensions() const
{
	return m_halfExtents * 2.0f;
}

void OBB3::Translate(const Vec3& translation)
{
	m_center += translation;
}

void OBB3::Rotate(const Matrix44& rotation_matrix)
{
	m_right = rotation_matrix.GetIvec3();
	m_up = rotation_matrix.GetJvec3();
	m_forward = rotation_matrix.GetKvec3();
}
