#pragma once
#include "Engine/EngineCommon.hpp"
#include "Engine/Math/Vec3.hpp"

struct OBB3;
struct Plane3;
struct Capsule3;
struct AABB3;
struct Sphere;

struct Ray3
{
	Ray3(); 
	Ray3( const Vec3& pos, const Vec3& dir ); 

	Vec3 position     = Vec3::ZERO; 
	Vec3 direction    = Vec3::FORWARD; // assumed normalized - by default, any unit vector is fine.

	// useful construction
	static Ray3 FromPoints( const Vec3& start, const Vec3& end );
	Vec3 PointAtTime( float t ) const;
}; 

// Functions
// All return ALL intersections (including negative).
// all out values must be a float array of size 2
//https://www.cl.cam.ac.uk/teaching/1999/AGraphHCI/SMAG/node2.html#eqn:rectcyl
unsigned int Raycast( float *out, const Ray3& ray, const Sphere& sphere );         // sphere
unsigned int Raycast( float *out, const Ray3& ray, const AABB3& aabb3 );          // aabb3
unsigned int Raycast( float *out, const Ray3& ray, const OBB3& obb3 );          // aabb3
unsigned int Raycast( float *out, const Ray3& ray, const Plane3& plane );          // plane
unsigned int Raycast( float *out, const Ray3& ray, const Capsule3& capsule );      // capsule
