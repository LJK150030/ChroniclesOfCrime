#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Core/Vertex_Master.hpp"
#include "Engine/EngineCommon.hpp"

STATIC BufferAttributeT const Vertex_PCU::LAYOUT[] = {
	BufferAttributeT( "POSITION",  RDF_VEC3,      offsetof(Vertex_PCU, m_position) ), 
	BufferAttributeT( "COLOR",     RDF_RGBA32,    offsetof(Vertex_PCU, m_color) ), 
	BufferAttributeT( "UV",        RDF_VEC2,      offsetof(Vertex_PCU, m_uvTexCoords) ), 
	BufferAttributeT() // end
};

Vertex_PCU::~Vertex_PCU() = default;

Vertex_PCU::Vertex_PCU() = default;

Vertex_PCU::Vertex_PCU(const Vertex_PCU& copy) = default;

//-----------------------------------------------------------------------------------------------
Vertex_PCU::Vertex_PCU(const Vec3& position, const Rgba& color, const Vec2& uv_tex_coords)
	: m_position(position)
	  , m_color(color)
	  , m_uvTexCoords(uv_tex_coords)
{
}

void Vertex_PCU::CopyFromMaster(void* buffer, const std::vector<Vertex_Master>& mesh_verts)
{
	Vertex_PCU* out = reinterpret_cast<Vertex_PCU*>(buffer);

	for(int vert_id = 0; vert_id < static_cast<int>(mesh_verts.size()); ++vert_id)
	{
		out[vert_id].m_position = mesh_verts[vert_id].m_position;
		out[vert_id].m_color = mesh_verts[vert_id].m_color;
		out[vert_id].m_uvTexCoords = mesh_verts[vert_id].m_uv;
	}
}
