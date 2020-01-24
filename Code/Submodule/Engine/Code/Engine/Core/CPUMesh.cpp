#include "Engine/Core/CPUMesh.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/Capsule2.hpp"
#include "Engine/Math/Capsule3.hpp"
#include "Engine/Math/OBB3.hpp"
#include "Engine/Math/OBB2.hpp"
#include "Engine/Math/IntVec3.hpp"
#include "Engine/Math/Segment2.hpp"
#include "Engine/EngineCommon.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/StringUtils.hpp"

#include "ThirdParty/TinyXML2/tinyxml2.h"
#include <fstream>

//------------------------------------------------------------------------
void CpuMeshAddQuad( CPUMesh *out, const AABB2& quad )
{
	CpuMeshAddQuad( out, false, Rgba::WHITE, quad );
}

void CpuMeshAddQuad(CPUMesh* out, bool concat, const AABB2& quad)
{
	CpuMeshAddQuad( out, concat, Rgba::WHITE, quad );
}

void CpuMeshAddQuad(CPUMesh* out, bool concat, const Rgba& color, const AABB2& quad)
{
	int offset = 0;

	if(!concat) out->Clear(); 
	else offset = static_cast<int>(out->m_vertices.size());
	out->m_stamp.m_color = color; 
	//out->SetNormal( vec3::BACK ); 

	// 0 --- 1
	// |   / |
	// | /   |
	// 2 --- 3
	out->SetUV( Vec2(0.0f, 0.0f) );
	const Vec2 top_left = quad.GetTopLeft() ;
	out->AddVertex( Vec3(top_left.x, top_left.y, 0.0f), 
		Vec3(0.0f, 0.0f, -1.0f),
		Vec4(1.0f, 0.0f, 0.0f, 1.0f) ); 

	out->SetUV( Vec2(1.0f, 0.0f) );
	const Vec2 top_right = quad.GetTopRight();
	out->AddVertex( Vec3(top_right.x, top_right.y, 0.0f), 
		Vec3(0.0f, 0.0f, -1.0f),
		Vec4(1.0f, 0.0f, 0.0f, 1.0f) ); 

	out->SetUV( Vec2(0.0f, 1.0f) ); 
	const Vec2 bottom_left = quad.GetBottomLeft();
	out->AddVertex( Vec3(bottom_left.x, bottom_left.y, 0.0f), 
		Vec3(0.0f, 0.0f, -1.0f),
		Vec4(1.0f, 0.0f, 0.0f, 1.0f) ); 

	out->SetUV( Vec2(1.0f, 1.0f) ); 
	const Vec2 bottom_right = quad.GetBottomRight();
	out->AddVertex( Vec3(bottom_right.x, bottom_right.y, 0.0f), 
		Vec3(0.0f, 0.0f, -1.0f),
		Vec4(1.0f, 0.0f, 0.0f, 1.0f)); 

	out->AddIndexedQuad(offset + 0, offset + 1, offset + 2, offset + 3);

}

void CpuMeshAddQuad(CPUMesh* out, const OBB2& quad)
{
	CpuMeshAddQuad( out, false, Rgba::WHITE, quad );
}

void CpuMeshAddQuad(CPUMesh* out, bool concat, const Rgba& color, const OBB2& quad)
{
	if(!concat) out->Clear(); 

	out->m_stamp.m_color = color; 
	//out->SetNormal( vec3::BACK ); 

	// 0 --- 1
	// |   / |
	// | /   |
	// 2 --- 3
	out->SetUV( Vec2(0.0f, 0.0f) );
	const Vec2 top_left = quad.GetTopLeft();
	out->AddVertex( Vec3(top_left.x, top_left.y, 0.0f), 
		Vec3(0.0f, 0.0f, -1.0f),
	Vec4(1.0f, 0.0f, 0.0f, 1.0f)); 

	out->SetUV( Vec2(1.0f, 0.0f) );
	const Vec2 top_right = quad.GetTopRight();
	out->AddVertex( Vec3(top_right.x, top_right.y, 0.0f), 
		Vec3(0.0f, 0.0f, -1.0f),
		Vec4(1.0f, 0.0f, 0.0f, 1.0f)); 

	out->SetUV( Vec2(0.0f, 1.0f) ); 
	const Vec2 bottom_left = quad.GetBottomLeft();
	out->AddVertex( Vec3(bottom_left.x, bottom_left.y, 0.0f), 
		Vec3(0.0f, 0.0f, -1.0f),
		Vec4(1.0f, 0.0f, 0.0f, 1.0f)); 

	out->SetUV( Vec2(1.0f, 1.0f) ); 
	const Vec2 bottom_right = quad.GetBottomRight();
	out->AddVertex( Vec3(bottom_right.x, bottom_right.y, 0.0f), 
		Vec3(0.0f, 0.0f, -1.0f),
		Vec4(1.0f, 0.0f, 0.0f, 1.0f)); 

	out->AddIndexedQuad(0, 1, 2, 3);
}

void CpuMeshAddQuad(CPUMesh* out, bool concat, const Rgba& color, const AABB2& world_quad, const AABB2& wrapped_uv)
{
	if(!concat) out->Clear(); 

	out->m_stamp.m_color = color; 
	//out->SetNormal( vec3::BACK ); 

	// 0 --- 1
	// |   / |
	// | /   |
	// 2 --- 3
	out->SetUV( wrapped_uv.GetBottomLeft() );
	const Vec2 top_left = world_quad.GetTopLeft();
	out->AddVertex( Vec3(top_left.x, top_left.y, 0.0f), 
		Vec3(0.0f, 0.0f, -1.0f),
		Vec4(1.0f, 0.0f, 0.0f, 1.0f)); 

	out->SetUV( wrapped_uv.GetBottomRight() );
	const Vec2 top_right = world_quad.GetTopRight();
	out->AddVertex( Vec3(top_right.x, top_right.y, 0.0f), 
		Vec3(0.0f, 0.0f, -1.0f),
		Vec4(1.0f, 0.0f, 0.0f, 1.0f)); 
	
	out->SetUV( wrapped_uv.GetTopLeft() ); 
	const Vec2 bottom_left = world_quad.GetBottomLeft();
	out->AddVertex( Vec3(bottom_left.x, bottom_left.y, 0.0f), 
		Vec3(0.0f, 0.0f, -1.0f),
		Vec4(1.0f, 0.0f, 0.0f, 1.0f)); 

	out->SetUV( wrapped_uv.GetTopRight() ); 
	const Vec2 bottom_right = world_quad.GetBottomRight();
	out->AddVertex( Vec3(bottom_right.x, bottom_right.y, 0.0f), 
		Vec3(0.0f, 0.0f, -1.0f),
		Vec4(1.0f, 0.0f, 0.0f, 1.0f)); 

	out->AddIndexedQuad(0, 1, 2, 3);
}

void CpuMeshAddDisc(CPUMesh* out, const Rgba& color, const float radius, const int num_sides)
{
	CpuMeshAddDisc(out, color, Vec2::ZERO, radius, num_sides);
}

void  CpuMeshAddDisc(CPUMesh* out, const Rgba& color, const Vec2& center, const float radius, const int num_sides)
{
	CpuMeshAddDisc(out, false, color, center, radius, num_sides);
}

void CpuMeshAddDisc(CPUMesh* out, bool concat, const Rgba& color, const Vec2& center, float radius, int num_sides)
{
	int offset = 0;

	if(!concat) out->Clear(); 
	else offset = static_cast<int>(out->m_vertices.size());
	
	out->m_stamp.m_color = color;

	const float delta = (2.0f * MATH_PI) / static_cast<float>(num_sides);
	out->AddVertex(Vec3(center.x, center.y, 0.0f),
		Vec3(0.0f, 0.0f, -1.0f),
		Vec4(1.0f, 0.0f, 0.0f, 1.0f));
	out->SetUV( Vec2::ZERO );

	for (int i = 0; i < num_sides; i++)
	{
		out->AddVertex(
			Vec3(center.x + radius * CosDegrees(ConvertRadiansToDegrees(static_cast<float>(i) * delta)),
				center.y + radius * SinDegrees(ConvertRadiansToDegrees(static_cast<float>(i) * delta)),
				0.0f), 	Vec3(0.0f, 0.0f, -1.0f),
			Vec4(1.0f, 0.0f, 0.0f, 1.0f));

		out->AddVertex(
			Vec3(center.x + radius * CosDegrees(ConvertRadiansToDegrees(static_cast<float>(i + 1) * delta)),
				center.y + radius * SinDegrees(ConvertRadiansToDegrees(static_cast<float>(i + 1) * delta)),
				0.0f), Vec3(0.0f, 0.0f, -1.0f),
			Vec4(1.0f, 0.0f, 0.0f, 1.0f));

		//we want to draw from the center (0), to the two new vertex we create
		out->AddIndexedTriangle(offset + 0, offset + i*2 + 1, offset + i*2 + 2);
	}
}

void CpuMeshAddHalfDisc(CPUMesh* out, bool concat, const Vec2& center, const float radius, const Rgba& color,
                        const float offset_rotation_degrees, const int num_sides)
{
	int offset = 0;
	
	if(!concat) out->Clear();
	else offset = static_cast<int>(out->m_vertices.size());
	out->m_stamp.m_color = color;

	const float delta = (MATH_PI) / static_cast<float>(num_sides);
	const Vec2 zero_vec2 = Vec2(0.0f, 1.0f);
	for (int i = 0; i < num_sides; i++)
	{
		
		out->AddVertex(Vec3(center.x, center.y, 0.0f), Vec3(0.0f, 0.0f, -1.0f),
			Vec4(1.0f, 0.0f, 0.0f, 1.0f));

		out->AddVertex(
			Vec3(center.x + radius * CosDegrees(ConvertRadiansToDegrees(static_cast<float>(i) * delta) + offset_rotation_degrees),
				center.y + radius * SinDegrees(ConvertRadiansToDegrees(static_cast<float>(i) * delta) + offset_rotation_degrees),
				0.0f), Vec3(0.0f, 0.0f, -1.0f),
			Vec4(1.0f, 0.0f, 0.0f, 1.0f));

		out->AddVertex(
			Vec3(center.x + radius * CosDegrees(ConvertRadiansToDegrees(static_cast<float>(i + 1) * delta) + offset_rotation_degrees),
				center.y + radius * SinDegrees(ConvertRadiansToDegrees(static_cast<float>(i + 1) * delta) + offset_rotation_degrees),
				0.0f), Vec3(0.0f, 0.0f, -1.0f),
			Vec4(1.0f, 0.0f, 0.0f, 1.0f));

		out->AddIndexedTriangle(i*3 + offset, i*3 + 1 + offset, i*3 + 2 + offset);
	}
}

void CpuMeshAddDisc(CPUMesh* out, const Vec2& center, const float radius, const int num_sides)
{
	CpuMeshAddDisc(out, Rgba::WHITE, center, radius, num_sides);
}

void CpuMeshAddLine(CPUMesh* out, const Vec2& start, const Vec2& end, const float thickness, const Rgba& color)
{
	CpuMeshAddLine(out, false, start, end, thickness, color);
}

void CpuMeshAddLine(CPUMesh* out, bool concat, const Vec2& start, const Vec2& end, const float thickness,
	const Rgba& color)
{
	int last_index = out->GetVertexCount();

	if(!concat)
	{
		out->Clear();
		last_index = 0;
	}

	const float ht = thickness * 0.5f;
	Vec2 se = end - start;
	Vec2 hf = se.GetClamped(ht);
	Vec2 hb = -1.0f * hf;
	const Vec2 sehf = end + hf;
	const Vec2 sehb = start + hb;
	const Vec2 fr = sehf + hf.GetRotated90Degrees();
	const Vec2 fl = sehf + hf.GetRotatedMinus90Degrees();
	const Vec2 br = sehb + hb.GetRotatedMinus90Degrees();
	const Vec2 bl = sehb + hb.GetRotated90Degrees();

	out->m_stamp.m_color = color; 
	out->SetUV( Vec2::ZERO );

	out->AddVertex(Vec3(fl.x, fl.y, 0.f), Vec3(0.0f, 0.0f, -1.0f), Vec4(1.0f, 0.0f, 0.0f, 1.0f));
	out->AddVertex(Vec3(bl.x, bl.y, 0.f), Vec3(0.0f, 0.0f, -1.0f), Vec4(1.0f, 0.0f, 0.0f, 1.0f));
	out->AddVertex(Vec3(fr.x, fr.y, 0.f), Vec3(0.0f, 0.0f, -1.0f), Vec4(1.0f, 0.0f, 0.0f, 1.0f));
	out->AddVertex(Vec3(br.x, br.y, 0.f), Vec3(0.0f, 0.0f, -1.0f), Vec4(1.0f, 0.0f, 0.0f, 1.0f));
	out->AddIndexedQuad(last_index + 0, last_index + 1, last_index + 2, last_index + 3);
}

void CpuMeshAddRing2D(CPUMesh* out, const Vec2& center, float radius, float thickness, const Rgba& color, int num_sides)
{
	out->Clear(); 

	const float delta = (2.0f * MATH_PI) / static_cast<float>(num_sides);
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

		out->AddVertex(back_right, color, Vec2::ZERO, Vec3(0.0f, 0.0f, -1.0f), Vec4(1.0f, 0.0f, 0.0f, 1.0f));
		out->AddVertex(front_right, color, Vec2::ZERO, Vec3(0.0f, 0.0f, -1.0f), Vec4(1.0f, 0.0f, 0.0f , 1.0f));
		out->AddVertex(front_left, color, Vec2::ZERO, Vec3(0.0f, 0.0f, -1.0f), Vec4(1.0f, 0.0f, 0.0f, 1.0f));
		out->AddIndexedTriangle(i*6 + 0, i*6 + 1, i*6 + 2);

		out->AddVertex(back_right, color, Vec2::ZERO, Vec3(0.0f, 0.0f, -1.0f), Vec4(1.0f, 0.0f, 0.0f, 1.0f));
		out->AddVertex(front_left, color, Vec2::ZERO, Vec3(0.0f, 0.0f, -1.0f), Vec4(1.0f, 0.0f, 0.0f, 1.0f));
		out->AddVertex(back_left, color, Vec2::ZERO, Vec3(0.0f, 0.0f, -1.0f), Vec4(1.0f, 0.0f, 0.0f, 1.0f));
		out->AddIndexedTriangle(i*6 + 3, i*6 + 4, i*6 + 5);
	}
}

void CpuMeshAddRing2D(CPUMesh* out, float radius, float thickness, const Rgba& color, int num_sides)
{
	CpuMeshAddRing2D(out, Vec2::ZERO, radius,thickness, color, num_sides);
}

void CpuMeshAddTriangle(CPUMesh* out, const float side_length, const Rgba& color)
{
	out->Clear(); 

	out->m_stamp.m_color = color; 
	out->SetUV( Vec2::ZERO );


// 	Vec2 head(SQRT_3_OVER_2 * 0.5f * side_length, 0.0f);
// 	Vec2 left_foot(SQRT_3_OVER_2 * -0.5f * side_length, 0.5f * side_length);
// 	Vec2 right_foot(SQRT_3_OVER_2 * -0.5f * side_length, -0.5f * side_length);

	Vec2 head(SQRT_3_OVER_2 * side_length * 0.5f, 0.0f);
	Vec2 left_foot(SQRT_3_OVER_2 * side_length * -0.5f, 0.5f * side_length);
	Vec2 right_foot(SQRT_3_OVER_2 * side_length * -0.5f, -0.5f * side_length);

	out->AddVertex(Vec3(head), Vec3(0.0f, 0.0f, -1.0f), Vec4(1.0f, 0.0f, 0.0f, 1.0f));
	out->AddVertex(Vec3(left_foot), Vec3(0.0f, 0.0f, -1.0f), Vec4(1.0f, 0.0f, 0.0f, 1.0f));
	out->AddVertex(Vec3(right_foot), Vec3(0.0f, 0.0f, -1.0f), Vec4(1.0f, 0.0f, 0.0f, 1.0f));
	out->AddIndexedTriangle(0, 1, 2);
}

void CpuMeshAddCapsule(CPUMesh* out, const Capsule2& world_capsule, const Rgba& color, int num_sides)
{
	Segment2 line(world_capsule.m_p0, world_capsule.m_p1);
	const float half_extence = line.GetLength();

	const Capsule2 local_capsule = Capsule2(Vec2(-0.5f * half_extence,  0.0f), Vec2(0.5f * half_extence,  0.0f), world_capsule.m_radius);

	//Left side circle
	CpuMeshAddHalfDisc(out, true, local_capsule.m_p0, local_capsule.m_radius, color, 90.0f, num_sides);

	const Vec2 inner_box_mins(-0.5f * half_extence, -1.0f * world_capsule.m_radius);
	const Vec2 inner_box_max(0.5f * half_extence, 1.0f * world_capsule.m_radius);
	
	CpuMeshAddQuad(out, true, color, AABB2(inner_box_mins, inner_box_max));
	CpuMeshAddHalfDisc(out, true, local_capsule.m_p1, local_capsule.m_radius, color, 270.0f, num_sides);

	out->TransformVertexCollection(1.0f, world_capsule.GetRotation(), world_capsule.GetCenter());
}

void CpuMeshAddCube(CPUMesh* out, const AABB3& box, const AABB2& wrapped_uv)
{
	out->Clear(); 

	out->m_stamp.m_color = Rgba::WHITE; 
	//out->SetNormal( vec3::BACK ); 

	// 0 --- 1
	// |   / |
	// | /   |
	// 2 --- 3

	//Front
	out->SetUV( wrapped_uv.GetBottomLeft() );
	out->AddVertex( box.GetTopLeftFrontCorner(), 
		Vec3(0.0f, 0.0f, -1.0f), 
		Vec4(1.0f, 0.0f, 0.0f, 1.0f) ); 

	out->SetUV( wrapped_uv.GetBottomRight() );
	out->AddVertex( box.GetTopRightFrontCorner(), 
		Vec3(0.0f, 0.0f, -1.0f), 
		Vec4(1.0f, 0.0f, 0.0f, 1.0f) ); 

	out->SetUV( wrapped_uv.GetTopLeft() ); 
	out->AddVertex( box.GetBottomLeftFrontCorner(), 
		Vec3(0.0f, 0.0f, -1.0f), 
		Vec4(1.0f, 0.0f, 0.0f, 1.0f) ); 

	out->SetUV( wrapped_uv.GetTopRight() ); 
	out->AddVertex( box.GetBottomRightFrontCorner(), 
		Vec3(0.0f, 0.0f, -1.0f), 
		Vec4(1.0f, 0.0f, 0.0f, 1.0f) ); 

	out->AddIndexedQuad(0, 1, 2, 3);

	// 0 --- 1
	// |   / |
	// | /   |
	// 2 --- 3

	//Right
	out->SetUV( wrapped_uv.GetBottomLeft() );
	out->AddVertex( box.GetTopRightFrontCorner(), 
		Vec3(1.0f, 0.0f, 0.0f), 
		Vec4(0.0f, 0.0f, 1.0f, 1.0f) ); 

	out->SetUV( wrapped_uv.GetBottomRight() );
	out->AddVertex( box.GetTopRightBackCorner(), 
		Vec3(1.0f, 0.0f, 0.0f), 
		Vec4(0.0f, 0.0f, 1.0f, 1.0f) ); 

	out->SetUV( wrapped_uv.GetTopLeft() ); 
	out->AddVertex( box.GetBottomRightFrontCorner(), 
		Vec3(1.0f, 0.0f, 0.0f), 
		Vec4(0.0f, 0.0f, 1.0f, 1.0f) ); 

	out->SetUV( wrapped_uv.GetTopRight() ); 
	out->AddVertex( box.GetBottomRightBackCorner(), 
		Vec3(1.0f, 0.0f, 0.0f), 
		Vec4(0.0f, 0.0f, 1.0f, 1.0f) ); 

	out->AddIndexedQuad(4, 5, 6, 7);

	//Top
	out->SetUV( wrapped_uv.GetBottomLeft() );
	out->AddVertex( box.GetTopLeftBackCorner(), 
		Vec3(0.0f, 1.0f, 0.0f), 
		Vec4(1.0f, 0.0f, 0.0f, 1.0f) ); 

	out->SetUV( wrapped_uv.GetBottomRight() );
	out->AddVertex( box.GetTopRightBackCorner(), 
		Vec3(0.0f, 1.0f, 0.0f), 
		Vec4(1.0f, 0.0f, 0.0f, 1.0f) ); 

	out->SetUV( wrapped_uv.GetTopLeft() );
	out->AddVertex( box.GetTopLeftFrontCorner(), 
		Vec3(0.0f, 1.0f, 0.0f), 
		Vec4(1.0f, 0.0f, 0.0f, 1.0f) ); 

	out->SetUV( wrapped_uv.GetTopRight() ); 
	out->AddVertex( box.GetTopRightFrontCorner(), 
		Vec3(0.0f, 1.0f, 0.0f), 
		Vec4(1.0f, 0.0f, 0.0f, 1.0f) ); 

	out->AddIndexedQuad(8, 9, 10, 11);

	//Bottom
	out->SetUV( wrapped_uv.GetBottomLeft() );
	out->AddVertex( box.GetBottomLeftFrontCorner(), 
		Vec3(0.0f, -1.0f, 0.0f), 
		Vec4(1.0f, 0.0f, 0.0f, 1.0f)); 

	out->SetUV( wrapped_uv.GetBottomRight() );
	out->AddVertex( box.GetBottomRightFrontCorner(), 
		Vec3(0.0f, -1.0f, 0.0f), 
		Vec4(1.0f, 0.0f, 0.0f, 1.0f) ); 

	out->SetUV( wrapped_uv.GetTopLeft() );
	out->AddVertex( box.GetBottomLeftBackCorner(), 
		Vec3(0.0f, -1.0f, 0.0f), 
		Vec4(1.0f, 0.0f, 0.0f, 1.0f) ); 

	out->SetUV( wrapped_uv.GetTopRight() ); 
	out->AddVertex( box.GetBottomRightBackCorner(), 
		Vec3(0.0f, -1.0f, 0.0f), 
		Vec4(1.0f, 0.0f, 0.0f, 1.0f) ); 

	out->AddIndexedQuad(12, 13, 14, 15);

	//Back
	out->SetUV( wrapped_uv.GetBottomLeft() );
	out->AddVertex( box.GetTopRightBackCorner(), 
		Vec3(0.0f, 0.0f, 1.0f), 
		Vec4(-1.0f, 0.0f, 0.0f, 1.0f) ); 

	out->SetUV( wrapped_uv.GetBottomRight() );
	out->AddVertex( box.GetTopLeftBackCorner(), 
		Vec3(0.0f, 0.0f, 1.0f), 
		Vec4(-1.0f, 0.0f, 0.0f, 1.0f) ); 

	out->SetUV( wrapped_uv.GetTopLeft() );
	out->AddVertex( box.GetBottomRightBackCorner(), 
		Vec3(0.0f, 0.0f, 1.0f), 
		Vec4(-1.0f, 0.0f, 0.0f, 1.0f) ); 

	out->SetUV( wrapped_uv.GetTopRight() ); 
	out->AddVertex( box.GetBottomLeftBackCorner(), 
		Vec3(0.0f, 0.0f, 1.0f), 
		Vec4(-1.0f, 0.0f, 0.0f, 1.0f) ); 

	out->AddIndexedQuad(16, 17, 18, 19);

	//Left
	out->SetUV( wrapped_uv.GetBottomLeft() );
	out->AddVertex( box.GetTopLeftBackCorner(), 
		Vec3(-1.0f, 0.0f, 0.0f), 
		Vec4(0.0f, 0.0f, -1.0f, 1.0f) ); 

	out->SetUV( wrapped_uv.GetBottomRight() );
	out->AddVertex( box.GetTopLeftFrontCorner(), 
		Vec3(-1.0f, 0.0f, 0.0f), 
		Vec4(0.0f, 0.0f, -1.0f, 1.0f)  ); 

	out->SetUV( wrapped_uv.GetTopLeft() );
	out->AddVertex( box.GetBottomLeftBackCorner(), 
		Vec3(-1.0f, 0.0f, 0.0f), 
		Vec4(0.0f, 0.0f, -1.0f, 1.0f)  ); 

	out->SetUV( wrapped_uv.GetTopRight() ); 
	out->AddVertex( box.GetBottomLeftFrontCorner(), 
		Vec3(-1.0f, 0.0f, 0.0f), 
		Vec4(0.0f, 0.0f, -1.0f, 1.0f)  ); 

	out->AddIndexedQuad(20, 21, 22, 23);


}

void CpuMeshAddCube(CPUMesh* out, const OBB3& box, const AABB2& wrapped_uv)
{
	out->Clear(); 

	out->m_stamp.m_color = Rgba::WHITE; 
	//out->SetNormal( vec3::BACK ); 

	// 0 --- 1
	// |   / |
	// | /   |
	// 2 --- 3

	//Front
	out->SetUV( wrapped_uv.GetBottomLeft() );
	out->AddVertex( box.GetTopLeftFrontCorner(), 
		Vec3(0.0f, 0.0f, -1.0f), 
		Vec4(1.0f, 0.0f, 0.0f, 1.0f) ); 

	out->SetUV( wrapped_uv.GetBottomRight() );
	out->AddVertex( box.GetTopRightFrontCorner(), 
		Vec3(0.0f, 0.0f, -1.0f), 
		Vec4(1.0f, 0.0f, 0.0f, 1.0f) ); 

	out->SetUV( wrapped_uv.GetTopLeft() );
	out->AddVertex( box.GetBottomLeftFrontCorner(), 
		Vec3(0.0f, 0.0f, -1.0f), 
		Vec4(1.0f, 0.0f, 0.0f, 1.0f) ); 

	out->SetUV( wrapped_uv.GetTopRight() ); 
	out->AddVertex( box.GetBottomRightFrontCorner(), 
		Vec3(0.0f, 0.0f, -1.0f), 
		Vec4(1.0f, 0.0f, 0.0f, 1.0f) ); 

	out->AddIndexedQuad(0, 1, 2, 3);

	//Right
	out->SetUV( wrapped_uv.GetBottomLeft() );
	out->AddVertex( box.GetTopRightFrontCorner(), 
		Vec3(1.0f, 0.0f, 0.0f), 
		Vec4(0.0f, 0.0f, 1.0f, 1.0f) ); 

	out->SetUV( wrapped_uv.GetBottomRight() );
	out->AddVertex( box.GetTopRightBackCorner(), 
		Vec3(1.0f, 0.0f, 0.0f), 
		Vec4(0.0f, 0.0f, 1.0f, 1.0f) ); 

	out->SetUV( wrapped_uv.GetTopLeft() ); 
	out->AddVertex( box.GetBottomRightFrontCorner(), 
		Vec3(1.0f, 0.0f, 0.0f), 
		Vec4(0.0f, 0.0f, 1.0f, 1.0f) ); 

	out->SetUV( wrapped_uv.GetTopRight() );
	out->AddVertex( box.GetBottomRightBackCorner(), 
		Vec3(1.0f, 0.0f, 0.0f), 
		Vec4(0.0f, 0.0f, 1.0f, 1.0f) ); 

	out->AddIndexedQuad(4, 5, 6, 7);

	//Top
	out->SetUV( wrapped_uv.GetBottomLeft() );
	out->AddVertex( box.GetTopLeftBackCorner(), 
		Vec3(0.0f, 1.0f, 0.0f), 
		Vec4(1.0f, 0.0f, 0.0f, 1.0f) ); 

	out->SetUV( wrapped_uv.GetBottomRight() );
	out->AddVertex( box.GetTopRightBackCorner(), 
		Vec3(0.0f, 1.0f, 0.0f), 
		Vec4(1.0f, 0.0f, 0.0f, 1.0f) ); 

	out->SetUV( wrapped_uv.GetTopLeft() ); 
	out->AddVertex( box.GetTopLeftFrontCorner(), 
		Vec3(0.0f, 1.0f, 0.0f), 
		Vec4(1.0f, 0.0f, 0.0f, 1.0f) ); 

	out->SetUV( wrapped_uv.GetTopRight() );
	out->AddVertex( box.GetTopRightFrontCorner(), 
		Vec3(0.0f, 1.0f, 0.0f), 
		Vec4(1.0f, 0.0f, 0.0f, 1.0f) ); 

	out->AddIndexedQuad(8, 9, 10, 11);

	//Bottom
	out->SetUV( wrapped_uv.GetBottomLeft() );
	out->AddVertex( box.GetBottomLeftFrontCorner(), 
		Vec3(0.0f, -1.0f, 0.0f), 
		Vec4(1.0f, 0.0f, 0.0f, 1.0f)); 

	out->SetUV( wrapped_uv.GetBottomRight() );
	out->AddVertex( box.GetBottomRightFrontCorner(), 
		Vec3(0.0f, -1.0f, 0.0f), 
		Vec4(1.0f, 0.0f, 0.0f, 1.0f) ); 

	out->SetUV( wrapped_uv.GetTopLeft() ); 
	out->AddVertex( box.GetBottomLeftBackCorner(), 
		Vec3(0.0f, -1.0f, 0.0f), 
		Vec4(1.0f, 0.0f, 0.0f, 1.0f) ); 

	out->SetUV( wrapped_uv.GetTopRight() );
	out->AddVertex( box.GetBottomRightBackCorner(), 
		Vec3(0.0f, -1.0f, 0.0f), 
		Vec4(1.0f, 0.0f, 0.0f, 1.0f) ); 

	out->AddIndexedQuad(12, 13, 14, 15);

	//Back
	out->SetUV( wrapped_uv.GetBottomLeft() );
	out->AddVertex( box.GetTopRightBackCorner(), 
		Vec3(0.0f, 0.0f, 1.0f), 
		Vec4(-1.0f, 0.0f, 0.0f, 1.0f) ); 

	out->SetUV( wrapped_uv.GetBottomRight() );
	out->AddVertex( box.GetTopLeftBackCorner(), 
		Vec3(0.0f, 0.0f, 1.0f), 
		Vec4(-1.0f, 0.0f, 0.0f, 1.0f) ); 

	out->SetUV( wrapped_uv.GetTopLeft() ); 
	out->AddVertex( box.GetBottomRightBackCorner(), 
		Vec3(0.0f, 0.0f, 1.0f), 
		Vec4(-1.0f, 0.0f, 0.0f, 1.0f) ); 

	out->SetUV( wrapped_uv.GetTopRight() );
	out->AddVertex( box.GetBottomLeftBackCorner(), 
		Vec3(0.0f, 0.0f, 1.0f), 
		Vec4(-1.0f, 0.0f, 0.0f, 1.0f) ); 

	out->AddIndexedQuad(16, 17, 18, 19);

	//Left
	out->SetUV( wrapped_uv.GetBottomLeft() );
	out->AddVertex( box.GetTopLeftBackCorner(), 
		Vec3(-1.0f, 0.0f, 0.0f), 
		Vec4(0.0f, 0.0f, -1.0f, 1.0f) ); 

	out->SetUV( wrapped_uv.GetBottomRight() );
	out->AddVertex( box.GetTopLeftFrontCorner(), 
		Vec3(-1.0f, 0.0f, 0.0f), 
		Vec4(0.0f, 0.0f, -1.0f, 1.0f)  ); 

	out->SetUV( wrapped_uv.GetTopLeft() ); 
	out->AddVertex( box.GetBottomLeftBackCorner(), 
		Vec3(-1.0f, 0.0f, 0.0f), 
		Vec4(0.0f, 0.0f, -1.0f, 1.0f)  ); 

	out->SetUV( wrapped_uv.GetTopRight() );
	out->AddVertex( box.GetBottomLeftFrontCorner(), 
		Vec3(-1.0f, 0.0f, 0.0f), 
		Vec4(0.0f, 0.0f, -1.0f, 1.0f)  ); 

	out->AddIndexedQuad(20, 21, 22, 23);


}

void CpuMeshAddUVSphere(CPUMesh* out, Vec3 center, float radius, unsigned int wedges, unsigned int slices)
{
	CpuMeshAddUVSphere(out, false, center, radius, wedges, slices);
}

void CpuMeshAddUVSphere(CPUMesh* out, bool concat, Vec3 center, float radius, unsigned wedges, unsigned slices)
{
	int last_vertex_index = 0;
	
	if(!concat) {
		out->Clear();
	}
	else
	{
		last_vertex_index = static_cast<int>(out->m_vertices.size());
	}
	
	out->m_stamp.m_color = Rgba::WHITE;

	const unsigned int u_step = wedges + 1;
	const unsigned int v_step = slices + 1;

	for(unsigned int v_id = 0; v_id < v_step; ++v_id)
	{
		const float v = static_cast<float>(v_id) / static_cast<float>(slices);
		const float pitch_degrees = RangeMapLinearFloat(v, 0.0f, 1.0f, 90.0f, -90.f);

		for(unsigned int u_id = 0; u_id < u_step; ++u_id)
		{
			const float u = static_cast<float>(u_id) / static_cast<float>(wedges);
			const float yaw_degrees = u * 360.0f;
			out->SetUV( Vec2(u, v) );

			float cos_pitch = CosDegrees(pitch_degrees);
			float sin_pitch = SinDegrees(pitch_degrees);
			float cos_yaw = CosDegrees(yaw_degrees);
			float sin_yaw = SinDegrees(yaw_degrees);

			Vec3 pose_norm = Vec3(cos_pitch * cos_yaw, 
				sin_pitch, 
				cos_pitch*sin_yaw);

			// calculated the tangent by taking the partial derivative of the m_normal, with respect to yaw
			Vec4 pose_tangent(cos_pitch * (-1.0f * sin_yaw),
				0.0f,
				cos_pitch*cos_yaw, 
				1.0f);

			//alternatively, could have calculated the tangent with surface plane
			//given (x,y,z) and (x+1,y+1,z+1), the tangent would be (x+1,y+1,z+1) - (x,y,z)
			// where (x+1,y+1,z+1) is ((u_id + 1)/(wedges)) * 360.0f, calculating the m_normal position
			// taking the diffrence, and normalizing

			out->AddVertex( center + pose_norm * radius, pose_norm, pose_tangent);
		}
	}

	for(unsigned int y = 0; y < slices; ++y)
	{
		for(unsigned int x = 0; x < wedges; ++x)
		{
			const unsigned int top_left = last_vertex_index + (y * u_step + x);
			const unsigned int top_right = (top_left + 1);
			const unsigned int bottom_left = (top_left + u_step);
			const unsigned int bottom_right = (bottom_left + 1);

			out->AddIndexedQuad(top_left, top_right, bottom_left, bottom_right);
		}
	}
}

void CpuMeshAddCapsule(CPUMesh* out, float radius, float length, unsigned num_sides, unsigned num_slices)
{
	CpuMeshAddUVSphere(out, false, Vec3(0.0f + radius, 0.0f, 0.0f), radius, num_sides, num_slices);
	CpuMeshAddCylinder(out, true, radius, radius, length, radius, num_sides);
	CpuMeshAddUVSphere(out, true, Vec3(0.0f + radius + length, 0.0f, 0.0f), radius, num_sides, num_slices);
}

void CpuMeshAddCapsule(CPUMesh* out, const Capsule3& cap, unsigned num_sides, unsigned num_slices)
{
	Vec3 displacement = cap.end - cap.start;
	const float length = displacement.GetLength();
	CpuMeshAddUVSphere(out, false, Vec3( 0.0f, 0.0f, 0.0f), cap.radius, num_sides, num_slices);
	CpuMeshAddCylinder(out, true, cap.radius, cap.radius, length, 0.0f, num_sides);
	CpuMeshAddUVSphere(out, true, Vec3( length, 0.0f, 0.0f), cap.radius, num_sides, num_slices);
}

// void CpuMeshAddCylinder(CPUMesh* out, const Capsule3& world_capsule, unsigned num_sides)
// {
// 	Vec3 displacment = world_capsule.end - world_capsule.start;
// 	CpuMeshAddCylinder( out, false, world_capsule.radius, world_capsule.radius,  displacment.GetLength(), 0.0f, 16);
// }

void CpuMeshAddCylinder(CPUMesh* out, float radius, float length, unsigned int num_sides)
{
	CpuMeshAddCylinder( out, false, radius, radius, length, 0.0f,  num_sides);
}

void CpuMeshAddCylinder(CPUMesh *out, float bottom_radius, float top_radius, float length,  unsigned int num_sides)
{
	CpuMeshAddCylinder( out, false, bottom_radius, top_radius, length, 0.0f,  num_sides);

}

void CpuMeshAddCylinder( CPUMesh * out, bool concat_last_cpu_mesh, float bottom_radius, float top_radius, float length, float displacement, unsigned int num_sides )
{
	int last_vertex_index = 0;
	
	if(!concat_last_cpu_mesh) out->Clear();
	else last_vertex_index = static_cast<int>(out->m_vertices.size());


	out->m_stamp.m_color = Rgba::WHITE;
	Vec3 bottom_center(displacement, 0.0f, 0.0f);
	Vec3 top_center(displacement + length, 0.0f, 0.0f);

	const float delta = (2.0f * MATH_PI) / static_cast<float>(num_sides);
	out->SetUV( Vec2::ZERO );
	out->AddVertex(bottom_center); //bottom disc
	out->AddVertex(top_center);

	for (int i = 0; i < static_cast<int>(num_sides); i++)
	{
		//bottom disc
		out->AddVertex(
			Vec3(bottom_center.x,
				bottom_center.y + bottom_radius * SinDegrees(ConvertRadiansToDegrees(static_cast<float>(i) * delta)),
				bottom_center.z + bottom_radius * CosDegrees(ConvertRadiansToDegrees(static_cast<float>(i) * delta))));

		out->AddVertex(
			Vec3(bottom_center.x,
				bottom_center.y + bottom_radius * SinDegrees(ConvertRadiansToDegrees(static_cast<float>(i + 1) * delta)),
				bottom_center.z + bottom_radius * CosDegrees(ConvertRadiansToDegrees(static_cast<float>(i + 1) * delta))));

		//top disc
		out->AddVertex(
			Vec3(top_center.x,
				top_center.y + top_radius * SinDegrees(ConvertRadiansToDegrees(static_cast<float>(i) * delta)),
				top_center.z + top_radius * CosDegrees(ConvertRadiansToDegrees(static_cast<float>(i) * delta))));

		out->AddVertex(
			Vec3(top_center.x,
				top_center.y + top_radius * SinDegrees(ConvertRadiansToDegrees(static_cast<float>(i + 1) * delta)),
				top_center.z + top_radius * CosDegrees(ConvertRadiansToDegrees(static_cast<float>(i + 1) * delta))));

		//we want to draw from the center (0), to the two new vertex we create
		out->AddIndexedTriangle(last_vertex_index + 0, last_vertex_index + i*4 + 3, last_vertex_index + i*4 + 2); //bottom segment
		out->AddIndexedQuad(last_vertex_index + i*4 + 2, last_vertex_index + i*4 + 4, last_vertex_index + i*4 + 3, last_vertex_index + i*4 + 5); //mid segment
		out->AddIndexedTriangle(last_vertex_index + 1, last_vertex_index + i*4 + 4, last_vertex_index + i*4 + 5); // top segment
	}
}

void CpuMeshAddQuad(CPUMesh* out, const Vec3& bottom_left, const Vec3& bottom_right, const Vec3& top_left, const AABB2& wrapped_uv)
{
	out->Clear(); 

	out->m_stamp.m_color = Rgba::WHITE;
	Vec3 normal = Cross(Vec3(bottom_right - bottom_left), Vec3(top_left - bottom_left));
	normal.Normalize();

	//left_bottom_front, right_bottom_front, right_top_front
	// 0 --- 1
	// |   / |
	// | /   |
	// 2 --- 3

	//Front
	out->SetUV(wrapped_uv.GetBottomLeft());
	out->AddVertex( top_left, normal); 

	out->SetUV(wrapped_uv.GetBottomRight());
	out->AddVertex( bottom_right + Vec3(top_left - bottom_left), normal); 

	out->SetUV(wrapped_uv.GetTopLeft()); 
	out->AddVertex( bottom_left, normal); 

	out->SetUV(wrapped_uv.GetTopRight()); 
	out->AddVertex( bottom_right, normal); 

	out->AddIndexedQuad(0, 1, 2, 3);	
}

CPUMesh::CPUMesh()
{
	m_stamp.m_position = Vec3(0.0f, 0.0f, 0.0f);
	m_stamp.m_color = Rgba::WHITE;
	m_stamp.m_uv = Vec2::ZERO;
	m_stamp.m_normal = Vec3::ZERO;
}

CPUMesh::~CPUMesh()
{
}

void CPUMesh::Clear()
{
	m_vertices.clear();
	m_indices.clear();
}

void CPUMesh::SetColor(const Rgba& color)
{
	for(std::vector<Vertex_Master>::iterator vert_it = m_vertices.begin();
		vert_it != m_vertices.end();
		/*++it*/)
	{
		vert_it->m_color = color;
		++vert_it;
	}
}

void CPUMesh::SetUV(const Vec2& uv)
{
	m_stamp.m_uv = uv;
}

void CPUMesh::ConcatCPUMesh(const CPUMesh& sub_set)
{
	const int vert_index_offset = static_cast<int>(m_vertices.size());
	const int sub_set_vert_list_size = static_cast<int>(sub_set.m_vertices.size());
	const int sub_set_indices_list_size = static_cast<int>(sub_set.m_indices.size());

	for(int sub_vert_idx = 0; sub_vert_idx < sub_set_vert_list_size; ++sub_vert_idx)
	{
		m_vertices.push_back(sub_set.m_vertices[sub_vert_idx]);
	}

	for(int sub_indices_idx = 0; sub_indices_idx < sub_set_indices_list_size; ++sub_indices_idx)
	{
		m_indices.push_back(sub_set.m_indices[sub_indices_idx] + vert_index_offset);
	}
}

unsigned int CPUMesh::AddVertex(const Vertex_Master& m)
{
	m_vertices.push_back(m);
	return static_cast<unsigned int>(m_vertices.size());
}

unsigned int CPUMesh::AddVertex(const Vec3& pos)
{
	m_stamp.m_position = pos;
	m_vertices.push_back(m_stamp);
	return static_cast<unsigned int>(m_vertices.size());
}

unsigned CPUMesh::AddVertex( const Vec3& pos, const Vec3& normal)
{
	m_stamp.m_position = pos;
	m_stamp.m_normal = normal;
	m_vertices.push_back(m_stamp);
	return static_cast<unsigned int>(m_vertices.size());
}

unsigned CPUMesh::AddVertex(const Vec3& pos, const Vec3& normal, const Vec4& tangent)
{
	m_stamp.m_position = pos;
	m_stamp.m_normal = normal;
	m_stamp.m_tangent = tangent;
	m_vertices.push_back(m_stamp);
	return static_cast<unsigned int>(m_vertices.size());
}

unsigned CPUMesh::AddVertex(const Vec3& pos, const Rgba& col, const Vec2& uv)
{
	m_stamp.m_position = pos;
	m_stamp.m_color = col;
	m_stamp.m_uv = uv;
	m_vertices.push_back(m_stamp);
	return static_cast<unsigned int>(m_vertices.size());
}

unsigned CPUMesh::AddVertex(const Vec3& pos, const Rgba& col, const Vec2& uv, const Vec3& normal)
{
	m_stamp.m_position = pos;
	m_stamp.m_color = col;
	m_stamp.m_uv = uv;
	m_stamp.m_normal = normal;
	m_vertices.push_back(m_stamp);
	return static_cast<unsigned int>(m_vertices.size());
}

unsigned CPUMesh::AddVertex(const Vec3& pos, const Rgba& col, const Vec2& uv, const Vec3& normal, const Vec4& tangent)
{
	m_stamp.m_position = pos;
	m_stamp.m_color = col;
	m_stamp.m_uv = uv;
	m_stamp.m_normal = normal;
	m_stamp.m_tangent = tangent;
	m_vertices.push_back(m_stamp);
	return static_cast<unsigned int>(m_vertices.size());
}

//------------------------------------------------------------------------
void CPUMesh::AddIndexedTriangle( const unsigned int i0, const unsigned int i1, const unsigned int i2 )
{
	ASSERT_OR_DIE( i0 < m_vertices.size(), "triangle indices is larger than the total number of Vertices" );
	ASSERT_OR_DIE( i1 < m_vertices.size(), "triangle indices is larger than the total number of Vertices" );
	ASSERT_OR_DIE( i2 < m_vertices.size(), "triangle indices is larger than the total number of Vertices" );

	m_indices.push_back(i0);
	m_indices.push_back(i1);
	m_indices.push_back(i2);
}

void CPUMesh::AddIndexedQuad(unsigned int topLeft, unsigned int topRight, unsigned int bottomLeft, unsigned int bottomRight)
{
	AddIndexedTriangle( bottomLeft, bottomRight, topRight );
	AddIndexedTriangle( topLeft, bottomLeft, topRight ); 
}

unsigned int CPUMesh::GetVertexCount() const
{
	return static_cast<unsigned int>(m_vertices.size());
}

unsigned int CPUMesh::GetIndexCount() const
{
	return static_cast<unsigned int>(m_indices.size());
}

void CPUMesh::TransformVertex(float uniform_scale, float rotation_degree_about_z,
	const Vec2& translation_xy)
{
	m_stamp.m_position = TransformPosition(m_stamp.m_position, uniform_scale, rotation_degree_about_z, translation_xy);
}

void CPUMesh::TransformVertex(const Matrix44& mat44, float scale_value)
{
	m_stamp.m_normal = mat44.GetTransformPosition3D(m_stamp.m_normal);
	Matrix44 scaled_mat = mat44;
	scaled_mat = scaled_mat * scale_value;
	m_stamp.m_position = scaled_mat.GetTransformPosition3D(m_stamp.m_position);
}

void CPUMesh::TransformVertexCollection(float uniform_scale_xy, float rotation_degree_about_z,
	const Vec2& translation_xy)
{
	for (int i = 0; i < static_cast<int>(m_vertices.size()); i++)
	{
		m_vertices[i].m_position = TransformPosition(m_vertices[i].m_position, uniform_scale_xy, rotation_degree_about_z, translation_xy);
	}
}

void CPUMesh::TransformVertexCollection(const Matrix44& mat44, float scale_value)
{
	for (int i = 0; i < static_cast<int>(m_vertices.size()); i++)
	{
		m_vertices[i].m_normal = mat44.GetTransformVector3D(m_vertices[i].m_normal);
		Matrix44 scaled_mat = mat44;
		scaled_mat = scaled_mat * scale_value;
		m_vertices[i].m_position = scaled_mat.GetTransformPosition3D(m_vertices[i].m_position);
	}
}

bool CPUMesh::UsesIndexBuffer() const
{
	return GetIndexCount() > 0; 
}

unsigned int CPUMesh::GetElementCount() const
{
	return UsesIndexBuffer() ? GetIndexCount() : GetVertexCount(); 
}

NamedStrings CPUMesh::ReadMeshXml(std::string file_dir, std::string xml_element)
{
	//this xml has a specif format, cannot generalize this in XML utils
	NamedStrings obj_meta_data;
	
	tinyxml2::XMLDocument xml_doc;
	std::string file_des = file_dir;
	tinyxml2::XMLError e_result = xml_doc.LoadFile(file_des.c_str());
	ASSERT_OR_DIE(e_result == tinyxml2::XML_SUCCESS, Stringf("Could not load %s, xml error: %s", 
		file_des.c_str(), 
		tinyxml2::XMLDocument::ErrorIDToName(e_result)));

	tinyxml2::XMLNode* p_root = xml_doc.FirstChild();
	ASSERT_OR_DIE(p_root != nullptr, Stringf("Could not read %s, xml error: %s", 
		file_des.c_str(), 
		tinyxml2::XMLDocument::ErrorIDToName(tinyxml2::XML_ERROR_FILE_READ_ERROR)));

	tinyxml2::XMLElement* p_element = p_root->FirstChildElement(xml_element.c_str());
	obj_meta_data.PopulateFromXmlElementAttributes(*p_element);

	return obj_meta_data;
}

TODO("This is bair minimum, need to create additonal functions that explain what is going on")
void CPUMesh::PopulateFromMeshFile( const NamedStrings& obj_meta_data, bool invert_face, bool invert_uvs )
{
	Clear();
	std::vector<Vec3> vertex_database;
	vertex_database.emplace_back(Vec3::ZERO);

	std::vector<Vec2> uv_database;
	uv_database.emplace_back(Vec2::ZERO);

	std::vector<Vec3> normal_database;
	normal_database.emplace_back(Vec3::ZERO);

	
	std::string line;
	std::ifstream myfile (obj_meta_data.GetValue("src", "garbage").c_str());
	if (myfile.is_open())
	{
		while ( getline (myfile,line) )
		{
			//std::cout << line << '\n';
			int space_index = static_cast<int>(line.find_first_of(' '));
			std::string data_type = line.substr(0, space_index);
			std::string data = line.substr(space_index+1);

			if(data_type[0] == '#') continue;
			else if (data_type[0] == 'v')
			{
				if( static_cast<int>(data_type.size()) == 1) // this is a Geometric Vertex
				{
					vertex_database.emplace_back(Vec3::ZERO);
					vertex_database[vertex_database.size()-1].SetFromText(data.c_str(), ' ');
				}
				else if(data_type[1] == 't') // this is a uv
				{
					uv_database.emplace_back(Vec2::ZERO);
					uv_database[uv_database.size()-1].SetFromText(data.c_str(), ' ');

					if(invert_uvs)
					{
						uv_database[uv_database.size()-1] = Vec2(uv_database[uv_database.size()-1].x, 
							1.0f - uv_database[uv_database.size()-1].y);
					}
				}
				else if(data_type[1] == 'n') // this is a m_normal
				{
					normal_database.emplace_back(Vec3::ZERO);
					normal_database[normal_database.size()-1].SetFromText(data.c_str(), ' ');
				}
			}
			else if (data_type[0] == 'f')
			{
				std::vector<std::string> split_face = SplitStringOnDelimiter(data, ' ');

				ASSERT_OR_DIE(split_face.size() >= 3, Stringf("Expecting a face from the following line, %s", data.c_str()));

				if(split_face[split_face.size() - 1].empty())
					split_face.pop_back();

				if(split_face.size() == 3) // adding a triangle
				{
					IntVec3 vertex_indices_0 = IntVec3::ZERO; 
					vertex_indices_0.SetFromText(split_face[0].c_str(), '/');
					IntVec3 vertex_indices_1 = IntVec3::ZERO;
					vertex_indices_1.SetFromText(split_face[1].c_str(), '/');
					IntVec3 vertex_indices_2 = IntVec3::ZERO;
					vertex_indices_2.SetFromText(split_face[2].c_str(), '/');

					if(invert_face)
					{
						AddVertex(vertex_database[vertex_indices_0.x], Rgba::WHITE, uv_database[vertex_indices_0.y], normal_database[vertex_indices_0.z]);
						AddVertex(vertex_database[vertex_indices_1.x], Rgba::WHITE, uv_database[vertex_indices_1.y], normal_database[vertex_indices_1.z]);
						AddVertex(vertex_database[vertex_indices_2.x], Rgba::WHITE, uv_database[vertex_indices_2.y], normal_database[vertex_indices_2.z]);
					}
					else
					{
						AddVertex(vertex_database[vertex_indices_2.x], Rgba::WHITE, uv_database[vertex_indices_2.y], normal_database[vertex_indices_2.z]);
						AddVertex(vertex_database[vertex_indices_1.x], Rgba::WHITE, uv_database[vertex_indices_1.y], normal_database[vertex_indices_1.z]);
						AddVertex(vertex_database[vertex_indices_0.x], Rgba::WHITE, uv_database[vertex_indices_0.y], normal_database[vertex_indices_0.z]);
					}
				}
				else if(split_face.size() == 4) // adding a quad
				{
					IntVec3 vertex_indices_0 = IntVec3::ZERO; 
					vertex_indices_0.SetFromText(split_face[0].c_str(), '/');
					IntVec3 vertex_indices_1 = IntVec3::ZERO;
					vertex_indices_1.SetFromText(split_face[1].c_str(), '/');
					IntVec3 vertex_indices_2 = IntVec3::ZERO;
					vertex_indices_2.SetFromText(split_face[2].c_str(), '/');
					IntVec3 vertex_indices_3 = IntVec3::ZERO;
					vertex_indices_3.SetFromText(split_face[3].c_str(), '/');

					if(invert_face)
					{
						AddVertex(vertex_database[vertex_indices_0.x], Rgba::WHITE, uv_database[vertex_indices_0.y], normal_database[vertex_indices_0.z]);
						AddVertex(vertex_database[vertex_indices_1.x], Rgba::WHITE, uv_database[vertex_indices_1.y], normal_database[vertex_indices_1.z]);
						AddVertex(vertex_database[vertex_indices_2.x], Rgba::WHITE, uv_database[vertex_indices_2.y], normal_database[vertex_indices_2.z]);

						AddVertex(vertex_database[vertex_indices_2.x], Rgba::WHITE, uv_database[vertex_indices_2.y], normal_database[vertex_indices_2.z]);
						AddVertex(vertex_database[vertex_indices_3.x], Rgba::WHITE, uv_database[vertex_indices_3.y], normal_database[vertex_indices_3.z]);
						AddVertex(vertex_database[vertex_indices_0.x], Rgba::WHITE, uv_database[vertex_indices_0.y], normal_database[vertex_indices_0.z]);

					}
					else
					{
						AddVertex(vertex_database[vertex_indices_0.x], Rgba::WHITE, uv_database[vertex_indices_0.y], normal_database[vertex_indices_0.z]);
						AddVertex(vertex_database[vertex_indices_3.x], Rgba::WHITE, uv_database[vertex_indices_3.y], normal_database[vertex_indices_3.z]);
						AddVertex(vertex_database[vertex_indices_2.x], Rgba::WHITE, uv_database[vertex_indices_2.y], normal_database[vertex_indices_2.z]);

						AddVertex(vertex_database[vertex_indices_2.x], Rgba::WHITE, uv_database[vertex_indices_2.y], normal_database[vertex_indices_2.z]);
						AddVertex(vertex_database[vertex_indices_1.x], Rgba::WHITE, uv_database[vertex_indices_1.y], normal_database[vertex_indices_1.z]);
						AddVertex(vertex_database[vertex_indices_0.x], Rgba::WHITE, uv_database[vertex_indices_0.y], normal_database[vertex_indices_0.z]);
					}
				}
			}
		}
		myfile.close();
	}

	else ERROR_AND_DIE("Could not read .obj file")
}
