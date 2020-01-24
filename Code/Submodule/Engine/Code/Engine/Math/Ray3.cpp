#include "Engine/Math/Ray3.hpp"
#include "Engine/Math/Sphere.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/OBB3.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Plane3.hpp"
#include "Engine/Math/Capsule3.hpp"
#include "Engine/Renderer/DebugRender.hpp"

STATIC Ray3 Ray3::FromPoints(const Vec3& start, const Vec3& end)
{
	const Vec3 dir = end - start;
	Ray3 new_ray(start, dir);
	return new_ray;
}

Ray3::Ray3() = default;

Ray3::Ray3(const Vec3& pos, const Vec3& dir)
{
	position = pos;
	direction = dir.GetNormalize();
}

Vec3 Ray3::PointAtTime(const float t) const
{
	return position + direction * t;
}

unsigned Raycast(float* out, const Ray3& ray, const Sphere& sphere)
{
	// our line represented as P(t) = E + tD, t >= 0
	// which can be decomposed as 
		// X(T) = E_x + t * D_x
		// Y(T) = E_y + t * D_y
		// Z(T) = E_z + t * D_z

	// The Cartesian equation of a sphere centered at the point (x_0,y_0,z_0) with radius R is given by
		// (x - x_0)^2 + (y - y_0)^2 + (z - z_0)^2 = R^2

	// Replacing each component with our decomposed value gives us
	// (E_x + t*D_x)^2 + (E_y + t*D_y)^2 + (E_z + t*D_z)^2 = R

	// Collecting the values into a quadratic equation gives us x^2 * A + x * B + C = 0
	// t^2 (D_x^2 + D_y^2 + D_z^2) + E_x^2 + E_y^2 + E_z^2 - 2 E_x x_o + x_o^2 - 2 E_y y_o + y_o^2 + 2 t (D_x (E_x - x_o) + D_y (E_y - y_o) + D_z (E_z - z_o)) - 2 E_z z_o + z_o^2 == R^2
	// A =	D_x^2 + D_y^2 + D_z^2
	// B =	D_x (E_x - x_o) + D_y (E_y - y_o) + D_z (E_z - z_o)
	// C =	E_x^2 + E_y^2 + E_z^2 - 2 E_x x_o + x_o^2 - 2 E_y y_o + y_o^2 - 2 E_z z_o + z_o^2 - R^2


	float E_x = ray.position.x;
	float E_y = ray.position.y;
	float E_z = ray.position.z;

	float D_x = ray.direction.x;
	float D_y = ray.direction.y;
	float D_z = ray.direction.z;

	float x_o = sphere.point.x;
	float y_o = sphere.point.y;
	float z_o = sphere.point.z;
	float R = sphere.radius;

	float A = D_x*D_x + D_y*D_y + D_z*D_z;
	float B = 2.0f * (D_x * (E_x - x_o) + D_y * (E_y - y_o) + D_z * (E_z - z_o));
	float C = E_x*E_x + E_y*E_y + E_z*E_z - 2.0f*E_x*x_o + x_o*x_o - 2.0f*E_y*y_o + y_o*y_o - 2.0f*E_z*z_o + z_o*z_o - R*R;

	return QuadraticFormula(out, A, B, C);
}

unsigned Raycast(float* out, const Ray3& ray, const AABB3& aabb3)
{
	const Vec3 mins = aabb3.mins;
	const Vec3 maxs = aabb3.maxs;

	//decompose the problem into 1D space for each axis
	float t_min_x = INFINITY;
	float t_max_x = INFINITY;
	float t_min_y = INFINITY;
	float t_max_y = INFINITY;
	float t_min_z = INFINITY;
	float t_max_z = INFINITY;

	if(!IsZero(ray.direction.x))
	{
		t_min_x = (mins.x - ray.position.x) / ray.direction.x;
		t_max_x = (maxs.x - ray.position.x) / ray.direction.x;
	}
	if(!IsZero(ray.direction.y))
	{
		t_min_y = (mins.y - ray.position.y) / ray.direction.y;
		t_max_y = (maxs.y - ray.position.y) / ray.direction.y;
	}
	if(!IsZero(ray.direction.z))
	{
		t_min_z = (mins.z - ray.position.z) / ray.direction.z;
		t_max_z = (maxs.z - ray.position.z) / ray.direction.z;
	}

	//Get the maximum min value of the Ts, and the Minimum max value of the Ts
	const float t_min = Max(
		Max(
			Min(t_min_x, t_max_x),
			Min(t_min_y, t_max_y)
		),
		Min(t_min_z, t_max_z)
	);

	const float t_max = Min(
		Min(
			Max(t_min_x, t_max_x),
			Max(t_min_y, t_max_y)
		),
		Max(t_min_z, t_max_z)
	);

	// if t_max is less than zero, then the ray is intersecting the AABB in the negative direction.
	// do not count this as an intersection
	if(t_max < 0.0f)
	{
		out[0] = INFINITY;
		out[1] = INFINITY;
		return 0;
	}

	// if t_min is greater than t_max, then the ray does not intersect with the AABB
	if(t_min > t_max)
	{
		out[0] = INFINITY;
		out[1] = INFINITY;
		return 0;
	}

	// if t_min is negative, but t_max is positive, then the ray is inside the AABB
	// and we will only have one valid intersection
	if(t_min < 0.0f)
	{
		out[0] = t_max;
		out[1] = INFINITY;
		return 1;
	}

	// we have a m_normal intersection with an AABB
	out[0] = t_min;
	out[1] = t_max;
	return 2;
}

unsigned Raycast(float* out, const Ray3& ray, const OBB3& obb3)
{
	//TODO: confirm that the math works for this
	Vec3 ray_local_position = obb3.ToLocalPoint(ray.position);
	Vec3 ray_local_direction = obb3.ToLocalVector(ray.direction);


	Ray3 local_ray(ray_local_position, ray_local_direction);
	AABB3 local_shape = obb3.GetLocalAABB3();

	return Raycast(out, local_ray, local_shape);
}

unsigned Raycast(float* out, const Ray3& ray, const Plane3& plane)
{
	float direction_projection = DotProduct(ray.direction, plane.m_normal);
	float displacement_projection = DotProduct(ray.position, plane.m_normal);

	//if the projected direction is positive, then we are face along with the plane and not intersecting it
	if (direction_projection >= 0.0f) {
		out[0] = INFINITY;
		out[1] = INFINITY;
		return 0;
	}

	float t = (plane.m_signedDistance - displacement_projection) / direction_projection;

	//if t is positive or equal zero, then we are in front of the plain
	if (t >= 0.0f) {
		out[0] = t;
		out[1] = t;
		return 1;
	}

	//otherwise, t is negative and the plain is behind us
	out[0] = INFINITY;
	out[1] = INFINITY;
	return 0;
}

//https://en.wikipedia.org/wiki/User:Nominal_animal
unsigned Raycast(float* out, const Ray3& ray, const Capsule3& capsule)
{
	// our line represented as P(t) = E + tD, t >= 0
	// which can be decomposed as 
	// X(T) = E_x + t * D_x
	// Y(T) = E_y + t * D_y
	// Z(T) = E_z + t * D_z

	// infinite cylinder as	r^2 = ||s+(((p-s).n)*n) - p||^2
		//where s is the start of the cylinder
		//and n is the axis/direction of the cylinder
		//and p is any point on the cylinder r distance away from n

	//Substituting our equation into p we get
		// r^2 = ||s+((((E + tD)-s).n)*n) - (E + tD)||^2

	//reorganizing in terms of t into quadratic equation 
		// t^2 I + t J + K = 0
		// I = B . B
		// J = 2 A . B
		// K = A . A - r^2

	// where A and B are
		// A = s + (p . n)n - (s . n)n - p
		// B = (v . n)n - v

	float contact_points_from_inf_cylinder[2];
	float contact_points_from_start_sphere[2];
	float contact_points_from_end_sphere[2];

	Vec3 capsule_direction = capsule.GetDirection().GetNormalize();

	Vec3 a = capsule.start + DotProduct(ray.position, capsule_direction)*capsule_direction - DotProduct(capsule.start, capsule_direction)*capsule_direction - ray.position;
	Vec3 b = DotProduct(ray.direction, capsule_direction)*capsule_direction - ray.direction;

	float A = DotProduct(b,b);
	float B = 2.0f * DotProduct(a,b);
	float C = DotProduct(a,a) - (capsule.radius * capsule.radius);
		
	const unsigned int num_cont_w_inf_cyl = QuadraticFormula(contact_points_from_inf_cylinder, A, B, C);

	float contactPoints[6];

	if(num_cont_w_inf_cyl != 0)
	{
		unsigned int num_count_w_cy = 0;
		//determining to keep values from cylinder if they are within the capsules length
 		const Vec3 contact_point_1 = ray.PointAtTime(contact_points_from_inf_cylinder[0]);
		const Vec3 contact_point_2 = ray.PointAtTime(contact_points_from_inf_cylinder[1]);

		const Vec3 displacement_from_cap_start_to_cp1 = contact_point_1 - capsule.start;
		float projection_of_disp_onto_cap = DotProduct(displacement_from_cap_start_to_cp1, capsule.GetDirection().GetNormalize());

		//testing if projected 
		if(projection_of_disp_onto_cap >= 0.0f && projection_of_disp_onto_cap * projection_of_disp_onto_cap <= capsule.GetLengthSquared())
		{
			contactPoints[0] = contact_points_from_inf_cylinder[0];
			num_count_w_cy++;
		}
		else contactPoints[0] = INFINITY;

		const Vec3 displacement_from_cap_start_to_cp2 = contact_point_2 - capsule.start;
		projection_of_disp_onto_cap = DotProduct(displacement_from_cap_start_to_cp2, capsule.GetDirection().GetNormalize());

		if(projection_of_disp_onto_cap >= 0.0f && projection_of_disp_onto_cap * projection_of_disp_onto_cap <= capsule.GetLengthSquared())
		{
			contactPoints[1] = contact_points_from_inf_cylinder[1];
			num_count_w_cy++;
		}
		else contactPoints[1] = INFINITY;

		const unsigned int num_cont_w_s_sph = Raycast(contact_points_from_start_sphere, ray, Sphere(capsule.start, capsule.radius));
		contactPoints[2] = contact_points_from_start_sphere[0];
		contactPoints[3] = contact_points_from_start_sphere[1];

		const unsigned int num_cont_w_e_sph = Raycast(contact_points_from_end_sphere, ray, Sphere(capsule.end, capsule.radius));
		contactPoints[4] = contact_points_from_end_sphere[0];
		contactPoints[5] = contact_points_from_end_sphere[1];

		float min = INFINITY;
		float max = 0.0f;

		if(num_count_w_cy == 2)
		{
			min = Min(contactPoints[0], min);
			max = Max(contactPoints[1], max);
		}
		else if(num_count_w_cy == 1)
		{
			min = Min(contactPoints[1], min);
		}

		min = Min(contactPoints[2], min);
		max = Max(contactPoints[3], max);
		min = Min(contactPoints[4], min);
		max = Max(contactPoints[5], max);

		out[0] = min;
		out[1] = max;

		return 2;
	}

	out[0] = INFINITY;
	out[1] = INFINITY;
	return 0;
}



