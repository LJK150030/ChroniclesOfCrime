#pragma once
#include "Engine/EngineCommon.hpp"

class SpriteDefinition
{
public:
	explicit SpriteDefinition(const Vec2& uv_at_bottom_left, const Vec2& uv_at_top_right);
	void GetUVs(Vec2& out_uv_at_bottom_left, Vec2& out_uv_at_top_right) const;

private:
	Vec2 m_uvAtBottomLeft = Vec2::ZERO;
	Vec2 m_uvAtTopRight = Vec2::ONE;
};
