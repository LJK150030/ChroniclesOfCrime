#include "Engine/Renderer/texture.hpp"

Texture::Texture(RenderContext* ctx)
{
	m_owner = ctx;
}

Texture::~Texture()
{
	DX_SAFE_RELEASE(m_handle);
}
