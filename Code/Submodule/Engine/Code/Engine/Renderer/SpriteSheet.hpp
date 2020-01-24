#pragma once
#include "Engine/EngineCommon.hpp"
#include "Engine/Renderer/SpriteDefinition.hpp"
#include "Engine/Math/AABB2.hpp"
#include <vector>

class TextureView2D;

class SpriteSheet
{
	friend class BitmapFont;

public:
	SpriteSheet();
	explicit SpriteSheet(const TextureView2D* texture, const IntVec2& sprite_grid_layout,
	                     const Vec2& pivot = AABB2::ALIGN_TOP_RIGHT, int pixel_margin = 0);

	~SpriteSheet();

	//Getters
	const TextureView2D* GetTexture() const { return m_texture; }
	const SpriteDefinition& GetSpriteDef(const int sprite_index) const { return m_spriteDefs[sprite_index]; }
	const SpriteDefinition& GetSpriteDef(const IntVec2& sprite_coord) const;

	//Helpers
	void GetUVCoordsFromGridCoords(const IntVec2& in_coords, Vec2& out_min, Vec2& out_max);

protected:
	const TextureView2D* m_texture = nullptr;
	std::vector<SpriteDefinition> m_spriteDefs;
	IntVec2 m_spriteGridLayout;
};
