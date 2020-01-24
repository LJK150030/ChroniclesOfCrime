#include "Engine/Renderer/ColorTargetView.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/EngineCommon.hpp"

class Texture2D;

#define DX_SAFE_RELEASE(dx_resource)   if ((dx_resource) != nullptr) { dx_resource->Release(); dx_resource = nullptr; }

ColorTargetView::ColorTargetView(void* device)
{
	m_device = static_cast<ID3D11Device*>(device);
}

ColorTargetView::~ColorTargetView()
{
	// be sure to release this D3D11 resource
	DX_SAFE_RELEASE( m_rtv );
}

void ColorTargetView::CreateFromTexture(Texture2D* texture)
{
	UNUSED(texture);
	
	//TODO: need to implement this!
}

void ColorTargetView::CreateForInternalTexture(ID3D11Texture2D* tex)
{
	DX_SAFE_RELEASE( m_rtv ); // release the old one if present 

	// next, create the rtv for it; 
	HRESULT hr = m_device->CreateRenderTargetView(tex, // texture to create the view for
	                                              nullptr, // default options (use texture info)
	                                              &m_rtv); // out variable to store the view

	GUARANTEE_OR_DIE( SUCCEEDED(hr), "could not create " ); // make sure we could get it 
	// if this fails, check your output - DX will usually tell you why
	// (likely texture wasn't setup to be an output)

	// cache off size from D3D11 texture; 
	D3D11_TEXTURE2D_DESC desc;
	tex->GetDesc(&desc);
	m_size = Vec2(static_cast<float>(desc.Width), static_cast<float>(desc.Height));
}
