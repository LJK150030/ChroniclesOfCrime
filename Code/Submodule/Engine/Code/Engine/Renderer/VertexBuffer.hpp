//------------------------------------------------------------------------
// VertexBuffer.hpp
//------------------------------------------------------------------------
#pragma once
#include "RenderBuffer.hpp"

struct Vertex_PCU;
struct BufferAttributeT;

// Convenience class - makes working with render buffers meant for
// vertex buffers a little easier; 
class VertexBuffer : public RenderBuffer
{
public:
	VertexBuffer(RenderContext* owner): RenderBuffer(owner)	{ m_bufferSize = 0U; }
	virtual ~VertexBuffer();

	bool CreateStaticFor(Vertex_PCU const* vertices, unsigned int count);
	bool CopyCPUToGPU(Vertex_PCU const* vertices, unsigned int count);

	bool CopyCPUToGPU(void const* buffer, unsigned int buf_size, BufferAttributeT const layout[], unsigned int element_size);

public:
	unsigned int m_vertexCount = 0; // how many vertices are stored currently; 
	unsigned int m_elementStride = 0;
	BufferAttributeT const* m_layout;

	//buffer_attirbute_t const *m_layout; 
	// BufferLayout const *m_layout;   // TODO: VertexBuffer knows what kind of vertices are in it; 
};
