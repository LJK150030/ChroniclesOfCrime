#include "Engine/Renderer/SpriteDefinition.hpp"
#include "Engine/Core/VertexUtils.hpp"

SpriteDefinition::SpriteDefinition(const Vec2& uv_at_bottom_left, const Vec2& uv_at_top_right):
	m_uvAtBottomLeft(Vec2(uv_at_bottom_left)), m_uvAtTopRight(Vec2(uv_at_top_right))
{
}

void SpriteDefinition::GetUVs(Vec2& out_uv_at_bottom_left, Vec2& out_uv_at_top_right) const
{
	out_uv_at_bottom_left = m_uvAtBottomLeft;
	out_uv_at_top_right =  m_uvAtTopRight;
}
