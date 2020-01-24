#include "Engine/Core/Vertex_Lit.hpp"
#include "Engine/Core/Vertex_Master.hpp"
#include "Engine/EngineCommon.hpp"

STATIC BufferAttributeT const Vertex_Lit::LAYOUT[] = {
	BufferAttributeT( "POSITION",  RDF_VEC3,      offsetof(Vertex_Lit, m_position) ), 
	BufferAttributeT( "COLOR",     RDF_RGBA32,    offsetof(Vertex_Lit, m_color) ), 
	BufferAttributeT( "UV",        RDF_VEC2,      offsetof(Vertex_Lit, m_uvTexCoords) ), 
	BufferAttributeT( "NORMAL",    RDF_VEC3,      offsetof(Vertex_Lit, m_normal) ), 
	BufferAttributeT( "TANGENT",   RDF_VEC4,      offsetof(Vertex_Lit, m_tangent) ), 
	BufferAttributeT() // end
};

Vertex_Lit::~Vertex_Lit() = default;

Vertex_Lit::Vertex_Lit() = default;

Vertex_Lit::Vertex_Lit(const Vertex_Lit& copy) = default;

Vertex_Lit::Vertex_Lit(const Vec3& position, const Rgba& color, const Vec2& uv_tex_coords, const Vec3& normal):
	m_position(position), m_normal(normal), m_color(color), m_uvTexCoords(uv_tex_coords)
{
}

Vertex_Lit::Vertex_Lit(const Vec3& position, const Rgba& color, const Vec2& uv_tex_coords, const Vec3& normal,
	const Vec4& tangent) : m_position(position), m_normal(normal), m_color(color), m_uvTexCoords(uv_tex_coords), m_tangent(tangent)
{
}

void Vertex_Lit::CopyFromMaster( void* buffer, const std::vector<Vertex_Master>& mesh_verts)
{
	 Vertex_Lit* out = reinterpret_cast<Vertex_Lit*>(buffer);

	for(int vert_id = 0; vert_id < static_cast<int>(mesh_verts.size()); ++vert_id)
	{
		out[vert_id].m_position = mesh_verts[vert_id].m_position;
		out[vert_id].m_normal = mesh_verts[vert_id].m_normal;
		out[vert_id].m_color = mesh_verts[vert_id].m_color;
		out[vert_id].m_uvTexCoords = mesh_verts[vert_id].m_uv;
		out[vert_id].m_tangent = mesh_verts[vert_id].m_tangent;
	}
}
