#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Math/Vec4.hpp"
#include "Engine/Core/BufferAttributeT.hpp"
#include <vector>

struct Vertex_Master;

struct Vertex_Lit
{
public:
	Vec3 m_position;
	Rgba m_color;
	Vec2 m_uvTexCoords;
	Vec3 m_normal;
	Vec4 m_tangent;
	static BufferAttributeT const LAYOUT[]; 

	//Represents a single point (vertex) of a simple 3D object intended to be rendered (drawn)
public:
	~Vertex_Lit(); // destructor: do nothing (for speed)
	Vertex_Lit(); // default constructor: do nothing (for speed)
	Vertex_Lit(const Vertex_Lit& copy); // copy constructor (from another Vec3)
	explicit Vertex_Lit(const Vec3& position, const Rgba& color, const Vec2& uv_tex_coords, const Vec3& normal);
	explicit Vertex_Lit(const Vec3& position, const Rgba& color, const Vec2& uv_tex_coords, const Vec3& normal, const Vec4& tangent);
	static void CopyFromMaster( void* buffer, const std::vector<Vertex_Master>& mesh_verts); 

};
