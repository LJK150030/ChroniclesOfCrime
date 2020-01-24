//------------------------------------------------------------------------
// RenderBuffer.cpp
//------------------------------------------------------------------------

#include "Engine/Renderer/RenderBuffer.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"

#define DX_SAFE_RELEASE(dx_resource)   if ((dx_resource) != nullptr) { dx_resource->Release(); dx_resource = nullptr; }

//------------------------------------------------------------------------
// Convert a buffer usage bitfield to a DX specific 
// version; 
static unsigned int DXBufferUsageFromBufferUsage(eRenderBufferUsageBits const bind_flags)
{
	unsigned int ret = 0U;

	if (bind_flags & RENDER_BUFFER_USAGE_VERTEX_STREAM_BIT)
	{
		ret |= D3D11_BIND_VERTEX_BUFFER;
	}

	if (bind_flags & RENDER_BUFFER_USAGE_INDEX_STREAM_BIT)
	{
		ret |= D3D11_BIND_INDEX_BUFFER;
	}

	if (bind_flags & RENDER_BUFFER_USAGE_UNIFORMS_BIT)
	{
		ret |= D3D11_BIND_CONSTANT_BUFFER;
	}

	return ret;
}

RenderBuffer::RenderBuffer(RenderContext* owner): m_owner(owner)
{

}

RenderBuffer::~RenderBuffer()
{
	DX_SAFE_RELEASE(m_handle);
}

size_t RenderBuffer::GetSize() const
{
	return m_bufferSize;
}

bool RenderBuffer::IsStatic() const
{
	return m_memoryUsage == GPU_MEMORY_USAGE_STATIC;
}

bool RenderBuffer::IsDynamic() const
{
	return m_memoryUsage == GPU_MEMORY_USAGE_DYNAMIC;
}

//------------------------------------------------------------------------
// Creates a buffer; 
bool RenderBuffer::Create(void const* initialData,
                          size_t bufferSize,
                          size_t elementSize,
                          eRenderBufferUsageBits bufferUsage,
                          eGPUMemoryUsage memUsage)
{
	// Free the old handle
	DX_SAFE_RELEASE(m_handle);

	// can not create a 0 sized buffer; 
	ASSERT_RECOVERABLE((bufferSize > 0U) && (elementSize > 0U), "can not create a 0 sized buffer")
	if ((bufferSize == 0U) && (elementSize == 0U)) return false;

	// static buffers MUST be supplied data.
	ASSERT_RECOVERABLE( (memUsage != GPU_MEMORY_USAGE_STATIC) || (initialData != nullptr),
		"static buffers MUST be supplied data." );
	if ((memUsage == GPU_MEMORY_USAGE_STATIC) && (initialData == nullptr)) return false;

	// Setup the buffer
	// When creating a D3D object, we setup
	// a DESC object to describe it...
	D3D11_BUFFER_DESC bd;
	memset(&bd, 0, sizeof(D3D11_BUFFER_DESC));

	bd.ByteWidth = static_cast<unsigned int>(bufferSize);
	bd.StructureByteStride = static_cast<unsigned int>(elementSize);
	bd.Usage = RenderContext::DXUsageFromMemoryUsage(memUsage);
	bd.BindFlags = DXBufferUsageFromBufferUsage(bufferUsage);

	// give us write access to dynamic buffers
	// and read/write access to staging buffers; 
	bd.CPUAccessFlags = 0U;
	if (memUsage == GPU_MEMORY_USAGE_DYNAMIC)
	{
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}
	else if (memUsage == GPU_MEMORY_USAGE_STAGING)
	{
		bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
	}

	// TODO: support structured buffers (Summer Semester)
	//       I will probably forget about this....

	// Map initial data if relevant (something was passed in)
	D3D11_SUBRESOURCE_DATA data;
	D3D11_SUBRESOURCE_DATA* data_ptr = nullptr;
	if (initialData != nullptr)
	{
		memset(&data, 0, sizeof(D3D11_SUBRESOURCE_DATA));
		data.pSysMem = initialData;
		data_ptr = &data;
	}

	// Create it - devices create resources; 
	ID3D11Device* dev = m_owner->m_device;
 	HRESULT hr = dev->CreateBuffer(&bd, data_ptr, &m_handle);

	if (SUCCEEDED(hr))
	{
// 		static unsigned int buffer_count = 0;
// 		std::string debug_name = Stringf("%u", buffer_count);
// 		++buffer_count;
//		m_handle->SetPrivateData( WKPDID_D3DDebugObjectName, (unsigned int) debug_name.size() + 1, debug_name.c_str());
		// save off options; 
		m_bufferUsage = bufferUsage;
		m_memoryUsage = memUsage;
		m_bufferSize = bufferSize;
		m_elementSize = elementSize;
		return true;
	}
	return false;
}

//------------------------------------------------------------------------
bool RenderBuffer::CopyCPUToGPU(void const* data, size_t const byteSize)
{
	// staging or dynamic only & we better have room; 
	ASSERT_OR_DIE( !IsStatic(), "staging or dynamic only!" );
	ASSERT_OR_DIE( byteSize <= m_bufferSize, "Need to have room for data" );

	// Map and copy
	// This is a command, so runs using the context
	ID3D11DeviceContext* ctx = m_owner->m_context;

	// Map (ie, lock and get a writable pointer)
	// Be sure to ONLY write to what you locked
	D3D11_MAPPED_SUBRESOURCE resource;
	HRESULT hr = ctx->Map(m_handle,
	                      0, // resource index (for arrays/mip layers/etc...)
	                      D3D11_MAP_WRITE_DISCARD,
	                      // says to discard (don't care) about the memory the was already there
	                      0U, // option to allow this to fail if the resource is in use, 0U means we'll wait...
	                      &resource);

	if (SUCCEEDED(hr))
	{
		// we're mapped!  Copy over
		memcpy(resource.pData, data, byteSize);

		// unlock the resource (we're done writing)
		ctx->Unmap(m_handle, 0);

		return true;
	}
	return false;
}
