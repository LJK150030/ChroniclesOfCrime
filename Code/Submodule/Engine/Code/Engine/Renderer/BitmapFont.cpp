#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/TextureView2D.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/EngineCommon.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/CPUMesh.hpp"
#include "RenderContext.hpp"

BitmapFont::BitmapFont(const char* font_name, const TextureView2D* font_texture): m_fontName(font_name),
                                                                            m_glyphSpriteSheet(
	                                                                            SpriteSheet(
		                                                                            font_texture, IntVec2(16, 16)))
{
}

BitmapFont::~BitmapFont()
{
	//DX_SAFE_RELEASE(m_glyphSpriteSheet.m_texture->m_handle);
	delete m_glyphSpriteSheet.m_texture;
}

const TextureView2D* BitmapFont::GetTexture() const
{
	return m_glyphSpriteSheet.GetTexture();
}

void BitmapFont::AddVertsForText2D(std::vector<Vertex_PCU>& vertex_array, const Vec2& text_pos, const float cell_height,
                                   const std::string& text, const Rgba& tint, float cell_aspect, int max_glyphs) const
{
	const float cell_width = cell_height * cell_aspect;
	Rgba local_tint = tint;

	for (int char_index = 0; char_index < static_cast<int>(text.length()); ++char_index)
	{
		const AABB2 box = AABB2(text_pos.x + cell_width * char_index, text_pos.y,
		                        text_pos.x + cell_width * (char_index + 1), text_pos.y + cell_height);
		const unsigned char letter = text[char_index];
		SpriteDefinition char_from_text = m_glyphSpriteSheet.GetSpriteDef(static_cast<int>(letter));
		Vec2 char_min_uv = Vec2::ZERO;
		Vec2 char_max_uv = Vec2::ONE;
		char_from_text.GetUVs(char_min_uv, char_max_uv);

		if (char_index >= max_glyphs)
			local_tint = Rgba(tint.r, tint.g, tint.b, 0.0f);

		vertex_array.emplace_back(Vec3(box.mins.x, box.mins.y, 0.f), local_tint, Vec2(char_min_uv.x, char_min_uv.y));
		vertex_array.emplace_back(Vec3(box.maxs.x, box.maxs.y, 0.f), local_tint, Vec2(char_max_uv.x, char_max_uv.y));
		vertex_array.emplace_back(Vec3(box.mins.x, box.maxs.y, 0.f), local_tint, Vec2(char_min_uv.x, char_max_uv.y));

		vertex_array.emplace_back(Vec3(box.mins.x, box.mins.y, 0.f), local_tint, Vec2(char_min_uv.x, char_min_uv.y));
		vertex_array.emplace_back(Vec3(box.maxs.x, box.mins.y, 0.f), local_tint, Vec2(char_max_uv.x, char_min_uv.y));
		vertex_array.emplace_back(Vec3(box.maxs.x, box.maxs.y, 0.f), local_tint, Vec2(char_max_uv.x, char_max_uv.y));
	}
}

void BitmapFont::AddVertsForTextInBox2D(std::vector<Vertex_PCU>& vertex_array, const AABB2& box, float cell_height,
                                        const std::string& text, const Rgba& tint, float cell_aspect,
                                        const Vec2& alignment, TextDrawMode mode, int max_glyphs_to_draw)
{
	std::vector<std::string> lines = SplitStringOnDelimiter(text, '\n');
	float recalculated_cell_height = cell_height;
	float recalculated_cell_width = cell_height * cell_aspect;
	int maxs_chars_left = max_glyphs_to_draw;

	switch (mode)
	{
	case TEXT_DRAW_SHRINK_TO_FIT:
		{
			//checking for the longest line
			int longest_line_index = 0;

			if (lines.size() > 1)
			{
				for (int lineID = 1; lineID < static_cast<int>(lines.size()); lineID++)
				{
					if (lines[longest_line_index].size() < lines[lineID].size())
					{
						longest_line_index = lineID;
					}
				}
			}

			const float longest_line_length = static_cast<float>(lines[longest_line_index].size());

			const float paragraph_height = static_cast<float>(lines.size()) * cell_height;
			const float paragraph_length = longest_line_length * cell_aspect;

			const float box_length = (box.maxs.x - box.mins.x);
			const float box_height = (box.maxs.y - box.mins.y);


			if (paragraph_height >= box_height)
			{
				recalculated_cell_height = box_height / paragraph_height;
				recalculated_cell_width = recalculated_cell_height * cell_aspect;
			}

			if (paragraph_length >= box_length)
			{
				recalculated_cell_width = box_length / paragraph_length;
				recalculated_cell_height = recalculated_cell_width / cell_aspect;
			}

			break;
		}
	case TEXT_DRAW_OVERRUN:
		{
			recalculated_cell_height = cell_height;
			recalculated_cell_width = cell_height * cell_aspect;
			break;
		}
	default:
		{
			ERROR_AND_DIE("Unknown Text Draw Mode");
		}
	}

	const float line_stride = 1.0f / static_cast<float>(lines.size());

	for (int lineID = 0; lineID < static_cast<int>(lines.size()); lineID++)
	{
		const float line_min_x = 0.0f;
		const float line_min_y = 0.0f;
		const float line_max_x = static_cast<float>(lines[lineID].size()) * recalculated_cell_width;
		const float line_max_y = static_cast<float>(lines.size()) * recalculated_cell_height;
		AABB2 line_bounds(line_min_x, line_min_y, line_max_x, line_max_y);

		Vec2 line_height_pos = line_bounds.GetPointFromUV(
			Vec2(0.0f, line_stride * (static_cast<float>(lines.size()) - (lineID + 1))));

		line_bounds.AlignWithinBox(box, alignment);
		AddVertsForText2D(vertex_array, box.mins + line_bounds.mins + line_height_pos, recalculated_cell_height,
		                  lines[lineID], tint, cell_aspect, maxs_chars_left);
		maxs_chars_left -= static_cast<int>(lines[lineID].size()) - 1;
	}
}

void BitmapFont::AddMeshForText2D(CPUMesh* out, const Vec2& text_pos, float cell_height, const std::string& text,
	const Rgba& tint, float cell_aspect, int max_glyphs) const
{
	const float cell_width = cell_height * cell_aspect;
	Rgba local_tint = tint;
	int char_offset = out->GetVertexCount();

	for (int char_index = 0; char_index < static_cast<int>(text.size()); ++char_index)
	{
		const AABB2 box = AABB2(text_pos.x + cell_width * char_index, text_pos.y,
			text_pos.x + cell_width * (char_index + 1), text_pos.y + cell_height);
		const unsigned char letter = text[char_index];
		SpriteDefinition char_from_text = m_glyphSpriteSheet.GetSpriteDef(static_cast<int>(letter));
		Vec2 char_min_uv = Vec2::ZERO;
		Vec2 char_max_uv = Vec2::ONE;
		char_from_text.GetUVs(char_min_uv, char_max_uv);

		if (char_index >= max_glyphs)
			local_tint = Rgba(tint.r, tint.g, tint.b, 0.0f);

		out->AddVertex(Vec3(box.mins.x, box.maxs.y, 0.f), local_tint, Vec2(char_min_uv.x, char_max_uv.y));
		out->AddVertex(Vec3(box.maxs.x, box.maxs.y, 0.f), local_tint, Vec2(char_max_uv.x, char_max_uv.y));
		out->AddVertex(Vec3(box.mins.x, box.mins.y, 0.f), local_tint, Vec2(char_min_uv.x, char_min_uv.y));
		out->AddVertex(Vec3(box.maxs.x, box.mins.y, 0.f), local_tint, Vec2(char_max_uv.x, char_min_uv.y));
		out->AddIndexedQuad(char_index*4+0+char_offset, char_index*4+1+char_offset, char_index*4+2+char_offset, char_index*4+3+char_offset);
	}
}

void BitmapFont::AddMeshForTextInBox2D(CPUMesh* out, const AABB2& box, float cell_height, const std::string& text,
	const Rgba& tint, float cell_aspect, const Vec2& alignment, TextDrawMode mode, int max_glyphs_to_draw)
{
	std::vector<std::string> lines = SplitStringOnDelimiter(text, '\n');
	float recalculated_cell_height = cell_height;
	float recalculated_cell_width = cell_height * cell_aspect;
	int maxs_chars_left = max_glyphs_to_draw;

	switch (mode)
	{
	case TEXT_DRAW_SHRINK_TO_FIT:
	{
		//checking for the longest line
		int longest_line_index = 0;

		if (lines.size() > 1)
		{
			for (int lineID = 1; lineID < static_cast<int>(lines.size()); lineID++)
			{
				if (lines[longest_line_index].size() < lines[lineID].size())
				{
					longest_line_index = lineID;
				}
			}
		}

		const float longest_line_length = static_cast<float>(lines[longest_line_index].size());
		const float paragraph_length = longest_line_length * recalculated_cell_width;

		const float box_length = (box.maxs.x - box.mins.x);
		const float box_height = (box.maxs.y - box.mins.y);


		if (paragraph_length >= box_length)
		{
			//recalculated_cell_height = box_length / paragraph_length * cell_height;
			recalculated_cell_width = box_length / paragraph_length * cell_height;
			recalculated_cell_height = recalculated_cell_width / cell_aspect;
		}

		const float paragraph_height = static_cast<float>(lines.size()) * recalculated_cell_height;

		if (paragraph_height >= box_height)
		{
			//recalculated_cell_height = box_height / paragraph_height * cell_height;
			recalculated_cell_height = box_height / paragraph_height * cell_height;
			recalculated_cell_width = recalculated_cell_height * cell_aspect;
		}

		break;
	}
	case TEXT_DRAW_OVERRUN:
	{
		//recalculated_cell_height = cell_height;
		//recalculated_cell_width = cell_height * cell_aspect;
		break;
	}
	default:
	{
		ERROR_AND_DIE("Unknown Text Draw Mode");
	}
	}

	//creating verts and indicies, requires duplicating code because we need to know how many character
	//are in the entire paragraph
	const float line_stride = 1.0f / static_cast<float>(lines.size());
	out->Clear();
	int char_created = 0;

	for (int lineID = 0; lineID < static_cast<int>(lines.size()); lineID++)
	{
		const float line_min_x = 0.0f;
		const float line_min_y = 0.0f;
		const float line_max_x = static_cast<float>(lines[lineID].size()) * recalculated_cell_width;
		const float line_max_y = static_cast<float>(lines.size()) * recalculated_cell_height;
		AABB2 line_bounds(line_min_x, line_min_y, line_max_x, line_max_y);

		Vec2 line_height_pos = line_bounds.GetPointFromUV(Vec2(
			0.0f, 
			line_stride * (static_cast<float>(lines.size()) - (lineID + 1))));

		line_bounds.AlignWithinBox(box, alignment);

		//const Vec2 text_pos = box.mins + line_bounds.mins + line_height_pos;
		const Vec2 text_pos =  line_bounds.mins + line_height_pos;

		const float cell_width = recalculated_cell_height * cell_aspect;
		Rgba local_tint = tint;
		
		for (int char_index = 0; char_index < static_cast<int>(lines[lineID].size()); ++char_index)
		{
			const AABB2 text_box = AABB2(text_pos.x + cell_width * char_index, text_pos.y,
				text_pos.x + cell_width * (char_index + 1), text_pos.y + recalculated_cell_height);
			const unsigned char letter = lines[lineID][char_index];
			SpriteDefinition char_from_text = m_glyphSpriteSheet.GetSpriteDef(static_cast<int>(letter));
			Vec2 char_min_uv = Vec2::ZERO;
			Vec2 char_max_uv = Vec2::ONE;
			char_from_text.GetUVs(char_min_uv, char_max_uv);

			if (char_index >= maxs_chars_left)
				local_tint = Rgba(tint.r, tint.g, tint.b, 0.0f);

			out->AddVertex(Vec3(text_box.mins.x, text_box.maxs.y, 0.f), local_tint, Vec2(char_min_uv.x, char_max_uv.y));
			out->AddVertex(Vec3(text_box.maxs.x, text_box.maxs.y, 0.f), local_tint, Vec2(char_max_uv.x, char_max_uv.y));
			out->AddVertex(Vec3(text_box.mins.x, text_box.mins.y, 0.f), local_tint, Vec2(char_min_uv.x, char_min_uv.y));
			out->AddVertex(Vec3(text_box.maxs.x, text_box.mins.y, 0.f), local_tint, Vec2(char_max_uv.x, char_min_uv.y));
			out->AddIndexedQuad(char_created*4+0, char_created*4+1, char_created*4+2, char_created*4+3);
			char_created++;
		}


		maxs_chars_left -= static_cast<int>(lines[lineID].size()) - 1;
	}
}

float BitmapFont::GetGlyphAspect(int glyph_unicode) const
{
	UNUSED(glyph_unicode); //TODO: need to change GetGlyphAspect
	return 1.0f;
}
