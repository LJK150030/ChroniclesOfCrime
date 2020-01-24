#include"MathUtils.hpp"
#include <cmath>
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec4.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/OBB2.hpp"
#include "Engine/Math/Capsule2.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include <vector>

typedef union {float f; int i;} IntOrFloat;

//Angle utilities
//--------------------------------------------------------------------------------------------------
float ConvertRadiansToDegrees(const float radians)
{
	return radians * RADIANS_TO_DEGREES;
}

//--------------------------------------------------------------------------------------------------
float ConvertDegreesToRadians(const float degrees)
{
	return degrees * DEGREES_TO_RADIANS;
}

//--------------------------------------------------------------------------------------------------
float CosDegrees(const float degrees)
{
	return cos(ConvertDegreesToRadians(degrees));
}

//--------------------------------------------------------------------------------------------------
float SinDegrees(const float degrees)
{
	return sin(ConvertDegreesToRadians(degrees));
}

float TanDegrees(const float radians)
{
	return tan(ConvertDegreesToRadians(radians));
}

float CosRadians(const float radians)
{
	return cos(radians);
}

float SinRadians(const float radians)
{
	return sin(radians);
}

float TanRadians(const float radians)
{
	return tan(radians);
}

//--------------------------------------------------------------------------------------------------
float Atan2Degrees(const float y, const float x)
{
	return ConvertRadiansToDegrees(atan2f(y, x));
}

float GetAngularDisplacement(const float start_degrees, const float end_degrees)
{
	float ang_disp = end_degrees - start_degrees;

	while (ang_disp > 180.f)
		ang_disp -= 360.f;

	while (ang_disp < -180.f)
		ang_disp += 360.f;

	return ang_disp;
}

float GetTurnedToward(const float current_degrees, const float goal_degrees, const float max_positive_delta_degrees)
{
	const float ang_disp = GetAngularDisplacement(current_degrees, goal_degrees);
	const float turn_degrees = ClampFloat(ang_disp, -max_positive_delta_degrees, max_positive_delta_degrees);
	return turn_degrees;
}

//Geometric utilities
//--------------------------------------------------------------------------------------------------
float GetDistance(const Vec2& position_a, const Vec2& position_b)
{
	const Vec2 direction = position_b - position_a;
	return direction.GetLength();
}

//--------------------------------------------------------------------------------------------------
float GetDistance(const Vec3& position_a, const Vec3& position_b)
{
	const Vec3 direction = position_b - position_a;
	return direction.GetLength();
}

//--------------------------------------------------------------------------------------------------
float GetDistanceXY(const Vec3& position_a, const Vec3& position_b)
{
	const Vec3 direction = position_b - position_a;
	return direction.GetLengthXY();
}

//--------------------------------------------------------------------------------------------------

float GetDistanceSquared(const Vec2& position_a, const Vec2& position_b)
{
	const Vec2 direction = position_b - position_a;
	return direction.GetLengthSquared();
}

//--------------------------------------------------------------------------------------------------
float GetDistanceSquared(const Vec3& position_a, const Vec3& position_b)
{
	const Vec3 direction = position_b - position_a;
	return direction.GetLengthSquared();
}

//--------------------------------------------------------------------------------------------------
float GetDistanceXYSquared(const Vec3& position_a, const Vec3& position_b)
{
	const Vec3 direction = position_b - position_a;
	return direction.GetLengthXYSquared();
}

//--------------------------------------------------------------------------------------------------
bool DoDiscsOverlap(const Vec2& center_a, const float radius_a, const Vec2& center_b, const float radius_b)
{
	const float i = GetDistanceSquared(center_a, center_b);
	const float j = (radius_a + radius_b) * (radius_a + radius_b);
	return i <= j;
}

bool DoesDiscOverlapAABB2D(const Vec2& disc_center, float disc_radius, const AABB2& box)
{
	const Vec2 closest_point = GetClosestPointOnAABB2(disc_center, box);
	
	Vec2 displacement_disc_box = closest_point - disc_center;
	const float length = displacement_disc_box.GetLengthSquared();

	return length < (disc_radius * disc_radius);
	// 	displacement_disc_box.SetLength(disc_radius);
	// 	Vec2 closest_point_on_aabb2 = GetClosestPointOnAABB2(displacement_disc_box, box);
}

bool DoesDiscOverlapLine2D(const Vec2& disc_center, float disc_radius, const Vec2& point_on_line,
                           const Vec2& another_point_on_line)
{
	const Vec2 line = another_point_on_line - point_on_line;
	const Vec2 i_axis = line.GetNormalized();
	const Vec2 j_axis = i_axis.GetRotated90Degrees();
	const Vec2 displacement = disc_center - point_on_line;
	const Vec2 displacement_onto_j = GetProjectedVectorAlongAxis2D(displacement, j_axis);
	return displacement_onto_j.GetLengthSquared() < disc_radius * disc_radius;
}

bool DoesDiscOverlapLineSegment2D(const Vec2& disc_center, float disc_radius, const Vec2& line_start,
                                  const Vec2& line_end)
{
	const Vec2 start_to_end = line_end - line_start;
	const Vec2 start_to_center = disc_center - line_start;

	if (DotProduct(start_to_end, start_to_center) < 0.0f)
		return IsPointInDisc2D(line_start, disc_center, disc_radius);

	const Vec2 end_to_start = line_start - line_end;
	const Vec2 end_to_center = disc_center - line_end;

	if (DotProduct(end_to_start, end_to_center) < 0.0f)
		return IsPointInDisc2D(line_end, disc_center, disc_radius);

	const Vec2 i_axis = start_to_end.GetNormalized();
	const Vec2 displacement_onto_i = DotProduct(i_axis, start_to_center) * i_axis;
	return IsPointInDisc2D(line_start + displacement_onto_i, disc_center, disc_radius);
}

bool DoesDiscOverlapCapsule2D(const Vec2& disc_center, float disc_radius, const Vec2& capsule_start,
                              const Vec2& capsule_end, float capsule_radius)
{
	const float disc_new_radius = disc_radius + capsule_radius;
	return DoesDiscOverlapLineSegment2D(disc_center, disc_new_radius, capsule_start, capsule_end);
}

bool DoesDiscOverlapOBB2D(const Vec2& disc_center, float disc_radius, const OBB2& box)
{
	const Vec2 local_center = box.ToLocalPoint(disc_center);
	return DoesDiscOverlapAABB2D(local_center, disc_radius, box.GetLocalAABB2());
}

//--------------------------------------------------------------------------------------------------
bool DoSpheresOverlap(const Vec3& center_a, const float radius_a, const Vec3& center_b, const float radius_b)
{
	const float i = GetDistanceSquared(center_a, center_b);
	const float j = (radius_a + radius_b) * (radius_a + radius_b);
	return i < j;
}

bool IsPointInSector(const Vec2& point, const Vec2& origin, const float orientation_degrees, const float max_dist,
                     const float aperture_degrees)
{
	if (!IsPointInDisc2D(point, origin, max_dist)) return false;

	Vec2 displacement_to_point = point - origin;
	const float angle_to_point_degrees = displacement_to_point.GetAngleDegrees();
	const float angular_displacement_to_point = GetAngularDisplacement(orientation_degrees, angle_to_point_degrees);
	return (fabsf(angular_displacement_to_point) <= aperture_degrees * 0.5f);
}

Vec2 GetClosestPointOnOBB2(const Vec2& reference_pos, const OBB2& box)
{
	const Vec2 local_point = box.ToLocalPoint(reference_pos);
	const Vec2 closest_local_point = GetClosestPointOnAABB2(local_point, box.GetLocalAABB2());
	Vec2 closest_world_point = box.ToWorldPoint(closest_local_point);
	return closest_world_point;
}

void GetClosestPointOnOBB2FromLineSegment(const Vec2& line_start, const Vec2& line_end, const OBB2& box, Vec2& cap_pos, Vec2& box_pos)
{
	std::vector<Vec2> start_pos = {box.GetTopLeft(), box.GetTopRight(), box.GetBottomRight(), box.GetBottomLeft()};
	std::vector<Vec2> end_pos = {box.GetTopRight(), box.GetBottomRight(), box.GetBottomLeft(), box.GetTopLeft()};

	std::vector<Vec2> line_pos = {Vec2::ZERO, Vec2::ZERO, Vec2::ZERO, Vec2::ZERO};
	std::vector<Vec2> aabb_pos = {Vec2::ZERO, Vec2::ZERO, Vec2::ZERO, Vec2::ZERO};

	std::vector<float> length_sqrd  = {0.0f, 0.0f, 0.0f, 0.0f};

	for(int line_id = 0; line_id < (int) start_pos.size(); ++line_id)
	{
		GetClosestPointsFromTwo2DLineSegments(line_pos[line_id], aabb_pos[line_id], line_start, line_end, start_pos[line_id], end_pos[line_id]);
		length_sqrd[line_id] = (aabb_pos[line_id] - line_pos[line_id]).GetLengthSquared();
	}

	int shortest_id = 0;
	for(int line_id = 1; line_id < (int) start_pos.size(); ++line_id)
	{
		if(length_sqrd[shortest_id] > length_sqrd[line_id]) shortest_id = line_id;
	}

	cap_pos = line_pos[shortest_id];
	box_pos = aabb_pos[shortest_id];
}

Vec2 GetLocalVectorFromWorld(const Vec2& world_vector, const Vec2& i_basis, const Vec2& j_basis)
{
	const float world_project_i = DotProduct(world_vector, i_basis);
	const float world_project_j = DotProduct(world_vector, j_basis);
	return Vec2(world_project_i, world_project_j);
}

Vec2 GetLocalPositionFromWorld(const Vec2& world_position, const Vec2& i_basis, const Vec2& j_basis,
                               const Vec2& t_basis)
{
	const Vec2 t_to_p = world_position - t_basis;
	const float displacement_project_i = DotProduct(t_to_p, i_basis);
	const float displacement_project_j = DotProduct(t_to_p, j_basis);
	return Vec2(displacement_project_i, displacement_project_j);
}

Vec2 GetWorldVectorFromLocal(const Vec2& local_vector, const Vec2& i_basis, const Vec2& j_basis)
{
	Vec2 world_vector = local_vector.x * i_basis + local_vector.y * j_basis;
	return world_vector;
}

Vec2 GetWorldPositionFromLocal(const Vec2& local_position, const Vec2& i_basis, const Vec2& j_basis,
                               const Vec2& t_basis)
{
	Vec2 world_position = t_basis + local_position.x * i_basis + local_position.y * j_basis;
	return world_position;
}

float SmoothStart2(const float input_zero_to_one)
{
	return input_zero_to_one * input_zero_to_one;
}

float SmoothStart3(const float input_zero_to_one)
{
	return input_zero_to_one * input_zero_to_one * input_zero_to_one;
}

float SmoothStart4(const float input_zero_to_one)
{
	return input_zero_to_one * input_zero_to_one * input_zero_to_one * input_zero_to_one;
}

float SmoothStart5(const float input_zero_to_one)
{
	return input_zero_to_one * input_zero_to_one * input_zero_to_one * input_zero_to_one * input_zero_to_one;
}

float SmoothStop2(const float input_zero_to_one)
{
	return (input_zero_to_one - 1.0f) * (input_zero_to_one - 1.0f) + 1.0f;
}

float SmoothStop3(const float input_zero_to_one)
{
	return (input_zero_to_one - 1.0f) * (input_zero_to_one - 1.0f) * (input_zero_to_one - 1.0f) + 1.0f;
}

float SmoothStop4(const float input_zero_to_one)
{
	return (input_zero_to_one - 1.0f) * (input_zero_to_one - 1.0f) * (input_zero_to_one - 1.0f) * (input_zero_to_one -
		1.0f) + 1.0f;
}

float SmoothStop5(const float input_zero_to_one)
{
	return (input_zero_to_one - 1.0f) * (input_zero_to_one - 1.0f) * (input_zero_to_one - 1.0f) * (input_zero_to_one -
		1.0f) * (input_zero_to_one - 1.0f) + 1.0f;
}

float SmoothStep3(const float input_zero_to_one)
{
	const float& t = input_zero_to_one;
	return t * t * (3.f - (2.f * t));
}

float SmoothStep5(const float input_zero_to_one)
{
	const float& t = input_zero_to_one;
	return t * t * t * (t * ((t * 6.0f) - 15.0f) + 10.0f);
}

float Mix(float first_result, float second_result, float weight)
{
	return (1.0f - weight) * first_result + weight * second_result;
}


//--------------------------------------------------------------------------------------------------
// The order of the composite transformation is first scale, then rotate, then translate
Vec2 TransformPosition(const Vec2& position, const float uniform_scale, const float rotation_degree_about_z,
                       const Vec2& translation_xy)
{
	Vec2 new_position = position * uniform_scale;
	new_position.RotateDegrees(rotation_degree_about_z);
	new_position += translation_xy;
	return new_position;
}

//--------------------------------------------------------------------------------------------------
// The order of the composite transformation is first scale, then rotate, then translate
Vec3 TransformPosition(const Vec3& position, const float uniform_scale_xy, const float rotation_degree_about_z,
                       const Vec2& translation_xy)
{
	Vec3 new_position = Vec3(position.x * uniform_scale_xy, position.y * uniform_scale_xy, position.z);
	new_position = new_position.GetRotatedAboutZDegrees(rotation_degree_about_z);
	new_position = Vec3(new_position.x + translation_xy.x, new_position.y + translation_xy.y, new_position.z);
	return new_position;
}

void TransformLocalVectorToWorld()
{
	//TODO: TransformLocalVectorToWorld
}

void TransformLocalPositionToWorld()
{
	//TODO: TransformLocalPositionToWorld
}

void TransformWorldVectorToLocal()
{
	//TODO: TransformWorldVectorToLocal
}

void TransformWorldPositionToLocal()
{
	//TODO: TransformWorldPositionToLocal
}

Vec2 MaxVec2(const Vec2& a, const Vec2& b)
{
	const float max_x = (a.x > b.x) ? a.x : b.x;
	const float max_y = (a.y > b.y) ? a.y : b.y;
	return Vec2(max_x, max_y);
}

Vec2 MinVec2(const Vec2& a, const Vec2& b)
{
	const float min_x = (a.x < b.x) ? a.x : b.x;
	const float min_y = (a.y < b.y) ? a.y : b.y;
	return Vec2(min_x, min_y);
}

float RangeMapLinearFloat(const float in_value, const float in_start, const float in_end, const float out_start,
                          const float out_end)
{
	if (in_end == in_start) return 0.5f * (out_start + out_end);

	const float in_range = in_end - in_start;
	const float out_range = out_end - out_start;
	const float in_distance = in_value - in_start;
	const float in_fraction = in_distance / in_range;
	const float out_distance = in_fraction * out_range;
	const float out_value = out_start + out_distance;
	return out_value;
}

Vec2 RangeMapLinearVec2(const Vec2& in_value, const Vec2& in_start, const Vec2& in_end, const Vec2& out_start,
	const Vec2& out_end)
{
	const float x_val = RangeMapLinearFloat(in_value.x, in_start.x, in_end.x, out_start.x, out_end.x);
	const float y_val = RangeMapLinearFloat(in_value.y, in_start.y, in_end.y, out_start.y, out_end.y);
	return Vec2(x_val, y_val);
}

Vec2 ConvertPosFromTwoAABB2S(const Vec2& in_space_pos, const AABB2& in_space, const AABB2& out_space)
{
	float x = RangeMapLinearFloat(in_space_pos.x, in_space.mins.x, in_space.maxs.x, out_space.mins.x, out_space.maxs.x);
	float y = RangeMapLinearFloat(in_space_pos.y, in_space.mins.y, in_space.maxs.y, out_space.mins.y, out_space.maxs.y);

	return Vec2(x, y);
}

float LinearInterpolationFloat(const float start_value, const float end_value, const float fraction)
{
	const float range = end_value - start_value;
	const float mapped_value = fraction * range;
	return (start_value + mapped_value);
}

Vec2 LinearInterpolationVec2(const Vec2& start_value, const Vec2& end_value, float fraction)
{
	const float range_x = end_value.x - start_value.x;
	const float mapped_value_x = fraction * range_x;
	const float result_x = start_value.x + mapped_value_x;

	const float range_y = end_value.y - start_value.y;
	const float mapped_value_y = fraction * range_y;
	const float result_y = start_value.y + mapped_value_y;

	return Vec2(result_x, result_y);
}

Vec3 LinearInterpolationVec3(const Vec3& start_value, const Vec3& end_value, const float fraction)
{
	const float range_x = end_value.x - start_value.x;
	const float mapped_value_x = fraction * range_x;
	const float result_x = start_value.x + mapped_value_x;

	const float range_y = end_value.y - start_value.y;
	const float mapped_value_y = fraction * range_y;
	const float result_y = start_value.y + mapped_value_y;

	const float range_z = end_value.z - start_value.z;
	const float mapped_value_z = fraction * range_z;
	const float result_z = start_value.z + mapped_value_z;

	return Vec3(result_x, result_y, result_z);
}

Rgba LinearInterpolationRgba(const Rgba& start_value, const Rgba& end_value, float fraction)
{
	const float range_r = end_value.r - start_value.r;
	const float mapped_value_r = fraction * range_r;
	const float result_r = start_value.r + mapped_value_r;

	const float range_g = end_value.g - start_value.g;
	const float mapped_value_g = fraction * range_g;
	const float result_g = start_value.g + mapped_value_g;

	const float range_b = end_value.b - start_value.b;
	const float mapped_value_b = fraction * range_b;
	const float result_b = start_value.b + mapped_value_b;

	const float range_a = end_value.a - start_value.a;
	const float mapped_value_a = fraction * range_a;
	const float result_a = start_value.a + mapped_value_a;

	return Rgba(result_r, result_g, result_b, result_a);
}

float GetFractionInRange(const float start_value, const float end_value, const float value)
{
	const float range = end_value - start_value;
	const float distance = value - start_value;
	return distance / range;
}

float Sign(const float value)
{
	return (value <= 0.0f) ? 1.0f : -1.0f;
}

bool IsBitFlagSet(const unsigned short bits, const unsigned short bit_flag)
{
	return (bits & bit_flag);
}

void SetBitFlag(unsigned short& bits, const unsigned short bit_flag)
{
	bits |= bit_flag;
}

unsigned short GetBitFlag(unsigned short& bits, const unsigned short bit_flag)
{
	const unsigned short set = bits;
	return set & bit_flag;
}

void ClearBitFlag(unsigned short& bits, const unsigned short bit_flag)
{
	bits &= ~bit_flag;
}

void ToggleBitFlag(unsigned short& bits, const unsigned short bit_flag)
{
	bits = bits ^ bit_flag;
}

bool IsBitFlagSet(const unsigned bits, const unsigned bit_flag)
{
	return (bits & bit_flag);
}

void SetBitFlag(unsigned& bits, const unsigned bit_flag)
{
	bits |= bit_flag;
}

unsigned GetBitFlag(unsigned& bits, const unsigned bit_flag)
{
	const unsigned int set = bits;
	return set & bit_flag;
}

void ClearBitFlag(unsigned& bits, const unsigned bit_flag)
{
	bits &= ~bit_flag;
}

float ClampFloat(const float value, const float min_value, const float max_value)
{
	if (value < min_value)
		return min_value;
	if (value > max_value)
		return max_value;
	return value;
}

double ClampDouble(const double value, const double min_value, const double max_value)
{
	if (value < min_value)
		return min_value;
	if (value > max_value)
		return max_value;
	return value;
}

float ClampFloatPositive(const float value, const float max_value)
{
	const float clamped_vale = ClampFloat(value, 0.0f, max_value);
	return clamped_vale;
}

float ClampFloatPositive(const float value)
{
	if( value < 0.f )
		return 0.f;
	else
		return value;
}

Vec2 ClampVec2(const Vec2& value, const Vec2& min_value, const Vec2& max_value)
{
	const float x_val = ClampFloat(value.x, min_value.x, max_value.x);
	const float y_val = ClampFloat(value.y, min_value.y, max_value.y);

	return Vec2(x_val, y_val);
}

int RoundToNearestInt(const float value)
{
	return static_cast<int>(floorf(value + 0.5f));
}

int RoundDownToNearestInt(const float value)
{
	return static_cast<int>(floorf(value));
}

int RoundUpToNearestInt(const float value)
{
	return static_cast<int>(ceilf(value));
}

int ModPositive(const int value, const int mod_by)
{
	int mod_value = value % mod_by;
	if (mod_value < 0)
		mod_value += mod_by;
	return mod_value;
}

float ModFloatPositive(float value, float mod_by)
{
	float mod_value = value;
	while(mod_value >= mod_by)
		mod_value -= mod_by;
	while(mod_value < 0.0f)
		mod_value += mod_by;

	return mod_value;
}

float ClampZeroToOne(const float value)
{
	return ClampFloat(value, 0.0f, 1.0f);
}

int ClampInt(const int value, const int min_value, const int max_value)
{
	if (value < min_value)
		return min_value;
	if (value > max_value)
		return max_value;
	return value;
}

int ClampIntPositive(const int value, const int max_value)
{
	const int clamped_vale = ClampInt(value, 0, max_value);
	return clamped_vale;
}

float Sqrt(const float value)
{
 	ASSERT_RECOVERABLE(value >= 0, "Attempting to Sqrt a zero value");
	return sqrtf( value );
}

float RecipSqrt(const float value)
{
	ASSERT_RECOVERABLE(value >= 0, "Attempting to Sqrt a zero value");
	return 1.0f/sqrtf( value );
}

float Abs(const float f)
{
	return fabsf(f);
}

int Abs(const int i)
{
	return abs(i);
}

bool IsZero(const float a, const float epsilon)
{
	return (Abs(a) <= epsilon);
}

bool AreEqual(const float a, const float b, const float epsilon) //assuming epsilon is less than 1.0f
{
	return (Abs(a-b) <= epsilon*(Abs(a) + Abs(b) + 1.0f));
}

bool AreEqual(const Vec2& a, const Vec2& b, const float epsilon)
{
	return AreEqual(a.x,b.x,epsilon) && AreEqual(a.y,b.y,epsilon);
}

float Min(float a, float b)
{
	return a < b ? a : b;
}

float Max(float a, float b)
{
	return a > b ? a : b;
}

int Min(int a, int b)
{
	return a < b ? a : b;
}

int Max(int a, int b)
{
	return a > b ? a : b;
}

//https://en.wikipedia.org/wiki/Quadratic_formula
unsigned QuadraticFormula(float* out, float A, float B, float C)
{
	// x = (-b +- sqrt(b^2 - 4 * a * c))/(2 * a)
	// return values:
	// 2 - two real roots found
	// 1 - one real root found
	// 0 - no real roots

	const float discriminant = (B * B) - 4.0f * A * C;

	if(discriminant > 0.0f)
	{
		// 2 - two real roots found
		out[0] = (-1.0f * B + Sqrt(discriminant)) / (2.0f * A);
		out[1] = (-1.0f * B - Sqrt(discriminant)) / (2.0f * A);
		return 2;
	}

	if(IsZero(discriminant))
	{
		// 1 - one real root found
		out[0] = (-1.0f * B) / (2.0f * A);
		out[1] = out[0];
		return 1;
	}

	// else discriminant < 0.0f
	out[0] = INFINITY;
	out[1] = INFINITY;
	return 0;
}


float DotProduct(const Vec2& a, const Vec2& b)
{
	return (a.x * b.x) + (a.y * b.y);
}

float DotProduct(const Vec3& a, const Vec3& b)
{
	return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}

float DotProduct(const Vec4& a, const Vec4& b)
{
	return (a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w);
}

float GetProjectedLengthAlongAxis2D(const Vec2& source, const Vec2& normalized_axis)
{
	return DotProduct(source, normalized_axis);
}

Vec2 GetProjectedVectorAlongAxis2D(const Vec2& source, const Vec2& axis)
{
	// 	const float projected_length = GetProjectedLengthAlongAxis2D(source, normalized_axis);
	// 	return normalized_axis * projected_length;
	const float dot_axis = DotProduct(axis, axis);
	const Vec2 normalized_axis = axis / dot_axis;
	const float dot_product = DotProduct(source, axis);
	return normalized_axis * dot_product;
}

Vec2 GetReflectedVector(const Vec2& reflect_this, const Vec2& point_normal)
{
	const Vec2 projected_vector_against_normal = GetProjectedVectorAlongAxis2D(reflect_this, point_normal);
	const Vec2 projected_vector_against_wall = reflect_this - projected_vector_against_normal;
	return projected_vector_against_wall + (-1 * projected_vector_against_normal);
}

Vec2 ReflectVectorOffSurfaceNormal(const Vec2& incoming_vector, const Vec2& surface_normal)
{
	const Vec2 incoming_vector_n = GetProjectedVectorAlongAxis2D(incoming_vector, surface_normal);
	const Vec2 incoming_vector_t = incoming_vector - incoming_vector_n;
	return incoming_vector_t + (-1.0f * incoming_vector_n);
}

float GetRayImpactFractionVsDisc2D(const Vec2& ray_start, const Vec2& ray_normalized_dir, float ray_length,
                                   const Vec2& disc_center, float disc_radius)
{
	UNUSED(ray_start);
	UNUSED(ray_normalized_dir);
	UNUSED(ray_length);
	UNUSED(disc_center);
	UNUSED(disc_radius);

	//TODO: GetRayImpactFractionVsDisc2D
	return 0.0f;
}

Vec3 Cross(const Vec3& a, const Vec3& b)
{
	return Vec3(a.y*b.z - b.y*a.z, a.z*b.x - b.z*a.x, a.x*b.y - b.x*a.y);
}

Vec2 GetClosestPointOnAABB2(const Vec2& reference_pos, const AABB2& box)
{
	const float closest_x = ClampFloat(reference_pos.x, box.mins.x, box.maxs.x);
	const float closest_y = ClampFloat(reference_pos.y, box.mins.y, box.maxs.y);
	return Vec2(closest_x, closest_y);
}

void GetClosestPointOnAABB2FromLineSegment(const Vec2& line_start, const Vec2& line_end, const AABB2& box, Vec2& point_start, Vec2& point_end)
{
	std::vector<Vec2> start_pos = {box.GetTopLeft(), box.GetTopRight(), box.GetBottomRight(), box.GetBottomLeft()};
	std::vector<Vec2> end_pos = {box.GetTopRight(), box.GetBottomRight(), box.GetBottomLeft(), box.GetTopLeft()};

	std::vector<Vec2> line_pos = {Vec2::ZERO, Vec2::ZERO, Vec2::ZERO, Vec2::ZERO};
	std::vector<Vec2> aabb_pos = {Vec2::ZERO, Vec2::ZERO, Vec2::ZERO, Vec2::ZERO};

	std::vector<float> length_sqrd  = {0.0f, 0.0f, 0.0f, 0.0f};
	
	for(int line_id = 0; line_id < (int) start_pos.size(); ++line_id)
	{
		GetClosestPointsFromTwo2DLineSegments(line_pos[line_id], aabb_pos[line_id], line_start, line_end, start_pos[line_id], end_pos[line_id]);
		length_sqrd[line_id] = (aabb_pos[line_id] - line_pos[line_id]).GetLengthSquared();
	}

	int shortest_id = 0;
	for(int line_id = 1; line_id < (int) start_pos.size(); ++line_id)
	{
		if(length_sqrd[shortest_id] > length_sqrd[line_id]) shortest_id = line_id;
	}

	point_start = line_pos[shortest_id];
	point_end = aabb_pos[shortest_id];
}

bool IsPointInAABB2(const Vec2& reference_pos, const AABB2& box)
{
	return (box.mins.x <= reference_pos.x && reference_pos.x <= box.maxs.x) && 
		(box.mins.y <= reference_pos.y && reference_pos.y <= box.maxs.y);
}

bool DoesAABB2OverlapLineSegment2D(const AABB2& box, const Vec2& line_start, const Vec2& line_end)
{
	if (IsPointInAABB2(line_start, box) ||	IsPointInAABB2(line_end, box)) {
		return true;
	}

	Vec2 norm = line_end - line_start;
	norm.Normalize();
	norm.x = (!IsZero(norm.x)) ? 1.0f / norm.x : 0.0f;
	norm.y = (!IsZero(norm.y)) ? 1.0f / norm.y : 0.0f;
	Vec2 min = box.mins - line_start;
	Vec2 max = box.maxs - line_start;
	min = DotProduct(min, norm) * norm;
	max = DotProduct(max, norm) * norm;
	float t_min = fmaxf(
		fminf(min.x, max.x),
		fminf(min.y, max.y)
	);
	float t_max = fminf(
		fmaxf(min.x, max.x),
		fmaxf(min.y, max.y)
	);
	if (t_max< 0 || t_min>t_max) {
		return false;
	}
	float t = (t_min< 0.0f) ? t_max : t_min;
	return t > 0.0f && t * t < (line_start.x * line_start.x + line_end.y * line_end.y);
}


Vec2 GetClosestPointOnDisc(const Vec2& position, const Vec2& disc_center, const float disc_radius)
{
	Vec2 pc = position - disc_center;
	pc.ClampLength(disc_radius);
	return disc_center + pc;
}

bool IsPointInDisc2D(const Vec2& position, const Vec2& origin, float disc_radius)
{
	Vec2 displacement = origin - position;
	const float length_squared = displacement.GetLengthSquared();
	const float disc_radius_squared = disc_radius * disc_radius;
	return length_squared <= disc_radius_squared;
}

bool DoAABB2SOverlap(const AABB2& first_box, const AABB2& second_box)
{
	AABB2 intersection = first_box.GetIntersection(second_box);
	return	first_box.IsPointInside(intersection.GetCenter());
}

void PushDiscOutOfDisc(const Vec2& stationary_disc_center, const float stationary_disc_radius, Vec2& mobile_disc_center,
                       const float mobile_disc_radius)
{
	if(DoDiscsOverlap(stationary_disc_center, stationary_disc_radius, mobile_disc_center, mobile_disc_radius))
	{
		Vec2 displacment = mobile_disc_center - stationary_disc_center;
		displacment.SetLength((mobile_disc_radius + stationary_disc_radius) - displacment.GetLength());
		mobile_disc_center += displacment;
	}
}

// assuming they have already overlapped
void PushDiscsOutOfEachOther(Vec2& disc_1_center, const float disc_1_radius, Vec2& disc_2_center,
                             const float disc_2_radius)
{
	Vec2 displacement = Vec2(disc_2_center - disc_1_center);
	const float length_from_discs = displacement.GetLength();
	const float summation_of_disc_radii = disc_1_radius + disc_2_radius;
	const float overlap = summation_of_disc_radii - length_from_discs;

	if (overlap <= 0) return;
	displacement.Normalize();
	disc_1_center += displacement * (-0.5f * overlap);
	disc_2_center += displacement * (0.5f * overlap);
}

void PushDiscOutOfAABB2(const AABB2& stationary_block, Vec2& mobile_disc_center, float mobile_disc_radius)
{
	Vec2 displacement = stationary_block.GetCenter() - mobile_disc_center;
	displacement.SetLength(mobile_disc_radius);
	const Vec2 closest_point_aabb2 = GetClosestPointOnAABB2(displacement, stationary_block);
	PushDiscOutOfPoint(closest_point_aabb2, mobile_disc_center, mobile_disc_radius);
}

void PushDiscOutOfPoint(const Vec2& position, Vec2& disc_center, const float disc_radius)
{
	const Vec2 closest_point = GetClosestPointOnDisc(position, disc_center, disc_radius);
	const Vec2 displacement_from_cp_to_center = disc_center - closest_point;
	const float force_amount = disc_radius - displacement_from_cp_to_center.GetLength();
	const float force_direction = displacement_from_cp_to_center.GetAngleDegrees();
	disc_center += Vec2::MakeFromPolarDegrees(force_direction, force_amount);
}

Vec2 GetClosestPointOnLine2D(const Vec2& reference_pos, const Vec2& point_on_line, const Vec2& another_point_on_line)
{
//	Guildhall MP1
// 	const Vec2 line = another_point_on_line - point_on_line;
// 	const Vec2 i = line.GetNormalized();
// 	const Vec2 sp = reference_pos - point_on_line;
// 	const Vec2 p_i = DotProduct(i, sp) * i;
// 	return point_on_line + p_i;

	//essential mathematics 3rd ed
	const Vec2 w = reference_pos  - point_on_line;
	const Vec2 line_direction = another_point_on_line - point_on_line;
	const float line_mag_sq = DotProduct(line_direction, line_direction);
	const float project = DotProduct(w, line_direction);
	return point_on_line + (project/line_mag_sq)*line_direction;
}

Vec3 GetClosestPointOnLine3D(const Vec3& reference_pos, const Vec3& point_on_line, const Vec3& another_point_on_line)
{
	//essential mathematics 3rd ed
	const Vec3 origin_to_point = reference_pos  - point_on_line;
	const Vec3 line_direction = another_point_on_line - point_on_line;
	const float line_mag_sq = DotProduct(line_direction, line_direction);
	const float project = DotProduct(origin_to_point, line_direction);
	return point_on_line + (project/line_mag_sq)*line_direction;
}

float GetDistanceSquaredFromLine2D(const Vec2& reference_pos, const Vec2& point_on_line, const Vec2& another_point_on_line)
{
	//essential mathematics 3rd ed
	const Vec2 origin_to_point = reference_pos - point_on_line;
	const Vec2 line_direction = another_point_on_line - point_on_line;
	const float line_mag_sq = DotProduct(line_direction, line_direction);
	const float o2p_line_mag_sq = DotProduct(origin_to_point, origin_to_point);
	const float projection = DotProduct(origin_to_point, line_direction);
	return o2p_line_mag_sq - projection * projection / line_mag_sq;
}

float GetDistanceSquaredFromLine3D(const Vec3& reference_pos, const Vec3& point_on_line, const Vec3& another_point_on_line)
{
	//essential mathematics 3rd ed
	const Vec3 origin_to_point = reference_pos - point_on_line;
	const Vec3 line_direction = another_point_on_line - point_on_line;
	const float line_mag_sq = DotProduct(line_direction, line_direction);
	const float o2p_line_mag_sq = DotProduct(origin_to_point, origin_to_point);
	const float projection = DotProduct(origin_to_point, line_direction);
	return o2p_line_mag_sq - projection * projection / line_mag_sq;
}

Vec2 GetClosestPointOnLineSegment2D(const Vec2& reference_pos, const Vec2& line_start, const Vec2& line_end)
{
//	Guildhall MP1
// 	Vec2 se = line_end - line_start;
// 
// 	se.Normalize();
// 	const Vec2 sp = reference_pos - line_start;
// 
// 	if (DotProduct(se, sp) < 0)
// 		return line_start;
// 
// 	const Vec2 es = line_start - line_end;
// 	const Vec2 ep = reference_pos - line_end;
// 
// 	if (DotProduct(es, ep) < 0)
// 		return line_end;
// 
// 	const Vec2 p_i = DotProduct(se, sp) * se;
// 	return line_start + p_i;

//	essential mathematics 3rd ed
	const Vec2 origin_to_point =  reference_pos - line_start;
	const Vec2 line_direction = line_end - line_start;
	const float projection = DotProduct(origin_to_point, line_direction);

	if( projection <= 0.0f )
	{
		return line_start;
	}

	const float line_mag_sq = DotProduct(line_direction, line_direction);

	if(projection >= line_mag_sq)
	{
		return line_end;
	}

	return line_start + (projection/line_mag_sq) * line_direction;
}

Vec3 GetClosestPointOnLineSegment3D(const Vec3& reference_pos, const Vec3& line_start, const Vec3& line_end)
{
	//	essential mathematics 3rd ed
	const Vec3 origin_to_point =  reference_pos - line_start;
	const Vec3 line_direction = line_end - line_start;
	const float projection = DotProduct(origin_to_point, line_direction);

	if( projection <= 0.0f )
	{
		return line_start;
	}

	const float line_mag_sq = DotProduct(line_direction, line_direction);

	if(projection >= line_mag_sq)
	{
		return line_end;
	}

	return line_start + (projection/line_mag_sq) * line_direction;
}

float GetDistanceSquaredFromLineSegment2D(const Vec2& reference_pos, const Vec2& line_start, const Vec2& line_end)
{
	//	essential mathematics 3rd ed
	const Vec2 origin_to_point =  reference_pos - line_start;
	const Vec2 line_direction = line_end - line_start;
	float projection = DotProduct(origin_to_point, line_direction);

	if( projection <= 0.0f )
	{
		return DotProduct(origin_to_point, origin_to_point);
	}

	const float line_mag_sq = DotProduct(line_direction, line_direction);

	if(projection >= line_mag_sq)
	{
		return DotProduct(origin_to_point, origin_to_point) - 2.0f * projection + line_mag_sq;
	}

	return DotProduct(origin_to_point, origin_to_point) - projection * projection / line_mag_sq;
}

float GetDistanceSquaredFromLineSegment3D(const Vec3& reference_pos, const Vec3& line_start, const Vec3& line_end)
{
	//	essential mathematics 3rd ed
	const Vec3 origin_to_point =  reference_pos - line_start;
	const Vec3 line_direction = line_end - line_start;
	float projection = DotProduct(origin_to_point, line_direction);

	if( projection <= 0.0f )
	{
		return DotProduct(origin_to_point, origin_to_point);
	}

	const float line_mag_sq = DotProduct(line_direction, line_direction);

	if(projection >= line_mag_sq)
	{
		return DotProduct(origin_to_point, origin_to_point) - 2.0f * projection + line_mag_sq;
	}

	return DotProduct(origin_to_point, origin_to_point) - projection * projection / line_mag_sq;

}

void GetClosestPointsFromTwo2DLines(Vec2& point_1, Vec2& point_2, const Vec2& line_1_start,
	const Vec2& line_1_end, const Vec2& line_2_start, const Vec2& line_2_end)
{
	//	essential mathematics 3rd ed
	const Vec2 w_0 = line_1_start - line_2_start;
	const Vec2 line_1_direction = line_1_end - line_1_start;
	const Vec2 line_2_direction = line_2_end - line_2_start;

	const float a = DotProduct(line_1_direction, line_1_direction);
	const float b = DotProduct(line_1_direction, line_2_direction);
	const float c = DotProduct(line_2_direction, line_2_direction);
	const float d = DotProduct(line_1_direction, w_0);
	const float e = DotProduct(line_2_direction, w_0);

	const float denom = a*c - b*b;

	if(IsZero(denom))
	{
		point_1 = line_1_start;
		point_2 = line_2_start + (e/c) * line_2_direction;
	}
	else
	{
		point_1 = line_1_start + ((b*e - c*d)/denom)*line_1_direction;
		point_2 = line_2_start + ((a*e - b*d)/denom)*line_2_direction;
	}
}

void GetClosestPointsFromTwo3DLines(Vec3& point_1, Vec3& point_2, const Vec3& line_1_start,
	const Vec3& line_1_end, const Vec3& line_2_start, const Vec3& line_2_end)
{
	//	essential mathematics 3rd ed
	const Vec3 w_0 = line_1_start - line_2_start;
	const Vec3 line_1_direction = line_1_end - line_1_start;
	const Vec3 line_2_direction = line_2_end - line_2_start;

	const float a = DotProduct(line_1_direction, line_1_direction);
	const float b = DotProduct(line_1_direction, line_2_direction);
	const float c = DotProduct(line_2_direction, line_2_direction);
	const float d = DotProduct(line_1_direction, w_0);
	const float e = DotProduct(line_2_direction, w_0);

	const float denom = a*c - b*b;

	if(IsZero(denom))
	{
		point_1 = line_1_start;
		point_2 = line_2_start + (e/c) * line_2_direction;
	}
	else
	{
		point_1 = line_1_start + ((b*e - c*d)/denom)*line_1_direction;
		point_2 = line_2_start + ((a*e - b*d)/denom)*line_2_direction;
	}
}

void GetClosestPointsFromTwo2DLineSegments(Vec2& point_1, Vec2& point_2, const Vec2& line_1_start,
	const Vec2& line_1_end, const Vec2& line_2_start, const Vec2& line_2_end)
{
	// compute intermediate parameters
	Vec2 w_0 = line_1_start - line_2_start;
	const Vec2 line_1_direction = line_1_end - line_1_start;
	const Vec2 line_2_direction = line_2_end - line_2_start;

	const float a = DotProduct(line_1_direction, line_1_direction);
	const float b = DotProduct(line_1_direction, line_2_direction);
	const float c = DotProduct(line_2_direction, line_2_direction);
	const float d = DotProduct(line_1_direction, w_0);
	const float e = DotProduct(line_2_direction, w_0);

	const float denom = a*c - b*b;
	// parameters to compute s_c, t_c
	float s_c, t_c;
	float sn, sd, tn, td;

	// if denom is zero, try finding closest point on segment1 to origin0
	if ( IsZero(denom) )
	{
		// clamp s_c to 0
		sd = td = c;
		sn = 0.0f;
		tn = e;
	}
	else
	{
		// clamp s_c within [0,1]
		sd = td = denom;
		sn = b*e - c*d;
		tn = a*e - b*d;

		// clamp s_c to 0
		if (sn < 0.0f)
		{
			sn = 0.0f;
			tn = e;
			td = c;
		}
		// clamp s_c to 1
		else if (sn > sd)
		{
			sn = sd;
			tn = e + b;
			td = c;
		}
	}

	// clamp t_c within [0,1]
	// clamp t_c to 0
	if (tn < 0.0f)
	{
		t_c = 0.0f;
		// clamp s_c to 0
		if ( -d < 0.0f )
		{
			s_c = 0.0f;
		}
		// clamp s_c to 1
		else if ( -d > a )
		{
			s_c = 1.0f;
		}
		else
		{
			s_c = -d/a;
		}
	}
	// clamp t_c to 1
	else if (tn > td)
	{
		t_c = 1.0f;
		// clamp s_c to 0
		if ( (-d+b) < 0.0f )
		{
			s_c = 0.0f;
		}
		// clamp s_c to 1
		else if ( (-d+b) > a )
		{
			s_c = 1.0f;
		}
		else
		{
			s_c = (-d+b)/a;
		}
	}
	else
	{
		t_c = tn/td;
		s_c = sn/sd;
	}

	// compute closest points
	point_1 = line_1_start + s_c*line_1_direction;
	point_2 = line_2_start + t_c*line_2_direction;
}

void GetClosestPointsFromTwo3DLineSegments(Vec3& point_1, Vec3& point_2, const Vec3& line_1_start,
	const Vec3& line_1_end, const Vec3& line_2_start, const Vec3& line_2_end)
{
	// compute intermediate parameters
	Vec3 w_0 = line_1_start - line_2_start;
	const Vec3 line_1_direction = line_1_end - line_1_start;
	const Vec3 line_2_direction = line_2_end - line_2_start;

	const float a = DotProduct(line_1_direction, line_1_direction);
	const float b = DotProduct(line_1_direction, line_2_direction);
	const float c = DotProduct(line_2_direction, line_2_direction);
	const float d = DotProduct(line_1_direction, w_0);
	const float e = DotProduct(line_2_direction, w_0);

	const float denom = a*c - b*b;
	// parameters to compute s_c, t_c
	float s_c, t_c;
	float sn, sd, tn, td;

	// if denom is zero, try finding closest point on segment1 to origin0
	if ( IsZero(denom) )
	{
		// clamp s_c to 0
		sd = td = c;
		sn = 0.0f;
		tn = e;
	}
	else
	{
		// clamp s_c within [0,1]
		sd = td = denom;
		sn = b*e - c*d;
		tn = a*e - b*d;

		// clamp s_c to 0
		if (sn < 0.0f)
		{
			sn = 0.0f;
			tn = e;
			td = c;
		}
		// clamp s_c to 1
		else if (sn > sd)
		{
			sn = sd;
			tn = e + b;
			td = c;
		}
	}

	// clamp t_c within [0,1]
	// clamp t_c to 0
	if (tn < 0.0f)
	{
		t_c = 0.0f;
		// clamp s_c to 0
		if ( -d < 0.0f )
		{
			s_c = 0.0f;
		}
		// clamp s_c to 1
		else if ( -d > a )
		{
			s_c = 1.0f;
		}
		else
		{
			s_c = -d/a;
		}
	}
	// clamp t_c to 1
	else if (tn > td)
	{
		t_c = 1.0f;
		// clamp s_c to 0
		if ( (-d+b) < 0.0f )
		{
			s_c = 0.0f;
		}
		// clamp s_c to 1
		else if ( (-d+b) > a )
		{
			s_c = 1.0f;
		}
		else
		{
			s_c = (-d+b)/a;
		}
	}
	else
	{
		t_c = tn/td;
		s_c = sn/sd;
	}

	// compute closest points
	point_1 = line_1_start + s_c*line_1_direction;
	point_2 = line_2_start + t_c*line_2_direction;
}

float GetDistanceSquaredFromTwo2DLineSegments(float& length_sq_from_l1_start, float& length_sq_from_l2_start,
	const Vec2& line_1_start, const Vec2& line_1_end, const Vec2& line_2_start, const Vec2& line_2_end)
{
	// compute intermediate parameters
	const Vec2 w_0 = line_1_start - line_2_start;
	const Vec2 line_1_direction = line_1_end - line_1_start;
	const Vec2 line_2_direction = line_2_end - line_2_start;

	const float a = DotProduct(line_1_direction, line_1_direction);
	const float b = DotProduct(line_1_direction, line_2_direction);
	const float c = DotProduct(line_2_direction, line_2_direction);
	const float d = DotProduct(line_1_direction, w_0);
	const float e = DotProduct(line_2_direction, w_0);

	const float denom = a*c - b*b;
	// parameters to compute s_c, t_c
	float sn, sd, tn, td;

	// if denom is zero, try finding closest point on segment1 to origin0
	if ( IsZero(denom) )
	{
		// clamp s_c to 0
		sd = td = c;
		sn = 0.0f;
		tn = e;
	}
	else
	{
		// clamp s_c within [0,1]
		sd = td = denom;
		sn = b*e - c*d;
		tn = a*e - b*d;

		// clamp s_c to 0
		if (sn < 0.0f)
		{
			sn = 0.0f;
			tn = e;
			td = c;
		}
		// clamp s_c to 1
		else if (sn > sd)
		{
			sn = sd;
			tn = e + b;
			td = c;
		}
	}

	// clamp t_c within [0,1]
	// clamp t_c to 0
	if (tn < 0.0f)
	{
		length_sq_from_l2_start = 0.0f;
		// clamp s_c to 0
		if ( -d < 0.0f )
		{
			length_sq_from_l1_start = 0.0f;
		}
		// clamp s_c to 1
		else if ( -d > a )
		{
			length_sq_from_l1_start = 1.0f;
		}
		else
		{
			length_sq_from_l1_start = -d/a;
		}
	}
	// clamp t_c to 1
	else if (tn > td)
	{
		length_sq_from_l2_start = 1.0f;
		// clamp s_c to 0
		if ( (-d+b) < 0.0f )
		{
			length_sq_from_l1_start = 0.0f;
		}
		// clamp s_c to 1
		else if ( (-d+b) > a )
		{
			length_sq_from_l1_start = 1.0f;
		}
		else
		{
			length_sq_from_l1_start = (-d+b)/a;
		}
	}
	else
	{
		length_sq_from_l2_start = tn/td;
		length_sq_from_l1_start = sn/sd;
	}

	// compute difference vector and distance squared
	const Vec2 wc = w_0 + length_sq_from_l1_start*line_1_direction - length_sq_from_l2_start*line_2_direction;
	return DotProduct(wc,wc);
}

float GetDistanceSquaredFromTwo3DLineSegments(float& length_sq_from_l1_start, float& length_sq_from_l2_start,
	const Vec3& line_1_start, const Vec3& line_1_end, const Vec3& line_2_start, const Vec3& line_2_end)
{
	// compute intermediate parameters
	const Vec3 w_0 = line_1_start - line_2_start;
	const Vec3 line_1_direction = line_1_end - line_1_start;
	const Vec3 line_2_direction = line_2_end - line_2_start;

	const float a = DotProduct(line_1_direction, line_1_direction);
	const float b = DotProduct(line_1_direction, line_2_direction);
	const float c = DotProduct(line_2_direction, line_2_direction);
	const float d = DotProduct(line_1_direction, w_0);
	const float e = DotProduct(line_2_direction, w_0);

	const float denom = a*c - b*b;
	// parameters to compute s_c, t_c
	float sn, sd, tn, td;

	// if denom is zero, try finding closest point on segment1 to origin0
	if ( IsZero(denom) )
	{
		// clamp s_c to 0
		sd = td = c;
		sn = 0.0f;
		tn = e;
	}
	else
	{
		// clamp s_c within [0,1]
		sd = td = denom;
		sn = b*e - c*d;
		tn = a*e - b*d;

		// clamp s_c to 0
		if (sn < 0.0f)
		{
			sn = 0.0f;
			tn = e;
			td = c;
		}
		// clamp s_c to 1
		else if (sn > sd)
		{
			sn = sd;
			tn = e + b;
			td = c;
		}
	}

	// clamp t_c within [0,1]
	// clamp t_c to 0
	if (tn < 0.0f)
	{
		length_sq_from_l2_start = 0.0f;
		// clamp s_c to 0
		if ( -d < 0.0f )
		{
			length_sq_from_l1_start = 0.0f;
		}
		// clamp s_c to 1
		else if ( -d > a )
		{
			length_sq_from_l1_start = 1.0f;
		}
		else
		{
			length_sq_from_l1_start = -d/a;
		}
	}
	// clamp t_c to 1
	else if (tn > td)
	{
		length_sq_from_l2_start = 1.0f;
		// clamp s_c to 0
		if ( (-d+b) < 0.0f )
		{
			length_sq_from_l1_start = 0.0f;
		}
		// clamp s_c to 1
		else if ( (-d+b) > a )
		{
			length_sq_from_l1_start = 1.0f;
		}
		else
		{
			length_sq_from_l1_start = (-d+b)/a;
		}
	}
	else
	{
		length_sq_from_l2_start = tn/td;
		length_sq_from_l1_start = sn/sd;
	}

	// compute difference vector and distance squared
	const Vec3 wc = w_0 + length_sq_from_l1_start*line_1_direction - length_sq_from_l2_start*line_2_direction;
	return DotProduct(wc,wc);
}

float GetDistanceSquaredFromTwo2DLines(const Vec2& line_1_start, const Vec2& line_1_end,
	const Vec2& line_2_start, const Vec2& line_2_end)
{
	const Vec2 w_0 = line_1_start - line_2_start;
	const Vec2 line_1_direction = line_1_end - line_1_start;
	const Vec2 line_2_direction = line_2_end - line_2_start;

	const float a = DotProduct(line_1_direction, line_1_direction);
	const float b = DotProduct(line_1_direction, line_2_direction);
	const float c = DotProduct(line_2_direction, line_2_direction);
	const float d = DotProduct(line_1_direction, w_0);
	const float e = DotProduct(line_2_direction, w_0);

	const float denom = a*c - b*b;

	if( IsZero(denom) )
	{
		const Vec2 w_c = w_0 - (e/c) * line_2_direction;
		return DotProduct(w_c, w_c);
	}

	const Vec2 w_c = w_0 + ((b*e - c*d)/denom) * line_1_direction - ((a*e - b*d)/denom)*line_2_direction;
	return DotProduct(w_c, w_c);
}

float GetDistanceSquaredFromTwo3DLines(const Vec3& line_1_start, const Vec3& line_1_end,
	const Vec3& line_2_start, const Vec3& line_2_end)
{
	const Vec3 w_0 = line_1_start - line_2_start;
	const Vec3 line_1_direction = line_1_end - line_1_start;
	const Vec3 line_2_direction = line_2_end - line_2_start;

	const float a = DotProduct(line_1_direction, line_1_direction);
	const float b = DotProduct(line_1_direction, line_2_direction);
	const float c = DotProduct(line_2_direction, line_2_direction);
	const float d = DotProduct(line_1_direction, w_0);
	const float e = DotProduct(line_2_direction, w_0);

	const float denom = a*c - b*b;

	if( IsZero(denom) )
	{
		const Vec3 w_c = w_0 - (e/c) * line_2_direction;
		return DotProduct(w_c, w_c);
	}

	const Vec3 w_c = w_0 + ((b*e - c*d)/denom) * line_1_direction - ((a*e - b*d)/denom)*line_2_direction;
	return DotProduct(w_c, w_c);
}

Vec2 GetClosestPointOnCapsule2D(const Vec2& reference_pos, const Vec2& capsule_start, const Vec2& capsule_end,
                                float capsule_radius)
{
	Vec2 se = capsule_end - capsule_start;

	se.Normalize();
	const Vec2 sp = reference_pos - capsule_start;

	if (DotProduct(se, sp) < 0)
		return GetClosestPointOnDisc(reference_pos, capsule_start, capsule_radius);

	const Vec2 es = capsule_start - capsule_end;
	const Vec2 ep = reference_pos - capsule_end;

	if (DotProduct(es, ep) < 0)
		return GetClosestPointOnDisc(reference_pos, capsule_end, capsule_radius);

	const Vec2 j = se.GetRotated90Degrees();
	const Vec2 p_j = DotProduct(j, sp) * j;
	const Vec2 p_i = sp - p_j;

	if (p_j.GetLengthSquared() > capsule_radius * capsule_radius)
		return capsule_start + p_i + (j * capsule_radius);
	return reference_pos;
}

bool IsPointInCapsule2D(const Vec2& point, const Vec2& capsule_start, const Vec2& capsule_end, float capsule_radius)
{
	return DoesDiscOverlapLineSegment2D(point, capsule_radius, capsule_start, capsule_end);
}

////////////////////////////////////////////////////////////////////////////////////////////

//Interval math
Interval2D GetInterval(const AABB2& box, const Vec2& axis)
{
	Interval2D result;

	//Get all verts of the obj
	std::vector<Vec2> verts = { box.GetBottomLeft(), box.GetBottomRight(), box.GetTopLeft(), box.GetTopRight()};

	result.m_min = result.m_max = DotProduct(axis, verts[0]);
	for (int i = 1; i < 4; ++i) {
		const float projection = DotProduct(axis, verts[i]);
		if (projection < result.m_min) {
			result.m_min = projection;
		}
		if (projection > result.m_max) {
			result.m_max = projection;
		}
	}

	//Vec2 center = box.GetCenter();
	//Vec2 axis_normal = axis.GetRotatedMinus90Degrees();
	//Vec2 interval_center = center + axis_normal * 10.0f;
	//Vec2 interval_begin = axis * result.m_min + interval_center;
	//Vec2 interval_end = axis * result.m_max + interval_center;

// 	g_theDebugRenderer->DebugRenderScreenLine(0.0f, center, interval_center, Rgba::CYAN, Rgba::CYAN, 0.1f);
// 	g_theDebugRenderer->DebugRenderScreenLine(0.0f, interval_begin, interval_end, Rgba::WHITE, Rgba::CYAN, 0.1f);

	return result;
}

Interval2D GetInterval(const OBB2& box, const Vec2& axis)
{
	Interval2D result;

	std::vector<Vec2> verts = { box.GetBottomLeft(), box.GetBottomRight(), box.GetTopLeft(), box.GetTopRight()};

	result.m_min = result.m_max = DotProduct(axis, verts[0]);
	for (int i = 1; i < 4; ++i) {
		const float projection = DotProduct(axis, verts[i]);
		if (projection < result.m_min) {
			result.m_min = projection;
		}
		if (projection > result.m_max) {
			result.m_max = projection;
		}
	}

	return result;
}

Interval2D GetInterval(const Capsule2& capsule, const Vec2& axis)
{
	Interval2D result_1;
	Interval2D result_2;

	result_1.m_min = result_1.m_max = DotProduct(axis, capsule.m_p0);

	result_1.m_min = result_1.m_min - capsule.m_radius;
	result_1.m_max = result_1.m_max + capsule.m_radius;

	result_2.m_min = result_2.m_max = DotProduct(axis, capsule.m_p1);

	result_2.m_min = result_2.m_min - capsule.m_radius;
	result_2.m_max = result_2.m_max + capsule.m_radius;

	if(result_1.m_min > result_2.m_min) result_1.m_min = result_2.m_min;
	if(result_1.m_max < result_2.m_max) result_1.m_max = result_2.m_max;

	return result_1;
}

Interval2D GetInterval(const Vec2& disc_pos, float disc_radius, const Vec2& axis)
{
	Interval2D result;

	result.m_min = result.m_max = DotProduct(axis, disc_pos);
	result.m_min = result.m_min - disc_radius;
	result.m_max = result.m_max + disc_radius;

	return result;
}

bool OverlapOnAxis(const AABB2& box_1, const AABB2& box_2, const Vec2& axis)
{
	const Interval2D a = GetInterval(box_1, axis);
	const Interval2D b = GetInterval(box_2, axis);

	return ((b.m_min <= a.m_max) && (a.m_min <= b.m_max));
}

bool OverlapOnAxis(const AABB2& aabb, const OBB2& obb, const Vec2& axis)
{
	Interval2D a = GetInterval(aabb, axis);
	Interval2D b = GetInterval(obb, axis);

	return ((b.m_min <= a.m_max) && (a.m_min <= b.m_max));
}

bool OverlapOnAxis(const AABB2& aabb, const Capsule2& capsule2, const Vec2& axis)
{
	Interval2D a = GetInterval(aabb, axis);
	Interval2D b = GetInterval(capsule2, axis);

	return ((b.m_min <= a.m_max) && (a.m_min <= b.m_max));
}

bool OverlapOnAxis(const OBB2& obb, const Capsule2& capsule, const Vec2& axis)
{
	Interval2D a = GetInterval(obb, axis);
	Interval2D b = GetInterval(capsule, axis);

	return ((b.m_min <= a.m_max) && (a.m_min <= b.m_max));
}

bool OverlapOnAxis(const AABB2& aabb, const Vec2& disc_pos, float disc_radius)
{
	Vec2 closest_point = GetClosestPointOnAABB2(disc_pos, aabb);
	Vec2 axis = closest_point - disc_pos;
	axis.Normalize();

	Interval2D a = GetInterval(aabb, axis);
	Interval2D b = GetInterval(disc_pos, disc_radius, axis);

	return ((b.m_min <= a.m_max) && (a.m_min <= b.m_max));
}

float GetSmallestPenetration(const AABB2& box_1, const AABB2& box_2, const Vec2& axis)
{
	const Interval2D a = GetInterval(box_1, axis);
	const Interval2D b = GetInterval(box_2, axis);

	float box_1_projection = DotProduct(box_1.GetCenter(), axis);
	float box_2_projection = DotProduct(box_2.GetCenter(), axis);

	//Get x axis interval
	Interval2D box_1_proj_onto = GetInterval(box_1, axis);
	Interval2D box_2_proj_onto = GetInterval(box_2, axis);

	float penetration = 0.0f;
	if(box_1_projection < box_2_projection)
	{
		penetration = box_2_proj_onto.m_min - box_1_proj_onto.m_max;
	}
	else
	{
		penetration = box_2_proj_onto.m_max - box_1_proj_onto.m_min;
	}

	return penetration;
}

float GetSmallestPenetration(const AABB2& aabb, const OBB2& obb, const Vec2& axis)
{
	const Interval2D a = GetInterval(aabb, axis);
	const Interval2D b = GetInterval(obb, axis);

	float box_1_projection = DotProduct(aabb.GetCenter(), axis);
	float box_2_projection = DotProduct(obb.GetCenter(), axis);

	//Get x axis interval
	Interval2D box_1_proj_onto = GetInterval(aabb, axis);
	Interval2D box_2_proj_onto = GetInterval(obb, axis);

	float penetration = 0.0f;
	if(box_1_projection < box_2_projection)
	{
		penetration = box_2_proj_onto.m_min - box_1_proj_onto.m_max;
	}
	else
	{
		penetration = box_2_proj_onto.m_max - box_1_proj_onto.m_min;
	}

	return penetration;
}

float GetSmallestPenetration(const OBB2& box_1, const OBB2& box_2, const Vec2& axis)
{
	const Interval2D a = GetInterval(box_1, axis);
	const Interval2D b = GetInterval(box_2, axis);

	float box_1_projection = DotProduct(box_1.GetCenter(), axis);
	float box_2_projection = DotProduct(box_2.GetCenter(), axis);

	//Get x axis interval
	Interval2D box_1_proj_onto = GetInterval(box_1, axis);
	Interval2D box_2_proj_onto = GetInterval(box_2, axis);

	float penetration = 0.0f;
	if(box_1_projection < box_2_projection)
	{
		penetration = box_2_proj_onto.m_min - box_1_proj_onto.m_max;
	}
	else
	{
		penetration = box_2_proj_onto.m_max - box_1_proj_onto.m_min;
	}

	return penetration;
}

bool SATonAABB2versusAABB2(const AABB2& box_1, const AABB2& box_2)
{
	std::vector<Vec2> axisToTest = { Vec2(1.0f, 0.0f), Vec2(0.0f, 1.0f)};

	for(int axis_id = 0; axis_id < static_cast<int>(axisToTest.size()); ++axis_id)
	{
		// if intervals do not overlap, a separating axis is found
		if(!OverlapOnAxis(box_1, box_2, axisToTest[axis_id])) return false; // no collision has taken place
	}

	// All interval overlapped, separating axis not found
	return true; //we have a collision
}

bool SATonAABB2versusOBB2(const AABB2& aabb, const OBB2& obb)
{
	std::vector<Vec2> axisToTest = { Vec2(1.0f, 0.0f), Vec2(0.0f, 1.0f), obb.m_right, obb.m_up};

	for(int axis_id = 0; axis_id < static_cast<int>(axisToTest.size()); ++axis_id)
	{
		// if intervals do not overlap, a separating axis is found
		if(!OverlapOnAxis(aabb, obb, axisToTest[axis_id])) return false; // no collision has taken place
	}

	// All interval overlapped, separating axis not found
	return true; //we have a collision
}

bool SATonOBB2versusOBB2(const OBB2& box_1, const OBB2& box_2)
{
	AABB2 local_aabb = box_1.GetLocalAABB2();

	float theta = Atan2Degrees(box_2.m_right.y, box_2.m_right.x) -  Atan2Degrees(box_1.m_right.y, box_1.m_right.x) ;
	
	OBB2 local_obb(box_1.ToLocalPoint(box_2.GetCenter()), box_2.m_halfExtents, theta);

	return SATonAABB2versusOBB2(local_aabb, local_obb);
}

bool SATonAABB2versusCapsule(const AABB2& aabb, const Capsule2& capsule)
{
	
	//Testing SAT on AABB and inner OBB in capsule
	OBB2 inner_box = capsule.GetOBB();

	Vec2 closest_point_0 = GetClosestPointOnAABB2(capsule.m_p0, aabb);
	Vec2 norm_0 = closest_point_0 - capsule.m_p0;
	norm_0.Normalize();

	Vec2 closest_point_1 = GetClosestPointOnAABB2(capsule.m_p1, aabb);
	Vec2 norm_1 = closest_point_1 - capsule.m_p1;
	norm_1.Normalize();

	std::vector<Vec2> axisToTest = { Vec2(1.0f, 0.0f), Vec2(0.0f, 1.0f), inner_box.m_right, inner_box.m_up, norm_0, norm_1};

	for(int axis_id = 0; axis_id < static_cast<int>(axisToTest.size()); ++axis_id)
	{
		// if intervals do not overlap, a separating axis is found
		//if(!OverlapOnAxis(aabb, inner_box, axisToTest[axis_id])) return false; // no collision has taken place
		if(!OverlapOnAxis(aabb, capsule, axisToTest[axis_id])) return false;
	}

	// All interval overlapped, separating axis not found
	return true; //we have a collision
}

bool SATonOBB2versusCapsule(const OBB2& obb, const Capsule2& capsule)
{
	//Testing SAT on AABB and inner OBB in capsule
	OBB2 inner_box = capsule.GetOBB();

	Vec2 closest_point_0 = obb.GetClosestPoint(capsule.m_p0);
	Vec2 norm_0 = closest_point_0 - capsule.m_p0;
	norm_0.Normalize();

	Vec2 closest_point_1 =  obb.GetClosestPoint(capsule.m_p1);
	Vec2 norm_1 = closest_point_1 - capsule.m_p1;
	norm_1.Normalize();

	std::vector<Vec2> axisToTest = { obb.m_right, obb.m_up, inner_box.m_right, inner_box.m_up, norm_0, norm_1};

	for(int axis_id = 0; axis_id < static_cast<int>(axisToTest.size()); ++axis_id)
	{
		// if intervals do not overlap, a separating axis is found
		//if(!OverlapOnAxis(aabb, inner_box, axisToTest[axis_id])) return false; // no collision has taken place
		if(!OverlapOnAxis(obb, capsule, axisToTest[axis_id])) return false;
	}

	// All interval overlapped, separating axis not found
	return true; //we have a collision
}
