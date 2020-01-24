#pragma once
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Renderer/RenderBuffer.hpp"

// Textures are just another form of a buffer, so enums used
// to describe buffers also apply to textures; 

// Eventually, when we add new Texture types, these common 
// enums & classes should be moved to Texture.hpp
enum eTextureUsageBit : unsigned int
{
	TEXTURE_USAGE_TEXTURE_BIT = BIT_FLAG(0),
	// Can be used to create a TextureView
	TEXTURE_USAGE_COLOR_TARGET_BIT = BIT_FLAG(1),
	// Can be used to create a ColorTargetView
	TEXTURE_USAGE_DEPTH_STENCIL_TARGET_BIT = BIT_FLAG(2),
	// Can be used to create a DepthStencilTargetView
};

typedef unsigned int eTextureUsageBits;

//------------------------------------------------------------------------
class Texture
{
public:
	Texture(RenderContext* ctx);
	virtual ~Texture(); // virtual, as this will release the resource; 

public:
	RenderContext* m_owner = nullptr;

	// D3D11 objets; 
	ID3D11Resource* m_handle = nullptr;
	eGPUMemoryUsage m_memoryUsage;
	eTextureUsageBits m_textureUsage;
	IntVec2 m_dimensions;

	// If you have the concept of an image format (RGBA8, R8, D24S8, etc..)
	// It would go here.  If you only have support for RGBA8, we'll just assume it; 
	// eImageFormat m_format; 
};
