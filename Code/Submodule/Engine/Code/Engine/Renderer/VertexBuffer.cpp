//------------------------------------------------------------------------
// VertexBuffer.cpp
//------------------------------------------------------------------------
#include "Engine/Renderer/VertexBuffer.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Core/Vertex_Lit.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

VertexBuffer::~VertexBuffer()
{
}

//------------------------------------------------------------------------
bool VertexBuffer::CreateStaticFor(Vertex_PCU const* vertices, unsigned int const count)
{
	// just always create for static a new static buffer; 
	bool result = Create(vertices,
	                     count, // total size needed for buffer?
	                     sizeof(Vertex_PCU), // stride - size from one vertex to another
	                     RENDER_BUFFER_USAGE_VERTEX_STREAM_BIT,
	                     GPU_MEMORY_USAGE_STATIC); // probably want dynamic if we're using copy

	if (result)
	{
		m_vertexCount = count;
		return true;
	}
	m_vertexCount = 0U;
	return false;
}

//------------------------------------------------------------------------
// Similar to UBO - since we are assuming if they are using this method 
// it is dynamic, and we only need to remake if the size changes; 
bool VertexBuffer::CopyCPUToGPU(Vertex_PCU const* vertices, unsigned int const count)
{
	// how many bytes do we need
	size_t sizeNeeded = count * sizeof(Vertex_PCU);

	// if we don't have enough room, or this is a static
	// buffer, recreate (Create should release the old buffer)
	if (sizeNeeded > GetSize() || IsStatic())
	{
		bool result = Create(vertices,
			sizeNeeded, // total size needed for buffer?
			sizeNeeded, // 1U or byteSize is fine - not used for a UniformBuffer
			RENDER_BUFFER_USAGE_VERTEX_STREAM_BIT,
			GPU_MEMORY_USAGE_DYNAMIC); // probably want dynamic if we're using copy

		return result;
	}

	// non-static and we have enough room, so call down 
	// to our base classes CopyGPUToGPU that assumes
	// enough room is already allocated; 
	ASSERT_RECOVERABLE( IsDynamic(), "Changing from static to dynamic" );
	return RenderBuffer::CopyCPUToGPU(vertices, sizeNeeded);
}

bool VertexBuffer::CopyCPUToGPU(void const* buffer, unsigned int count, BufferAttributeT const layout[], unsigned int element_size)
{
	m_layout = layout;

// 	int length = 0;
// 	while(!layout[length].IsNull())
// 		length++;

	unsigned int sizeNeeded = count * element_size;
	m_elementStride = element_size;

	// if we don't have enough room, or this is a static
	// buffer, recreate (Create should release the old buffer)
	if (sizeNeeded >= GetSize() || IsStatic())
	{
		bool result = Create(buffer,
			sizeNeeded, // total size needed for buffer?
			element_size, // 1U or byteSize is fine - not used for a UniformBuffer
			RENDER_BUFFER_USAGE_VERTEX_STREAM_BIT,
			GPU_MEMORY_USAGE_DYNAMIC); // probably want dynamic if we're using copy

		return result;
	}

	// non-static and we have enough room, so call down 
	// to our base classes CopyGPUToGPU that assumes
	// enough room is already allocated; 
 	ASSERT_OR_DIE( IsDynamic(), "Changing from static to dynamic" );
	return RenderBuffer::CopyCPUToGPU(buffer, sizeNeeded);
}
