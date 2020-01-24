#pragma once
#include <vector>
#include "Engine/Math/Vec2.hpp"

struct OBB2;
struct Capsule2;
struct AABB2;
struct Vertex_PCU;
struct Rgba;


void AddVertsForDisc2D(std::vector<Vertex_PCU>& vertex_array, const Vec2& center, float radius, const Rgba& color,
                       int num_sides = 64);
void AddVertsForHalfDisc2D(std::vector<Vertex_PCU>& vertex_array, const Vec2& center, float radius, const Rgba& color,
						float offset_rotation_degrees, int num_sides = 32);
void AddVertsForLine2D(std::vector<Vertex_PCU>& vertex_array, const Vec2& start, const Vec2& end, float thickness,
                       const Rgba& color);
void AddVertsForRing2D(std::vector<Vertex_PCU>& vertex_array, const Vec2& center, float radius, float thickness,
                       const Rgba& color, int num_sides = 64);
void AddVertsForAABB2D(std::vector<Vertex_PCU>& vertex_array, const AABB2& box, const Rgba& color,
                       const Vec2& uv_at_mins = Vec2(0.f, 1.f), const Vec2& uv_at_maxs = Vec2(1.f, 0.f));
void AddVertsForQuad2D(std::vector<Vertex_PCU>& vertex_array, const OBB2& box, const Rgba& color,
	const Vec2& uv_at_mins = Vec2(0.f, 1.f), const Vec2& uv_at_maxs = Vec2(1.f, 0.f));
void AddVertsForCapsule(std::vector<Vertex_PCU>& vertex_array, const Capsule2& wolrd_capsule, const Rgba& color, int num_sides = 32);

void TransformVertex(Vertex_PCU& vertexes, float uniform_scale, float rotation_degree_about_z,
                     const Vec2& translation_xy);
void TransformVertexArray(int num_vertexes, Vertex_PCU* vertexes, float uniform_scale_xy, float rotation_degree_about_z,
                          const Vec2& translation_xy);
void TransformVertexArray(std::vector<Vertex_PCU>& vertex_array, float uniform_scale_xy, float rotation_degree_about_z,
                          const Vec2& translation_xy);

Vec2 DetermineFlippedAxes(const Vec2& input_uv);