#pragma once
#include <string>
#include "Engine/Renderer/SpriteSheet.hpp"
#include "Engine/Core/Rgba.hpp"

class CPUMesh;
class TextureView2D;
struct Vertex_PCU;

class BitmapFont
{
	friend class RenderContext; // Only the RenderContext can create new BitmapFont objects!

public:
	BitmapFont(const char* font_name, const TextureView2D* font_texture);
	~BitmapFont();

public:
	enum TextDrawMode
	{
		NO_DRAW_MODE = -1,
		TEXT_DRAW_SHRINK_TO_FIT,
		TEXT_DRAW_OVERRUN,
		NUM_DRAW_MODES
	};

	const TextureView2D* GetTexture() const;

	void AddVertsForText2D(std::vector<Vertex_PCU>& vertex_array, const Vec2& text_pos, float cell_height,
	                       const std::string& text, const Rgba& tint = Rgba::WHITE, float cell_aspect = 1.f,
	                       int max_glyphs = 999999999) const;
	void AddVertsForTextInBox2D(std::vector<Vertex_PCU>& vertex_array, const AABB2& box, float cell_height,
	                            const std::string& text, const Rgba& tint = Rgba::WHITE, float cell_aspect = 1.f,
	                            const Vec2& alignment = AABB2::ALIGN_CENTERED,
	                            TextDrawMode mode = TEXT_DRAW_SHRINK_TO_FIT, int max_glyphs_to_draw = 99999999);
	void AddMeshForText2D(CPUMesh *out, const Vec2& text_pos, float cell_height,
		const std::string& text, const Rgba& tint = Rgba::WHITE, float cell_aspect = 1.f,
		int max_glyphs = 999999999) const;
	void AddMeshForTextInBox2D(CPUMesh *out, const AABB2& box, float cell_height,
		const std::string& text, const Rgba& tint = Rgba::WHITE, float cell_aspect = 1.f,
		const Vec2& alignment = AABB2::ALIGN_CENTERED,
		TextDrawMode mode = TEXT_DRAW_SHRINK_TO_FIT, int max_glyphs_to_draw = 99999999);

protected:
	float GetGlyphAspect(int glyph_unicode) const;

protected:
	std::string m_fontName;
	SpriteSheet m_glyphSpriteSheet;
};
