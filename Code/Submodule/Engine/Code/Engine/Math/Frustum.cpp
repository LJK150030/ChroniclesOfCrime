#include "Engine/Math/Frustum.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Math/Sphere.hpp"
#include "Engine/Math/Capsule3.hpp"
#include "Engine/Math/Ray3.hpp"

Frustum::Frustum()
{
}

Frustum::~Frustum()
{
}

void Frustum::GenerateFromCorners(
	/* 0 */ const Vec3& left_bottom_front,
	/* 1 */ const Vec3& right_bottom_front,
	/* 2 */ const Vec3& left_top_front, 
	/* 3 */ const Vec3& right_top_front, 
	/* 4 */ const Vec3& left_bottom_back,
	/* 5 */ const Vec3& right_bottom_back, 
	/* 6 */ const Vec3& left_top_back, 
	/* 7 */ const Vec3& right_top_back)
{
	UNUSED(right_top_back);
	m_near = Plane3(left_bottom_front, right_bottom_front, right_top_front);
	m_far = Plane3(right_bottom_back, left_bottom_back, left_top_back);
	m_top = Plane3(left_top_back, left_top_front, right_top_front);
	m_bottom = Plane3(left_bottom_front, left_bottom_back, right_bottom_back);
	m_left = Plane3(left_top_front, left_top_back, left_bottom_back);
	m_right = Plane3(right_top_front, right_bottom_front,  right_bottom_back);
}

bool Frustum::IntersectionTest(const Vec3& point) const
{
	Vec3 top_point = m_top.ClosestPointToOrigin();
	Vec3 point_to_point = point - top_point;
	float projected_value = DotProduct(m_top.m_normal, point_to_point);
	if(projected_value < 0.0f) return false;

	Vec3 bottom_point = m_bottom.ClosestPointToOrigin();
	point_to_point = point - bottom_point;
	projected_value = DotProduct(m_bottom.m_normal, point_to_point);
	if(projected_value < 0.0f) return false;

	Vec3 left_point = m_left.ClosestPointToOrigin();
	point_to_point = point - left_point;
	projected_value = DotProduct(m_left.m_normal, point_to_point);
	if(projected_value < 0.0f) return false;

	Vec3 right_point = m_right.ClosestPointToOrigin();
	point_to_point = point - right_point;
	projected_value = DotProduct(m_right.m_normal, point_to_point);
	if(projected_value < 0.0f) return false;

	Vec3 near_point = m_near.ClosestPointToOrigin();
	point_to_point = point - near_point;
	projected_value = DotProduct(m_near.m_normal, point_to_point);
	if(projected_value < 0.0f) return false;

	Vec3 far_point = m_far.ClosestPointToOrigin();
	point_to_point = point - far_point;
	projected_value = DotProduct(m_far.m_normal, point_to_point);
	if(projected_value < 0.0f) return false;

	return true;
}
 
bool Frustum::IntersectionTest(const Sphere& sphere) const
{
	Vec3 top_point = m_top.ClosestPointToOrigin();
	Vec3 point_to_sphere = sphere.point - top_point;
	float projected_value = DotProduct(m_top.m_normal, point_to_sphere);
	if(projected_value < -1.0f * sphere.radius) return false;

	Vec3 bottom_point = m_bottom.ClosestPointToOrigin();
	point_to_sphere = sphere.point - bottom_point;
	projected_value = DotProduct(m_bottom.m_normal, point_to_sphere);
	if(projected_value < -1.0f * sphere.radius) return false;

	Vec3 left_point = m_left.ClosestPointToOrigin();
	point_to_sphere = sphere.point - left_point;
	projected_value = DotProduct(m_left.m_normal, point_to_sphere);
	if(projected_value < -1.0f * sphere.radius) return false;

	Vec3 right_point = m_right.ClosestPointToOrigin();
	point_to_sphere = sphere.point - right_point;
	projected_value = DotProduct(m_right.m_normal, point_to_sphere);
	if(projected_value < -1.0f * sphere.radius) return false;

	Vec3 near_point = m_near.ClosestPointToOrigin();
	point_to_sphere = sphere.point - near_point;
	projected_value = DotProduct(m_near.m_normal, point_to_sphere);
	if(projected_value < -1.0f * sphere.radius) return false;

	Vec3 far_point = m_far.ClosestPointToOrigin();
	point_to_sphere = sphere.point - far_point;
	projected_value = DotProduct(m_far.m_normal, point_to_sphere);
	if(projected_value < -1.0f * sphere.radius) return false;

	return true;
}

float Frustum::IntersectionAt(const Ray3& ray) const
{
	float results[6];
	

	Vec3 top_normal =  m_top.m_normal;
	Vec3 top_point = m_top.ClosestPointToOrigin();

	float dot_2 = DotProduct(top_normal, ray.direction);
	if(IsZero(dot_2)) results[0] = INFINITY;
	else
	{
		float dot_1 = DotProduct(top_point - ray.position, top_normal);
		results[0] = dot_1 / dot_2;
		if(results[0] < 0.0f) results[0] = INFINITY;
	}

	Vec3 bottom_normal = m_bottom.m_normal;
	Vec3 bottom_point = m_bottom.ClosestPointToOrigin();
	dot_2 = DotProduct(bottom_normal, ray.direction);
	if(IsZero(dot_2)) results[1] = INFINITY;
	else
	{
		float dot_1 = DotProduct(bottom_point - ray.position, bottom_normal);
		results[1] = dot_1 / dot_2;
		if(results[1] < 0.0f) results[1] = INFINITY;
	}

	Vec3 left_normal = m_left.m_normal;
	Vec3 left_point = m_left.ClosestPointToOrigin();
	dot_2 = DotProduct(left_normal, ray.direction);
	if(IsZero(dot_2)) results[2] = INFINITY;
	else
	{
		float dot_1 = DotProduct(left_point - ray.position, left_normal);
		results[2] = dot_1 / dot_2;
		if(results[2] < 0.0f) results[2] = INFINITY;
	}

	Vec3 right_normal = m_right.m_normal;
	Vec3 right_point = m_right.ClosestPointToOrigin();
	dot_2 = DotProduct(right_normal, ray.direction);
	if(IsZero(dot_2)) results[3] = INFINITY;
	else
	{
		float dot_1 = DotProduct(right_point - ray.position, right_normal);
		results[3] = dot_1 / dot_2;
		if(results[3] < 0.0f) results[3] = INFINITY;
	}

	Vec3 near_normal = m_near.m_normal;
	Vec3 near_point = m_near.ClosestPointToOrigin();
	dot_2 = DotProduct(near_normal, ray.direction);
	if(IsZero(dot_2)) results[4] = INFINITY;
	else
	{
		float dot_1 = DotProduct(near_point - ray.position, near_normal);
		results[4] = dot_1 / dot_2;
		if(results[4] < 0.0f) results[4] = INFINITY;
	}

	Vec3 far_normal = m_far.m_normal;
	Vec3 far_point = m_far.ClosestPointToOrigin();
	dot_2 = DotProduct(far_normal, ray.direction);
	if(IsZero(dot_2)) results[5] = INFINITY;
	else
	{
		float dot_1 = DotProduct(far_point - ray.position, far_normal);
		results[5] = dot_1 / dot_2;
		if(results[5] < 0.0f) results[5] = INFINITY;
	}

	float min = results[0];
	for(int t_id = 1; t_id < 6; ++t_id)
	{
		min = Min(min, results[t_id]);
	}

	return min;
}

bool Frustum::IntersectionTest(const Capsule3& capsule) const
{
	if(IntersectionTest(Sphere(capsule.start, capsule.radius)))
	{
		return true;
	}
	if(IntersectionTest(Sphere(capsule.end, capsule.radius))) 
	{
		return true;
	}

 	Ray3 ray(capsule.start, Vec3(capsule.end - capsule.start));
 	float t = IntersectionAt(ray);
 
 	if(t == INFINITY) return false;
 	
 	float cap_length = Vec3(capsule.end - capsule.start).GetLength();
 
 	if(t <= cap_length) 
	{
		if(IntersectionTest(Sphere(ray.PointAtTime(t), capsule.radius)))
		{
 			return true;
		}
	}
	return false;
}
