#include "Engine/Renderer/DepthStencilTargetView.hpp"

DepthStencilTargetView::DepthStencilTargetView()
{
}

DepthStencilTargetView::~DepthStencilTargetView()
{
	DX_SAFE_RELEASE(m_source);
	DX_SAFE_RELEASE(m_rtv);
}

float DepthStencilTargetView::GetWidth() const
{
	return 0.0f;
}

float DepthStencilTargetView::GetHeight() const
{
	return 0.0f;
}
