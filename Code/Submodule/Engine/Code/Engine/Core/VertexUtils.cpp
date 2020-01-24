#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Capsule2.hpp"
#include "Engine/Math/Segment2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/OBB2.hpp"

void AddVertsForDisc2D(std::vector<Vertex_PCU>& vertex_array, const Vec2& center, const float radius,
                       const Rgba& color, const int num_sides)
{
	const float delta = (2.0f * MATH_PI) / static_cast<float>(num_sides);
	const Vec2 zero_vec2 = Vec2(0.0f, 0.0f);
	for (int i = 0; i < num_sides; i++)
	{
		vertex_array.emplace_back(Vec3(center.x, center.y, 0.0f), color, zero_vec2);

		vertex_array.emplace_back(
			Vec3(center.x + radius * CosDegrees(ConvertRadiansToDegrees(static_cast<float>(i + 1) * delta)),
			     center.y + radius * SinDegrees(ConvertRadiansToDegrees(static_cast<float>(i + 1) * delta)),
			     0.0f), color, zero_vec2);

		vertex_array.emplace_back(
			Vec3(center.x + radius * CosDegrees(ConvertRadiansToDegrees(static_cast<float>(i + 2) * delta)),
			     center.y + radius * SinDegrees(ConvertRadiansToDegrees(static_cast<float>(i + 2) * delta)),
			     0.0f), color, zero_vec2);

	}
}

void AddVertsForHalfDisc2D(std::vector<Vertex_PCU>& vertex_array, const Vec2& center, float radius, const Rgba& color,
	float offset_rotation_degrees, int num_sides)
{
	const float delta = (MATH_PI) / static_cast<float>(num_sides);
	const Vec2 zero_vec2 = Vec2(0.0f, 0.0f);
	for (int i = 0; i < num_sides; i++)
	{
		vertex_array.emplace_back(Vec3(center.x, center.y, 0.0f), color, zero_vec2);

		vertex_array.emplace_back(
			Vec3(center.x + radius * CosDegrees(ConvertRadiansToDegrees(static_cast<float>(i) * delta) + offset_rotation_degrees),
				center.y + radius * SinDegrees(ConvertRadiansToDegrees(static_cast<float>(i) * delta) + offset_rotation_degrees),
				0.0f), color, zero_vec2);

		vertex_array.emplace_back(
			Vec3(center.x + radius * CosDegrees(ConvertRadiansToDegrees(static_cast<float>(i + 1) * delta) + offset_rotation_degrees),
				center.y + radius * SinDegrees(ConvertRadiansToDegrees(static_cast<float>(i + 1) * delta) + offset_rotation_degrees),
				0.0f), color, zero_vec2);

	}
}

void AddVertsForLine2D(std::vector<Vertex_PCU>& vertex_array, const Vec2& start, const Vec2& end,
                       const float thickness, const Rgba& color)
{
	const float ht = thickness * 0.5f;
	Vec2 se = end - start;
	Vec2 hf = se.GetClamped(ht);
	Vec2 hb = -1.0f * hf;
	const Vec2 sehf = end + hf;
	const Vec2 sehb = start + hb;
	const Vec2 fl = sehf + hf.GetRotated90Degrees();
	const Vec2 fr = sehf + hf.GetRotatedMinus90Degrees();
	const Vec2 bl = sehb + hb.GetRotatedMinus90Degrees();
	const Vec2 br = sehb + hb.GetRotated90Degrees();

	vertex_array.emplace_back(Vec3(br.x, br.y, 0.f), Rgba(color), Vec2(0.0f, 0.0f));
	vertex_array.emplace_back(Vec3(fl.x, fl.y, 0.f), Rgba(color), Vec2(0.0f, 0.0f));
	vertex_array.emplace_back(Vec3(bl.x, bl.y, 0.f), Rgba(color), Vec2(0.0f, 0.0f));

	vertex_array.emplace_back(Vec3(br.x, br.y, 0.f), Rgba(color), Vec2(0.0f, 0.0f));
	vertex_array.emplace_back(Vec3(fr.x, fr.y, 0.f), Rgba(color), Vec2(0.0f, 0.0f));
	vertex_array.emplace_back(Vec3(fl.x, fl.y, 0.f), Rgba(color), Vec2(0.0f, 0.0f));
}

void AddVertsForRing2D(std::vector<Vertex_PCU>& vertex_array, const Vec2& center, const float radius,
                       const float thickness, const Rgba& color, const int num_sides)
{
	const float delta = (2.0f * MATH_PI) / static_cast<float>(num_sides);
	const Vec2 zero_vec2 = Vec2(0.0f, 0.0f);
	const float half_thickness = thickness * 0.5f;
	const float inner_radius = radius - half_thickness;
	const float outer_radius = radius + half_thickness;

	for (int i = 0; i < num_sides; i++)
	{
		// find the center
		// first segment
		Vec3 front_right = Vec3(
			center.x + inner_radius * CosDegrees(ConvertRadiansToDegrees(static_cast<float>(i + 1) * delta)),
			center.y + inner_radius * SinDegrees(ConvertRadiansToDegrees(static_cast<float>(i + 1) * delta)),
			0.0f);

		Vec3 front_left = Vec3(
			center.x + outer_radius * CosDegrees(ConvertRadiansToDegrees(static_cast<float>(i + 1) * delta)),
			center.y + outer_radius * SinDegrees(ConvertRadiansToDegrees(static_cast<float>(i + 1) * delta)),
			0.0f);

		Vec3 back_right = Vec3(
			center.x + inner_radius * CosDegrees(ConvertRadiansToDegrees(static_cast<float>(i + 2) * delta)),
			center.y + inner_radius * SinDegrees(ConvertRadiansToDegrees(static_cast<float>(i + 2) * delta)),
			0.0f);

		Vec3 back_left = Vec3(
			center.x + outer_radius * CosDegrees(ConvertRadiansToDegrees(static_cast<float>(i + 2) * delta)),
			center.y + outer_radius * SinDegrees(ConvertRadiansToDegrees(static_cast<float>(i + 2) * delta)),
			0.0f);

		vertex_array.emplace_back(back_right, color, zero_vec2);
		vertex_array.emplace_back(front_right, color, zero_vec2);
		vertex_array.emplace_back(front_left, color, zero_vec2);

		vertex_array.emplace_back(back_right, color, zero_vec2);
		vertex_array.emplace_back(front_left, color, zero_vec2);
		vertex_array.emplace_back(back_left, color, zero_vec2);
	}
}

void AddVertsForAABB2D(std::vector<Vertex_PCU>& vertex_array, const AABB2& box, const Rgba& color,
                       const Vec2& uv_at_mins, const Vec2& uv_at_maxs)
{
	const Vec2 recalculate_uv_mins = uv_at_mins;
	const Vec2 recalculate_uv_maxs = uv_at_maxs;

	vertex_array.emplace_back(Vec3(box.mins.x, box.mins.y, 0.f), color, Vec2(recalculate_uv_mins.x, recalculate_uv_mins.y));
	vertex_array.emplace_back(Vec3(box.maxs.x, box.maxs.y, 0.f), color, Vec2(recalculate_uv_maxs.x, recalculate_uv_maxs.y));
	vertex_array.emplace_back(Vec3(box.mins.x, box.maxs.y, 0.f), color, Vec2(recalculate_uv_mins.x, recalculate_uv_maxs.y));

	vertex_array.emplace_back(Vec3(box.mins.x, box.mins.y, 0.f), color, Vec2(recalculate_uv_mins.x, recalculate_uv_mins.y));
	vertex_array.emplace_back(Vec3(box.maxs.x, box.mins.y, 0.f), color, Vec2(recalculate_uv_maxs.x, recalculate_uv_mins.y));
	vertex_array.emplace_back(Vec3(box.maxs.x, box.maxs.y, 0.f), color, Vec2(recalculate_uv_maxs.x, recalculate_uv_maxs.y));
}

void AddVertsForQuad2D(std::vector<Vertex_PCU>& vertex_array, const OBB2& box, const Rgba& color,
	const Vec2& uv_at_mins, const Vec2& uv_at_maxs)
{
	AddVertsForAABB2D(vertex_array, box.GetLocalAABB2(), color, uv_at_mins, uv_at_maxs);
	TransformVertexArray(vertex_array, 1.0f, box.GetRotation(), box.GetCenter());
}

void AddVertsForCapsule(std::vector<Vertex_PCU>& vertex_array, const Capsule2& wolrd_capsule, const Rgba& color, int num_sides)
{
	Segment2 line(wolrd_capsule.m_p0, wolrd_capsule.m_p1);
	const float half_extence = line.GetLength();

	const Capsule2 local_capsule = Capsule2(Vec2(-0.5f * half_extence,  0.0f), Vec2(0.5f * half_extence,  0.0f), wolrd_capsule.m_radius);
	
	//Left side circle
	AddVertsForHalfDisc2D(vertex_array, local_capsule.m_p0, local_capsule.m_radius, color, 90.0f, num_sides);

	const Vec2 inner_box_mins(-0.5f * half_extence, -1.0f * wolrd_capsule.m_radius);
	const Vec2 inner_box_max(0.5f * half_extence, 1.0f * wolrd_capsule.m_radius);

	AddVertsForAABB2D(vertex_array, AABB2(inner_box_mins, inner_box_max), color, Vec2::ZERO, Vec2::ZERO);
	AddVertsForHalfDisc2D(vertex_array, local_capsule.m_p1, local_capsule.m_radius, color, 270.0f, num_sides);

	TransformVertexArray(vertex_array, 1.0f, wolrd_capsule.GetRotation(), wolrd_capsule.GetCenter());
}

//--------------------------------------------------------------------------------------------------
void TransformVertex(Vertex_PCU& vertexes, const float uniform_scale, const float rotation_degree_about_z,
                     const Vec2& translation_xy)
{
	vertexes.m_position = TransformPosition(vertexes.m_position, uniform_scale, rotation_degree_about_z,
	                                        translation_xy);
}

//--------------------------------------------------------------------------------------------------
void TransformVertexArray(const int num_vertexes, Vertex_PCU* vertexes, const float uniform_scale_xy,
                          const float rotation_degree_about_z, const Vec2& translation_xy)
{
	for (int i = 0; i < num_vertexes; i++)
	{
		vertexes[i].m_position = TransformPosition(vertexes[i].m_position, uniform_scale_xy, rotation_degree_about_z,
		                                           translation_xy);
	}
}

void TransformVertexArray(std::vector<Vertex_PCU>& vertex_array, const float uniform_scale_xy,
                          const float rotation_degree_about_z, const Vec2& translation_xy)
{
	for (int i = 0; i < static_cast<int>(vertex_array.size()); i++)
	{
		vertex_array[i].m_position = TransformPosition(vertex_array[i].m_position, uniform_scale_xy,
		                                               rotation_degree_about_z,
		                                               translation_xy);
	}
}

Vec2 DetermineFlippedAxes(const Vec2& input_uv)
{
	Vec2 new_uv = input_uv;
	
	if(!FLIP_TEXTURE_VERTICALLY)
		new_uv.y = (1.0f - input_uv.y);

	return new_uv;
}
