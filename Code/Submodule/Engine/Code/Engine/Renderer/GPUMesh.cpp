#include "Engine/Renderer/GPUMesh.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Renderer/VertexBuffer.hpp"
#include "Engine/Renderer/IndexBuffer.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Core/CPUMesh.hpp"

GPUMesh::GPUMesh(RenderContext* ctx): m_elementCount(0), m_useIndexBuffer(false)
{
	m_vertexBuffer = new VertexBuffer(ctx);
	m_indexBuffer = new IndexBuffer(ctx);
}

GPUMesh::~GPUMesh()
{
	delete m_vertexBuffer;
	m_vertexBuffer = nullptr;

	delete m_indexBuffer;
	m_indexBuffer = nullptr;
}

void GPUMesh::CreateFromCPUData(void const* buffer, BufferAttributeT const layout[], CPUMesh const& mesh, unsigned int element_size)
{	
	m_vertexBuffer->CopyCPUToGPU(buffer, mesh.GetVertexCount(), layout, element_size); 
	
	if(mesh.UsesIndexBuffer())
		m_indexBuffer->CopyCPUToGPU( &mesh.m_indices[0], mesh.GetIndexCount() ); 

	SetDrawCall( mesh.UsesIndexBuffer(), mesh.GetElementCount() ); 
}

void GPUMesh::CopyVertexArray(Vertex_PCU const* vertices, unsigned count)
{
	UNUSED(vertices);
	UNUSED(count);
	//TODO: GPUMesh::CopyVertexArray(Vertex_PCU const* vertices, unsigned count)
}

void GPUMesh::CopyIndices(unsigned const* indices, unsigned count)
{
	UNUSED(indices);
	UNUSED(count);
	//TODO: GPUMesh::CopyIndices(unsigned const* indices, unsigned count)
}

void GPUMesh::SetDrawCall(bool use_index_buffer, unsigned elem_count)
{
	if (use_index_buffer) {
		ASSERT_OR_DIE( m_indexBuffer->GetSize() >= elem_count, "Number of elements are larger than the index buffer" ); 
	} else {
		ASSERT_OR_DIE( m_vertexBuffer->GetSize() >= elem_count, "Number of elements are larger than the vertex buffer"); 
	}

	m_elementCount = elem_count; 
	m_useIndexBuffer = use_index_buffer; 
}

