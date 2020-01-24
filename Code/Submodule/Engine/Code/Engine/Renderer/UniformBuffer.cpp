//------------------------------------------------------------------------
// UniformBuffer.cpp
//------------------------------------------------------------------------
#include "Engine/Renderer/UniformBuffer.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

UniformBuffer::UniformBuffer(RenderContext* owner): RenderBuffer(owner)
{
}

UniformBuffer::~UniformBuffer()
{
}

//------------------------------------------------------------------------
bool UniformBuffer::CopyCPUToGPU(void const* data, size_t const byteSize)
{
	// if we don't have enough room, or this is a static
	// buffer, recreate (Create should release the old buffer)
	if (byteSize > GetSize() || IsStatic())
	{
		bool result = Create(data,
		                     byteSize, // total size needed for buffer?
		                     byteSize, // 1U or byteSize is fine - not used for a UniformBuffer
		                     RENDER_BUFFER_USAGE_UNIFORMS_BIT,
		                     GPU_MEMORY_USAGE_DYNAMIC); // probably want dynamic if we're using copy

		return result;
	}
	// non-static and we have enough room, so call down 
	// to our base classes CopyGPUToGPU that assumes
	// enough room is already allocated; 
	ASSERT_RECOVERABLE( IsDynamic(), "Changing from static to dynamic" );
	return RenderBuffer::CopyCPUToGPU(data, byteSize);
}
