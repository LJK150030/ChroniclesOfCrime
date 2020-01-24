#pragma once
#include "Engine/Renderer/RenderBuffer.hpp"

class IndexBuffer : public RenderBuffer        // A04
{
public:
	IndexBuffer( RenderContext *ctx );       // A04
	virtual ~IndexBuffer();
											 // Technically you can create 16-bit or 32-bit index buffers
											 // but for simplicity we'll stick to just 32-bit; 
	bool CreateStaticFor( unsigned int const *indices, unsigned int const count );          // A04
	bool CopyCPUToGPU( unsigned int const *indices, unsigned int const count );            // A04

public: 
	unsigned int m_indexCount;                      // A04
};

