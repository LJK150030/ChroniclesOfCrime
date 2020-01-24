#pragma once
#include "Engine/Math/IntVec2.hpp"
//#define INITGUID
#include <d3d11.h>
#pragma comment( lib, "d3d11.lib" )
#define DX_SAFE_RELEASE(dx_resource)   if ((dx_resource) != nullptr) { dx_resource->Release(); dx_resource = nullptr; }

class DepthStencilTargetView                             // A04
{
public: // external use
	DepthStencilTargetView();                          // A04
	~DepthStencilTargetView();                         // A04

	float GetWidth() const;                            // A04
	float GetHeight() const;                           // A04

public: // a renderable view of a texture // private
	ID3D11Resource* m_source         = nullptr;        // A04
	ID3D11DepthStencilView* m_rtv    = nullptr;        // A04
	IntVec2 m_size                      = IntVec2::ZERO;     // A04

}; 