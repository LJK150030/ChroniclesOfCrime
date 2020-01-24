#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Renderer/TextureView2D.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Shader.hpp"

SpriteSheet::SpriteSheet()
= default;

SpriteSheet::SpriteSheet(const TextureView2D* texture, const IntVec2& sprite_grid_layout, const Vec2& pivot,
                         const int pixel_margin)
{
	m_texture = texture;
	m_spriteGridLayout = sprite_grid_layout;

	const float u_stride = 1.f / static_cast<float>(m_spriteGridLayout.x);
	const float v_stride = 1.f / static_cast<float>(m_spriteGridLayout.y);

	const float u_margin = static_cast<float>(pixel_margin) / static_cast<const float>(m_texture->m_dimensions.x);
	const float v_margin = static_cast<float>(pixel_margin) / static_cast<const float>(m_texture->m_dimensions.y);

	const float resized_u = u_stride - u_margin;
	const float resized_v = v_stride - v_margin;
	const Vec2 resized_bounds(resized_u, resized_v);

	const int num_sprites = sprite_grid_layout.x * sprite_grid_layout.y;
	const int& sprites_per_row = sprite_grid_layout.x;

	for (int sprite_index = 0; sprite_index < num_sprites; ++sprite_index)
	{
		const int sprite_grid_x = sprite_index % sprites_per_row;
		const int sprite_grid_y = sprite_index / sprites_per_row;
		const IntVec2 sprite_grid(sprite_grid_x, sprite_grid_y);

		Vec2 min_uv = Vec2::ZERO;
		Vec2 max_uv = Vec2::ONE;

		GetUVCoordsFromGridCoords(sprite_grid, min_uv, max_uv);

		AABB2 sprite_bounds(min_uv, max_uv);
		sprite_bounds.Resize(resized_bounds, pivot);

		m_spriteDefs.emplace_back(sprite_bounds.mins, sprite_bounds.maxs);
	}
}

SpriteSheet::~SpriteSheet()
{
}

const SpriteDefinition& SpriteSheet::GetSpriteDef(const IntVec2& sprite_coord) const
{
	return m_spriteDefs[sprite_coord.y * m_spriteGridLayout.x + sprite_coord.x];
}

void SpriteSheet::GetUVCoordsFromGridCoords(const IntVec2& in_coords, Vec2& out_min, Vec2& out_max)
{
	const float u_stride = 1.f / static_cast<float>(m_spriteGridLayout.x);
	const float v_stride = 1.f / static_cast<float>(m_spriteGridLayout.y);

	float u_at_min_x;
	float v_at_min_y;
	float u_at_max_x;
	float v_at_max_y;

// 	if(FLIP_TEXTURE_VERTICALLY)
// 	{
// 		u_at_min_x = u_stride * static_cast<float>(in_coords.x);
// 		v_at_max_y = 1.0f - (v_stride * static_cast<float>(in_coords.y));
// 		u_at_max_x = u_at_min_x + u_stride;
// 		v_at_min_y = v_at_max_y - v_stride;
// 	}
// 	else
// 	{
// 		u_at_min_x = u_stride * static_cast<float>(in_coords.x);
// 		v_at_min_y = v_stride * static_cast<float>(in_coords.y);
// 		u_at_max_x = u_at_min_x + u_stride; 
// 		v_at_max_y = v_at_min_y - v_stride;
// 	}

	v_at_max_y = v_stride * static_cast<float>(in_coords.y);
	v_at_min_y = v_stride * (static_cast<float>(in_coords.y) + 1.0f);

	u_at_min_x = u_stride * static_cast<float>(in_coords.x);
	u_at_max_x = u_stride * (static_cast<float>(in_coords.x) + 1.0f);


	out_min = Vec2(u_at_min_x, v_at_min_y);
 	out_max = Vec2(u_at_max_x, v_at_max_y);
}
