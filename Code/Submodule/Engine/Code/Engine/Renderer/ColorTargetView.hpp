#pragma once

//#define INITGUID
#include <d3d11.h>
#pragma comment( lib, "d3d11.lib" )

#include "Engine/Math/Vec2.hpp"

class Texture2D;

// (AKA Render Target View), is a writable view of a texture
class ColorTargetView
{
	friend class RenderContext;

public:
	ID3D11RenderTargetView* m_rtv = nullptr;
	Vec2 m_size = Vec2::ZERO;

public:
	ColorTargetView(void* device);
	~ColorTargetView();

	void CreateFromTexture( Texture2D *texture );

public: // a renderable view of a texture 
	ID3D11Device* m_device = nullptr;
	float GetWidth() const { return m_size.x; }
	float GetHeight() const { return m_size.y; }

private:
	void CreateForInternalTexture(ID3D11Texture2D* tex);
};
