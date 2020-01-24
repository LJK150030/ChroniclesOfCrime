#pragma once
#include "Engine/EngineCommon.hpp"

//--------------------------------------------------------------------------------------------------
struct Capsule2;
struct Vec2;
struct Vec3;
struct Vertex_PCU;
struct OBB2;

typedef struct Interval2D 
{ 
	Interval2D(): m_min(0.0f), m_max(1.0f) {}
	explicit Interval2D(const float min, const float max): m_min(min), m_max(max) {}
	float m_min; float m_max; 
} Interval2D;

//--------------------------------------------------------------------------------------------------
// Angle utilities
//
float ConvertRadiansToDegrees(float radians);
float ConvertDegreesToRadians(float degrees);
float CosDegrees(float degrees);
float SinDegrees(float degrees);
float TanDegrees(float degrees);
float CosRadians(float radians);
float SinRadians(float radians);
float TanRadians(float radians);
float Atan2Degrees(float y, float x);
float GetAngularDisplacement(float start_degrees, float end_degrees);
float GetTurnedToward(float current_degrees, float goal_degrees, float max_positive_delta_degrees);

//--------------------------------------------------------------------------------------------------
// Geometric utilities
//
float GetDistance(const Vec2& position_a, const Vec2& position_b);
float GetDistance(const Vec3& position_a, const Vec3& position_b);
float GetDistanceXY(const Vec3& position_a, const Vec3& position_b);
float GetDistanceSquared(const Vec2& position_a, const Vec2& position_b);
float GetDistanceSquared(const Vec3& position_a, const Vec3& position_b);
float GetDistanceXYSquared(const Vec3& position_a, const Vec3& position_b);

//--------------------------------------------------------------------------------------------------
// Transform utilities
//
Vec2 TransformPosition(const Vec2& position, float uniform_scale, float rotation_degree_about_z,
                       const Vec2& translation_xy);
Vec3 TransformPosition(const Vec3& position, float uniform_scale_xy, float rotation_degree_about_z,
                       const Vec2& translation_xy);
void TransformLocalVectorToWorld();
void TransformLocalPositionToWorld();
void TransformWorldVectorToLocal();
void TransformWorldPositionToLocal();
Vec2 MaxVec2(const Vec2& a, const Vec2& b);
Vec2 MinVec2(const Vec2& a, const Vec2& b);

//--------------------------------------------------------------------------------------------------
// Conversion
//
constexpr float MATH_PI = 3.1415926535897932384626433832795f;
constexpr float RADIANS_TO_DEGREES = 180.0f / MATH_PI;
constexpr float DEGREES_TO_RADIANS = MATH_PI / 180.0f;
float RangeMapLinearFloat(float in_value, float in_start, float in_end, float out_start, float out_end);
Vec2 RangeMapLinearVec2(const Vec2& in_value, const Vec2& in_start, const Vec2& in_end, const Vec2& out_start, const Vec2& out_end);
Vec2 ConvertPosFromTwoAABB2S(const Vec2& in_space_pos, const AABB2& in_space, const AABB2& out_space);
float LinearInterpolationFloat(float start_value, float end_value, float fraction);
Vec2 LinearInterpolationVec2(const Vec2& start_value, const Vec2& end_value, float fraction);
Vec3 LinearInterpolationVec3(const Vec3& start_value, const Vec3& end_value, float fraction);
Rgba LinearInterpolationRgba(const Rgba& start_value, const Rgba& end_value, float fraction);
float GetFractionInRange(float start_value, float end_value, float value);
float Sign( float value );

//--------------------------------------------------------------------------------------------------
// Bit operations
//
bool IsBitFlagSet(unsigned short bits, unsigned short bit_flag);
void SetBitFlag(unsigned short& bits, unsigned short bit_flag);
unsigned short GetBitFlag(unsigned short& bits, unsigned short bit_flag);
void ClearBitFlag(unsigned short& bits, unsigned short bit_flag);
void ToggleBitFlag(unsigned short& bits, unsigned short bit_flag);

bool IsBitFlagSet(unsigned int bits, unsigned int bit_flag);
void SetBitFlag(unsigned int& bits, unsigned int bit_flag);
unsigned int GetBitFlag(unsigned int& bits, unsigned int bit_flag);
void ClearBitFlag(unsigned int& bits, unsigned int bit_flag);
void ToggleBitFlag(unsigned int& bits, unsigned int bit_flag);

//--------------------------------------------------------------------------------------------------
// number operations
//
float ClampFloat(float value, float min_value, float max_value);
double ClampDouble(double value, double min_value, double max_value);
float ClampFloatPositive(float value, float max_value);
float ClampFloatPositive(const float value);
Vec2 ClampVec2(const Vec2& value, const Vec2& min_value, const Vec2& max_value);
int RoundToNearestInt(float value);
int RoundDownToNearestInt(float value);
int RoundUpToNearestInt(float value);
int ModPositive(int value, int mod_by);
float ModFloatPositive(float value, float mod_by);
float ClampZeroToOne(float value);
int ClampInt(int value, int min_value, int max_value);
int ClampIntPositive(int value, int max_value);
float Sqrt(float value);
float RecipSqrt(float value);
float Abs(float f);
int Abs(int i);
bool IsZero(float a, float epsilon = EPSILON);
bool AreEqual(float a, float b, float epsilon = EPSILON);
bool AreEqual(const Vec2& a, const Vec2& b, float epsilon = EPSILON);
float Min(float a, float b);
float Max(float a, float b);
int Min(int a, int b);
int Max(int a, int b);
unsigned int QuadraticFormula(float* out, float A, float B, float C);

//--------------------------------------------------------------------------------------------------
// Matrix Operations
//
float DotProduct(const Vec2& a, const Vec2& b);
float DotProduct(const Vec3& a, const Vec3& b);
float DotProduct(const Vec4& a, const Vec4& b);
float GetProjectedLengthAlongAxis2D(const Vec2& source, const Vec2& normalized_axis);
Vec2 GetProjectedVectorAlongAxis2D(const Vec2& source, const Vec2& axis);
Vec2 ReflectVectorOffSurfaceNormal(const Vec2& incoming_vector, const Vec2& surface_normal);
float GetRayImpactFractionVsDisc2D(const Vec2& ray_start, const Vec2& ray_normalized_dir, float ray_length,
                                   const Vec2& disc_center, float disc_radius);
Vec3 Cross(const Vec3& a, const Vec3& b);

//--------------------------------------------------------------------------------------------------
// Collision Detection and resolution
//

// line vs point
Vec2 GetClosestPointOnLine2D(const Vec2& reference_pos, const Vec2& point_on_line, const Vec2& another_point_on_line);
Vec3 GetClosestPointOnLine3D(const Vec3& reference_pos, const Vec3& point_on_line, const Vec3& another_point_on_line);
float GetDistanceSquaredFromLine2D(const Vec2& reference_pos, const Vec2& point_on_line, const Vec2& another_point_on_line);
float GetDistanceSquaredFromLine3D(const Vec3& reference_pos, const Vec3& point_on_line, const Vec3& another_point_on_line);

Vec2 GetClosestPointOnLineSegment2D(const Vec2& reference_pos, const Vec2& line_start, const Vec2& line_end);
Vec3 GetClosestPointOnLineSegment3D(const Vec3& reference_pos, const Vec3& line_start, const Vec3& line_end);
float GetDistanceSquaredFromLineSegment2D(const Vec2& reference_pos, const Vec2& line_start, const Vec2& line_end);
float GetDistanceSquaredFromLineSegment3D(const Vec3& reference_pos, const Vec3& line_start, const Vec3& line_end);

void GetClosestPointsFromTwo2DLines(Vec2& point_1, Vec2& point_2, const Vec2& line_1_start, const Vec2& line_1_end, const Vec2& line_2_start, const Vec2& line_2_end);
void GetClosestPointsFromTwo3DLines(Vec3& point_1, Vec3& point_2, const Vec3& line_1_start, const Vec3& line_1_end, const Vec3& line_2_start, const Vec3& line_2_end);
void GetClosestPointsFromTwo2DLineSegments(Vec2& point_1, Vec2& point_2, const Vec2& line_1_start, const Vec2& line_1_end, const Vec2& line_2_start, const Vec2& line_2_end);
void GetClosestPointsFromTwo3DLineSegments(Vec3& point_1, Vec3& point_2, const Vec3& line_1_start, const Vec3& line_1_end, const Vec3& line_2_start, const Vec3& line_2_end);
float GetDistanceSquaredFromTwo2DLineSegments(float& length_sq_from_l1_start, float& length_sq_from_l2_start, const Vec2& line_1_start, const Vec2& line_1_end, const Vec2& line_2_start, const Vec2& line_2_end);
float GetDistanceSquaredFromTwo3DLineSegments(float& length_sq_from_l1_start, float& length_sq_from_l2_start, const Vec3& line_1_start, const Vec3& line_1_end, const Vec3& line_2_start, const Vec3& line_2_end);

// Disc checks
Vec2 GetClosestPointOnDisc(const Vec2& position, const Vec2& disc_center, float disc_radius);
bool IsPointInDisc2D(const Vec2& position, const Vec2& origin, float disc_radius);
bool DoDiscsOverlap(const Vec2& center_a, float radius_a, const Vec2& center_b, float radius_b);
bool DoesDiscOverlapAABB2D(const Vec2& disc_center, float disc_radius, const AABB2& box);
bool DoesDiscOverlapLine2D(const Vec2& disc_center, float disc_radius, const Vec2& point_on_line, const Vec2& another_point_on_line);
bool DoesDiscOverlapLineSegment2D(const Vec2& disc_center, float disc_radius, const Vec2& line_start, const Vec2& line_end);
bool DoesDiscOverlapCapsule2D(const Vec2& disc_center, float disc_radius, const Vec2& capsule_start, const Vec2& capsule_end, float capsule_radius);
bool DoesDiscOverlapOBB2D(const Vec2& disc_center, float disc_radius, const OBB2& box);

void PushDiscOutOfPoint(const Vec2& position, Vec2& disc_center, float disc_radius);
void PushDiscOutOfDisc(const Vec2& stationary_disc_center, float stationary_disc_radius, Vec2& mobile_disc_center, float mobile_disc_radius);
void PushDiscsOutOfEachOther(Vec2& disc_1_center, float disc_1_radius, Vec2& disc_2_center, float disc_2_radius);
void PushDiscOutOfAABB2(const AABB2& stationary_block, Vec2& mobile_disc_center, float mobile_disc_radius);

Vec2 GetClosestPointOnAABB2(const Vec2& reference_pos, const AABB2& box);
void GetClosestPointOnAABB2FromLineSegment(const Vec2& line_start, const Vec2& line_end, const AABB2& box, Vec2& point_start, Vec2& point_end);
bool IsPointInAABB2(const Vec2& reference_pos, const AABB2& box);
bool DoesAABB2OverlapLineSegment2D(const AABB2& box, const Vec2& line_start, const Vec2& line_end);
bool DoAABB2SOverlap(const AABB2& first_box, const AABB2& second_box);

Vec2 GetClosestPointOnCapsule2D(const Vec2& reference_pos, const Vec2& capsule_start, const Vec2& capsule_end, float capsule_radius);
bool IsPointInCapsule2D(const Vec2& point, const Vec2& capsule_start, const Vec2& capsule_end, float capsule_radius);

bool DoSpheresOverlap(const Vec3& center_a, float radius_a, const Vec3& center_b, float radius_b);

bool IsPointInSector(const Vec2& point, const Vec2& origin, float orientation_degrees, float max_dist,
                     float aperture_degrees);

Vec2 GetClosestPointOnOBB2(const Vec2& reference_pos, const OBB2& box);
void GetClosestPointOnOBB2FromLineSegment(const Vec2& line_start, const Vec2& line_end, const OBB2& box, Vec2& cap_pos, Vec2& box_pos);

//--------------------------------------------------------------------------------------------------
// Local and World conversions
//
Vec2 GetLocalVectorFromWorld(const Vec2& world_vector, const Vec2& i_basis, const Vec2& j_basis);
Vec2 GetLocalPositionFromWorld(const Vec2& world_position, const Vec2& i_basis, const Vec2& j_basis,
                               const Vec2& t_basis);
Vec2 GetWorldVectorFromLocal(const Vec2& local_vector, const Vec2& i_basis, const Vec2& j_basis);
Vec2 GetWorldPositionFromLocal(const Vec2& local_position, const Vec2& i_basis, const Vec2& j_basis,
                               const Vec2& t_basis);

//--------------------------------------------------------------------------------------------------
// Easing functions 
//

float SmoothStart2(float input_zero_to_one);
float SmoothStart3(float input_zero_to_one);
float SmoothStart4(float input_zero_to_one);
float SmoothStart5(float input_zero_to_one);

float SmoothStop2(float input_zero_to_one);
float SmoothStop3(float input_zero_to_one);
float SmoothStop4(float input_zero_to_one);
float SmoothStop5(float input_zero_to_one);

float SmoothStep3(float input_zero_to_one);
float SmoothStep5(float input_zero_to_one);

float Mix(float first_result, float second_result, float weight);

// 1D Interval math
Interval2D GetInterval( const Vec2& disc_pos, float disc_radius, const Vec2& axis );
Interval2D GetInterval( const AABB2& box, const Vec2& axis);
Interval2D GetInterval( const OBB2& box, const Vec2& axis );
Interval2D GetInterval( const Capsule2& capsule, const Vec2& axis );

bool OverlapOnAxis(const AABB2& aabb, const Vec2& disc_pos, float disc_radius);
bool OverlapOnAxis(const AABB2& box_1, const AABB2& box_2, const Vec2& axis);
bool OverlapOnAxis(const AABB2& aabb, const OBB2& obb, const Vec2& axis);
bool OverlapOnAxis(const AABB2& aabb, const Capsule2& capsule2, const Vec2& axis);
bool OverlapOnAxis(const OBB2& obb, const Capsule2& capsule, const Vec2& axis);

float GetSmallestPenetration(const AABB2& box_1, const AABB2& box_2, const Vec2& axis);
float GetSmallestPenetration(const AABB2& aabb, const OBB2& obb, const Vec2& axis);
float GetSmallestPenetration(const OBB2& box_1, const OBB2& box_2, const Vec2& axis);

// SAT Tests
bool SATonAABB2versusAABB2(const AABB2& box_1, const AABB2& box_2);
bool SATonAABB2versusOBB2(const AABB2& aabb, const OBB2& obb);
bool SATonOBB2versusOBB2(const OBB2& box_1, const OBB2& box_2);
bool SATonAABB2versusCapsule( const AABB2& aabb, const Capsule2& capsule);
bool SATonOBB2versusCapsule( const OBB2& obb, const Capsule2& capsule );