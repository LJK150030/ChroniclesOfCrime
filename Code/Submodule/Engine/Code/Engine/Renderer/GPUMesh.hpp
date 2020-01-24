#pragma once
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Core/Vertex_Lit.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Core/CPUMesh.hpp"

struct BufferAttributeT;
class VertexBuffer;
class IndexBuffer;

class GPUMesh                            // A04
{
public:
	explicit GPUMesh( RenderContext* ctx );     // A04
	~GPUMesh();

	// these actually mostly do the same thing - just differnt default
	// memory usage
/*	void CreateFromCPUMesh( CPUMesh const* mesh, eGPUMemoryUsage mem); // A04*/
	void CreateFromCPUData( void const* buffer, BufferAttributeT const layout[], CPUMesh const& mesh, unsigned int element_size );
	//void CopyFromCPUMesh( CPUMesh const *mesh, eGPUMemoryUsage mem = GPU_MEMORY_USAGE_DYNAMIC );  // A04

	void CopyVertexArray( Vertex_PCU const *vertices, unsigned int count );                           // A04 Optional; 
	void CopyIndices( unsigned int  const *indices, unsigned int  count );                                          // A04 Optional;

	void SetDrawCall( bool use_index_buffer, unsigned int  elem_count );

	inline std::string const& GetDefaultMaterialName() const    { return m_defaultMaterial; }

	template <typename VERTEX_TYPE>
	bool CreateFromCPUMesh( CPUMesh const& mesh )
	{
		// size of the vertex is the last element plus the size of that element
		size_t dataTypeSize = sizeof(VERTEX_TYPE);
		size_t gpu_buffer_size = dataTypeSize * mesh.GetVertexCount();
		//size_t gpu_buffer_size = ComputeGPUBuffer( VERTEX_TYPE::LAYOUT, mesh.GetVertexCount() ); 
		void* buffer = malloc( gpu_buffer_size ); 

		VERTEX_TYPE::CopyFromMaster( buffer, mesh.m_vertices );
		CreateFromCPUData( buffer, VERTEX_TYPE::LAYOUT, mesh, sizeof(VERTEX_TYPE) ); 

		// Don't Forget to Free
		free( buffer );
		return true;
	}

public: 
	VertexBuffer* m_vertexBuffer; 
	IndexBuffer* m_indexBuffer; 

	// information for drawing; 
	unsigned int  m_elementCount; 
	bool m_useIndexBuffer;

	std::string m_defaultMaterial = "test.mat";
};
