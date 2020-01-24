#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec4.hpp" 
#include "Engine/Core/Rgba.hpp"

//------------------------------------------------------------------------
// Vertex_Master is a super-set of all vertex parameters we
// will want our CPUMesh to know about; 
// (Historically I would call CPUMesh a MeshBuilder, as it is used 
// for construction of a mesh before it is sent to the GPU.  Like an Image for a Texture
struct Vertex_Master
{
public:
	Vec3 m_position;          // A04
	Vec3 m_normal;         // A05 TODO: update code to now support normals
	Rgba m_color;             // A04
	Vec2 m_uv;                // A04;
	Vec4 m_tangent;        // A06
	// skin weights         // Summer
	// skin indices         // Summer

public:
	~Vertex_Master(); // destructor: do nothing (for speed)
	Vertex_Master(); // default constructor: do nothing (for speed)
	Vertex_Master(const Vertex_Master& copy); // copy constructor (from another Vec3)
	explicit Vertex_Master(const Vec3& position, const Vec3& normal, const Rgba& color, const Vec2& uv_tex_coords);
}; 