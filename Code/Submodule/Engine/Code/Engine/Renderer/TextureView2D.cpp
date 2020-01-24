//------------------------------------------------------------------------
// TextureView2D.cpp
//------------------------------------------------------------------------

#include "Engine/Renderer/TextureView2D.hpp"
#include "Engine/Renderer/RenderContext.hpp"


TextureView::TextureView()
{
}

TextureView::~TextureView()
{
	DX_SAFE_RELEASE(m_handle);
	DX_SAFE_RELEASE(m_source);
}

TextureView2D::TextureView2D()
	: TextureView()
{
}

TextureView2D::~TextureView2D()
{
}
