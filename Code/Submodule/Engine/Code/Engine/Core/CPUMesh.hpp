#pragma once
#include "Engine/Math/Vec3.hpp"
#include "Engine/Core/Vertex_Master.hpp"
#include "Engine/Math/Matrix44.hpp"
#include "Engine/Math/AABB2.hpp"
#include <vector>

struct Capsule2;
struct Capsule3;
struct AABB3;
struct OBB3;
struct OBB2;
class NamedStrings;

//------------------------------------------------------------------------

//------------------------------------------------------------------------
class CPUMesh              // A04
{
public:
	CPUMesh();           // A04
	~CPUMesh();

	void Clear();        // A04; 

	// Modify the stamp;
	void SetColor( const Rgba& color );           // A04
	void SetUV( const Vec2& uv );                 // A04
	void ConcatCPUMesh( const CPUMesh& sub_set );

	// Stamp a vertex into the list - return the index; 
	unsigned int AddVertex( const Vertex_Master& m );     // A04
	unsigned int AddVertex( const Vec3& pos );           // A04
	unsigned int AddVertex( const Vec3& pos, const Vec3& normal ); 
	unsigned int AddVertex( const Vec3& pos, const Vec3& normal, const Vec4& tangent ); 
	unsigned int AddVertex( const Vec3& pos, const Rgba& col, const Vec2& uv);           // A04
	unsigned int AddVertex( const Vec3& pos, const Rgba& col, const Vec2& uv, const Vec3& normal );           // A04
	unsigned int AddVertex( const Vec3& pos, const Rgba& col, const Vec2& uv, const Vec3& normal, const Vec4& tangent );           // A04

	 // Adds a single triangle; 
	void AddIndexedTriangle( unsigned int i0, unsigned int i1, unsigned int i2 );    // a04
	// adds two triangles (bl, tr, tl) and (bl, br, tr)
	void AddIndexedQuad( unsigned int topLeft, unsigned int topRight, unsigned int bottomLeft, unsigned int bottomRight );    // A04

	// Helpers
	unsigned int GetVertexCount() const;                 // A04
	unsigned int GetIndexCount() const;                  // A04
	void TransformVertex(float uniform_scale, float rotation_degree_about_z, const Vec2& translation_xy);
	void TransformVertex(const Matrix44& mat44, float scale_value);
	void TransformVertexCollection(float uniform_scale_xy, float rotation_degree_about_z, const Vec2& translation_xy);
	void TransformVertexCollection(const Matrix44& mat44, float scale_value);
	bool UsesIndexBuffer() const;
	unsigned int GetElementCount() const;

	NamedStrings ReadMeshXml( std::string file_dir, std::string xml_element );
	void PopulateFromMeshFile( const NamedStrings& obj_meta_data, bool invert_face, bool invert_uvs );

public: 
	std::vector<Vertex_Master>	m_vertices;       // A04
	std::vector<unsigned int>	m_indices;        // A04

	Vertex_Master m_stamp;                        // A04
};


//------------------------------------------------------------------------
// I prefer keeping my construction functions as C functions
// as I like to pick and choose the ones I want instead of having them
// all live in CPUMesh.hpp
//------------------------------------------------------------------------

// 2D
void CpuMeshAddQuad( CPUMesh *out, const AABB2& quad );
void CpuMeshAddQuad( CPUMesh *out, const OBB2& quad );
void CpuMeshAddQuad( CPUMesh *out, bool concat, const AABB2& quad );
void CpuMeshAddQuad( CPUMesh *out, bool concat, const Rgba& color, const AABB2& quad );
void CpuMeshAddQuad( CPUMesh *out, bool concat, const Rgba& color, const OBB2& quad );
void CpuMeshAddQuad( CPUMesh *out, bool concat, const Rgba& color, const AABB2& world_quad, const AABB2& wrapped_uv);
void CpuMeshAddDisc( CPUMesh *out, const Rgba& color, float radius, int num_sides = 32 );
void CpuMeshAddDisc( CPUMesh *out, const Rgba& color, const Vec2& center, float radius, int num_sides = 32 );
void CpuMeshAddDisc( CPUMesh *out,  bool concat, const Rgba& color, const Vec2& center, float radius, int num_sides = 32 );
void CpuMeshAddHalfDisc( CPUMesh *out, bool concat, const Vec2& center, float radius, const Rgba& color, float offset_rotation_degrees, int num_sides = 32);
void CpuMeshAddLine( CPUMesh *out, const Vec2& start, const Vec2& end,	float thickness, const Rgba& color);
void CpuMeshAddLine( CPUMesh *out, bool concat, const Vec2& start, const Vec2& end,	float thickness, const Rgba& color);
void CpuMeshAddRing2D( CPUMesh *out, const Vec2& center, float radius, float thickness,	const Rgba& color, int num_sides = 64);
void CpuMeshAddRing2D( CPUMesh *out, float radius, float thickness,	const Rgba& color, int num_sides = 64);
void CpuMeshAddTriangle( CPUMesh *out, float side_length, const Rgba& color );
void CpuMeshAddCapsule( CPUMesh *out, const Capsule2& world_capsule, const Rgba& color, int num_sides = 32);

// 3D
void CpuMeshAddCube( CPUMesh *out, const AABB3& box, const AABB2& wrapped_uv = AABB2::UNIT_BOX); // A04
void CpuMeshAddCube( CPUMesh *out, const OBB3& box, const AABB2& wrapped_uv = AABB2::UNIT_BOX); // A04
void CpuMeshAddUVSphere( CPUMesh *out, Vec3 center, float radius, unsigned int wedges = 32, unsigned int slices = 16 );  // A04
void CpuMeshAddUVSphere( CPUMesh *out, bool concat, Vec3 center, float radius, unsigned int wedges = 32, unsigned int slices = 16 );  // A04
//void CpuMeshAddCylinder( CPUMesh *out, const Capsule3& world_capsule,  unsigned int num_sides = 16);
void CpuMeshAddCapsule( CPUMesh *out, float radius, float length,  unsigned int num_sides = 16, unsigned num_slices = 32);
void CpuMeshAddCapsule( CPUMesh *out, const Capsule3& cap,  unsigned int num_sides = 16, unsigned num_slices = 32);
void CpuMeshAddCylinder( CPUMesh *out, float radius, float length,  unsigned int num_sides = 16);
void CpuMeshAddCylinder( CPUMesh *out, float bottom_radius, float top_radius, float length,  unsigned int num_sides = 16);
void CpuMeshAddCylinder( CPUMesh *out, bool concat, float bottom_radius, float top_radius, float length, float displacement_x,  unsigned int num_sides = 16);
void CpuMeshAddQuad( CPUMesh *out, const Vec3& bottom_left, const Vec3& bottom_right, const Vec3& top_left, const AABB2& wrapped_uv = AABB2::UNIT_BOX);