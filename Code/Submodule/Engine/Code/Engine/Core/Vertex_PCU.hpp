#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Core/BufferAttributeT.hpp"
#include <vector>

struct Vertex_Master;

struct Vertex_PCU
{
public:
	Vec3 m_position;
	Rgba m_color;
	Vec2 m_uvTexCoords;
	static BufferAttributeT const LAYOUT[]; 

//Represents a single point (vertex) of a simple 3D object intended to be rendered (drawn)
public:
	~Vertex_PCU(); // destructor: do nothing (for speed)
	Vertex_PCU(); // default constructor: do nothing (for speed)
	Vertex_PCU(const Vertex_PCU& copy); // copy constructor (from another Vec3)
	explicit Vertex_PCU(const Vec3& position, const Rgba& color, const Vec2& uv_tex_coords);
	static void CopyFromMaster( void* buffer, const std::vector<Vertex_Master>& mesh_verts); 

};
