#include "Engine/Renderer/IndexBuffer.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

IndexBuffer::IndexBuffer(RenderContext* ctx): RenderBuffer(ctx), m_indexCount(0)
{
}

IndexBuffer::~IndexBuffer()
{
}

bool IndexBuffer::CreateStaticFor(unsigned const* indices, unsigned const count)
{
	// just always create for static a new static buffer; 
	bool result = Create(indices,
		count, // total size needed for buffer?
		sizeof(IndexBuffer), // stride - size from one vertex to another
		RENDER_BUFFER_USAGE_INDEX_STREAM_BIT,
		GPU_MEMORY_USAGE_STATIC); // probably want dynamic if we're using copy

	if (result)
	{
		m_indexCount = count;
		return true;
	}
	m_indexCount = 0U;
	return false;

}

bool IndexBuffer::CopyCPUToGPU(unsigned const* indices, unsigned const count)
{
	// how many bytes do we need
	size_t sizeNeeded = count * sizeof(unsigned int);

	// if we don't have enough room, or this is a static
	// buffer, recreate (Create should release the old buffer)
	if (sizeNeeded > GetSize() || IsStatic())
	{
		bool result = Create(indices,
			sizeNeeded, // total size needed for buffer?
			sizeof(unsigned int), // stride - size from one vertex to another
			RENDER_BUFFER_USAGE_INDEX_STREAM_BIT,
			GPU_MEMORY_USAGE_DYNAMIC); // probably want dynamic if we're using copy

		if (result)
		{
			m_indexCount = count;
		}
		else
		{
			m_indexCount = 0U;
		}

		return result;
	}
	// non-static and we have enough room
	ASSERT_RECOVERABLE( IsDynamic(), "IndexBuffer is non-static and we have enough room");
	if (RenderBuffer::CopyCPUToGPU(indices, sizeNeeded))
	{
		m_indexCount = count;
		return true;
	}

	return false;
}
