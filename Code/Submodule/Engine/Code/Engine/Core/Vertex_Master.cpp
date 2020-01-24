#include "Engine/Core/Vertex_Master.hpp"

Vertex_Master::~Vertex_Master() = default;

Vertex_Master::Vertex_Master() = default;

Vertex_Master::Vertex_Master(const Vertex_Master& copy) =  default;

Vertex_Master::Vertex_Master(const Vec3& position, const Vec3& normal, const Rgba& color, const Vec2& uv_tex_coords):
	m_position(position), m_normal(normal), m_color(color), m_uv(uv_tex_coords)
{
}
