//------------------------------------------------------------------------
// RenderBuffer.hpp
// Render Buffers are GPU storage for information.  
//------------------------------------------------------------------------6
#pragma once
#include "Engine/Renderer/RenderContext.hpp"

//#define INITGUID
#include <d3d11.h>
#pragma comment( lib, "d3d11.lib" )

#define BIT_FLAG(b) ((1U) << b)

//------------------------------------------------------------------------
// We have to specify what can a buffer be used 
// for.  It is possible for a buffer to serve multiple purposes
// though for our case, we'll be special casing them for clarity; 
enum eRenderBufferUsageBit : unsigned int
{
	RENDER_BUFFER_USAGE_VERTEX_STREAM_BIT = BIT_FLAG(0),
	// Can be bound to an vertex input stream slot
	RENDER_BUFFER_USAGE_INDEX_STREAM_BIT = BIT_FLAG(1),
	// Can be bound as an index input stream.  
	RENDER_BUFFER_USAGE_UNIFORMS_BIT = BIT_FLAG(2),
	// Can be bound to a constant buffer slot; 
};

typedef unsigned int eRenderBufferUsageBits;

//------------------------------------------------------------------------
// GPU memory allocation; 
class RenderBuffer
{
public:
	// for convenience - we'll have the RenderBuffer hold onto
	// the RenderContext used to create it so we're not constantly passing it in; 
	RenderBuffer(RenderContext* owner);
	virtual ~RenderBuffer();

	size_t GetSize() const; // return max byte size of this buffer; 
	bool IsStatic() const; // has static usage?
	bool IsDynamic() const;

protected:
	// for doing initial setup - we'll mark 
	// it as protected as the higher level classes
	// will help limit the number of options; 
	bool Create(void const* initialData,
	            size_t bufferSize,
	            size_t elementSize,
	            eRenderBufferUsageBits usage,
	            eGPUMemoryUsage memUsage);

	// Assumes a buffer is already created, and copies this data into it; 
	// Only valid for DYNAMIC buffers; 
	bool CopyCPUToGPU(void const* data, size_t byteSize);

public:
	RenderContext* m_owner = nullptr;
	eRenderBufferUsageBits m_bufferUsage;
	eGPUMemoryUsage m_memoryUsage;

	size_t m_bufferSize = 0; // total byte count of this buffer; 
	size_t m_elementSize; // certain buffers have the idea of an element stride, which we'll store here; 

	// D3D11 Resource
	ID3D11Buffer* m_handle = nullptr;
};
