#include "Engine/Renderer/Texture2D.hpp"
#include "Engine/Renderer/TextureView2D.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/Image.hpp"
#include "Engine/Renderer/DepthStencilTargetView.hpp"

//------------------------------------------------------------------------
// Texture2D.cpp
//------------------------------------------------------------------------

//------------------------------------------------------------------------
static unsigned int DXBindFromUsage(unsigned int usage)
{
	unsigned int binds = 0U;
	if (usage & TEXTURE_USAGE_TEXTURE_BIT)
	{
		binds |= D3D11_BIND_SHADER_RESOURCE;
	}
	if (usage & TEXTURE_USAGE_COLOR_TARGET_BIT)
	{
		binds |= D3D11_BIND_RENDER_TARGET;
	}
	if (usage & TEXTURE_USAGE_DEPTH_STENCIL_TARGET_BIT)
	{
		binds |= D3D11_BIND_DEPTH_STENCIL;
	}

	return binds;
}

Texture2D::Texture2D(RenderContext* ctx): Texture(ctx)
{
}

Texture2D::~Texture2D()
{
	DX_SAFE_RELEASE(m_handle);
}

//------------------------------------------------------------------------
bool Texture2D::LoadFromFile(std::string const& filename)
{
	// I am assuming you already have an Image class from SD1 (let me know if you do not!)
	const Image img(filename.c_str());
	return LoadTextureFromImage(img);
}

//------------------------------------------------------------------------
bool Texture2D::LoadTextureFromImage(Image const& image)
{
	// cleanup old resources before creating new one just in case; 
	DX_SAFE_RELEASE(m_handle);

	ID3D11Device* dd = m_owner->m_device;

	// If created from image, we'll assume it is only needed
	// as a read-only texture resource (if this is not true, change the
	// signature to take in the option)
	m_textureUsage = TEXTURE_USAGE_TEXTURE_BIT;

	// we are not picking static here because
	// we will eventually want to generate mipmaps,
	// which requires a GPU access pattern to generate.
	m_memoryUsage = GPU_MEMORY_USAGE_GPU;

	// Setup the Texture Description (what the resource will be like on the GPU)
	D3D11_TEXTURE2D_DESC texDesc;
	memset( &texDesc, 0, sizeof(D3D11_TEXTURE2D_DESC) );

	m_dimensions = image.GetDimensions();

	texDesc.Width = m_dimensions.x;
	texDesc.Height = m_dimensions.y;
	texDesc.MipLevels = 1; // setting to 0 means there's a full chain (or can generate a full chain)
	texDesc.ArraySize = 1; // only one texture
	texDesc.Usage = RenderContext::DXUsageFromMemoryUsage(m_memoryUsage); // loaded from image - probably not changing
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // if you support different image types  - this could change!  
	texDesc.BindFlags = DXBindFromUsage(m_textureUsage); // only allowing rendertarget for mipmap generation
	texDesc.CPUAccessFlags = 0U; // Determines how I can access this resource CPU side 
	texDesc.MiscFlags = 0U;

	// If Multisampling - set this up.
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;

	// Setup Initial Data
	// pitch is how many bytes is a single row of pixels;  
	unsigned int pitch = m_dimensions.x * 4;
	// 4 bytes for an R8G8B8A8 format;  Just sub in four if your image is always 4 bytes per channel
	D3D11_SUBRESOURCE_DATA data;
	memset( &data, 0, sizeof(D3D11_SUBRESOURCE_DATA) );
	data.pSysMem = image.GetBuffer();
	
	data.SysMemPitch = pitch;

	// Actually create it
	ID3D11Texture2D* tex2D = nullptr;
	HRESULT hr = dd->CreateTexture2D(&texDesc,
	                                 &data,
	                                 &tex2D);

	if (SUCCEEDED(hr))
	{
		// save off the info; 
		m_dimensions = image.GetDimensions();
		m_handle = tex2D;
		// TODO later assigment, generate mips if option is set; 
		return true;
	}
	ASSERT_OR_DIE(tex2D == nullptr, "text2D was not set to null"); // should be, just like to have the postcondition; 
	return false;
}

//------------------------------------------------------------------------
TextureView2D* Texture2D::CreateTextureView2D() const
{
	// if we don't have a handle, we can't create a view, so return nullptr
	if(m_handle == nullptr) return nullptr;

	// 2D - we will want to eventually create specific views of a texture
	// and will want ot fill out a D3D11_SHADER_RESOURCE_VIEW_DESC, but for now
	// we'll just do the default thing (nullptr)

	// get our device - since we're creating a resource
	ID3D11Device* dev = m_owner->m_device;
	ID3D11ShaderResourceView* srv = nullptr;
	dev->CreateShaderResourceView(m_handle, nullptr, &srv);

	if (srv != nullptr)
	{
		// Awesome, we have one
		TextureView2D* view = new TextureView2D();

		// give it the handle to the srv (we do not AddRef, 
		// but are instead just handing this off)
		view->m_handle = srv;

		// Also let the view hold onto a handle to this texture
		// (so both the texture AND the view are holding onto it)
		// (hence the AddRef)
		m_handle->AddRef();
		view->m_source = m_handle;

		// copy the size over for convenience
		view->m_dimensions = m_dimensions;

		// done - return!
		return view;
	}
	return nullptr;
}

bool Texture2D::CreateDepthStencilTarget(unsigned widht, unsigned height)
{
	// cleanup old resources before creating new one just in case; 
	DX_SAFE_RELEASE(m_handle);

	ID3D11Device *dd = m_owner->m_device; 

	// We want this to be bindable as a depth texture
	// AND a shader resource (for effects later);
	m_textureUsage = TEXTURE_USAGE_DEPTH_STENCIL_TARGET_BIT; 

	// we are not picking static here because
	// we will eventually want to generate mipmaps,
	// which requires a GPU access pattern to generate.
	m_memoryUsage = GPU_MEMORY_USAGE_GPU; 

	D3D11_TEXTURE2D_DESC texDesc;
	memset( &texDesc, 0, sizeof(D3D11_TEXTURE2D_DESC) );

	texDesc.Width = widht;
	texDesc.Height = height;
	texDesc.MipLevels = 1; // setting to 0 means there's a full chain (or can generate a full chain)
	texDesc.ArraySize = 1; // only one texture
	texDesc.Usage = RenderContext::DXUsageFromMemoryUsage(m_memoryUsage);  // loaded from image - probably not changing
	texDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;            // if you support different image types  - this could change!  
	texDesc.BindFlags = DXBindFromUsage(m_textureUsage);    // only allowing rendertarget for mipmap generation
	texDesc.CPUAccessFlags = 0U;                            // Determines how I can access this resource CPU side 
	texDesc.MiscFlags = 0U;  

	// If Multisampling - set this up.
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;

	// Actually create it
	ID3D11Texture2D *tex2D = nullptr; 
	HRESULT hr = dd->CreateTexture2D( &texDesc,
		nullptr, 
		&tex2D );

	if (SUCCEEDED(hr)) {
		// save off the info; 
		m_dimensions = IntVec2(widht, height);
		m_handle = tex2D;

		return true; 

	} else {
		ASSERT_RECOVERABLE( tex2D == nullptr, "error on CreateDepthStencilTarget" ); // should be, just like to have the postcondition; 
		return false; 
	}

}

DepthStencilTargetView* Texture2D::CreateDepthStencilTargetView()
{
	
	// get our device - since we're creating a resource
	ID3D11Device *dev = m_owner->m_device; 
	ID3D11DepthStencilView *dsv = nullptr; 

	D3D11_DEPTH_STENCIL_VIEW_DESC dsv_desc;
	memset( &dsv_desc, 0, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC) );
	dsv_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsv_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

	HRESULT hr = dev->CreateDepthStencilView( m_handle, &dsv_desc, &dsv );
	UNUSED(hr);
	if (dsv != nullptr) {
		

		// Awesome, we have one
		DepthStencilTargetView *view = new DepthStencilTargetView();

		// give it the handle to the srv (we do not AddRef, 
		// but are instead just handing this off)
		view->m_rtv = dsv; 

		// Also let the view hold onto a handle to this texture
		// (so both the texture AND the view are holding onto it)
		// (hence the AddRef)
		m_handle->AddRef(); 
		view->m_source = m_handle; 

		// copy the size over for convenience
		view->m_size = m_dimensions;

		// Release a handle reference
		DX_SAFE_RELEASE(m_handle);

		// done - return!
		return view; 

	} else {
		return nullptr; 
	}
}

bool Texture2D::CreateColorTarget(unsigned widht, unsigned height)
{
	// cleanup old resources before creating new one just in case; 
	DX_SAFE_RELEASE(m_handle);

	ID3D11Device *dd = m_owner->m_device; 

	// We want this to be bindable as a depth texture
	// AND a shader resource (for effects later);
	m_textureUsage = TEXTURE_USAGE_TEXTURE_BIT | TEXTURE_USAGE_COLOR_TARGET_BIT ; 

	// we are not picking static here because
	// we will eventually want to generate mipmaps,
	// which requires a GPU access pattern to generate.
	m_memoryUsage = GPU_MEMORY_USAGE_GPU; 

	D3D11_TEXTURE2D_DESC texDesc;
	memset( &texDesc, 0, sizeof(D3D11_TEXTURE2D_DESC) );

	texDesc.Width = widht;
	texDesc.Height = height;
	texDesc.MipLevels = 1; // setting to 0 means there's a full chain (or can generate a full chain)
	texDesc.ArraySize = 1; // only one texture
	texDesc.Usage = RenderContext::DXUsageFromMemoryUsage(m_memoryUsage);  // loaded from image - probably not changing
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;            // if you support different image types  - this could change!  
	texDesc.BindFlags = DXBindFromUsage(m_textureUsage);    // only allowing rendertarget for mipmap generation
	texDesc.CPUAccessFlags = 0U;                            // Determines how I can access this resource CPU side 
	texDesc.MiscFlags = 0U;  

	// If Multisampling - set this up.
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;

	// Actually create it
	ID3D11Texture2D *tex2D = nullptr; 
	HRESULT hr = dd->CreateTexture2D( &texDesc,
		nullptr, 
		&tex2D );

	if (SUCCEEDED(hr)) {
		// save off the info; 
		m_dimensions = IntVec2(widht, height);
		m_handle = tex2D;

		return true; 

	} else {
		ASSERT_RECOVERABLE( tex2D == nullptr, "error on CreateDepthStencilTarget" ); // should be, just like to have the postcondition; 
		return false; 
	}
}

ColorTargetView* Texture2D::CreateColorTargetView() const
{
	//TODO: need to implement this!
	return nullptr;
}

STATIC Texture2D* Texture2D::CreateDepthStencilTarget(RenderContext* ctx, unsigned width, unsigned height)
{
	Texture2D* temp_tex = new Texture2D(ctx);
	bool result = temp_tex->CreateDepthStencilTarget(width, height);

	if(result)
		return temp_tex;
	return nullptr;
}

STATIC Texture2D* Texture2D::CreateDepthStencilTargetFor(Texture2D* colorTarget)
{
	return CreateDepthStencilTarget( colorTarget->m_owner, colorTarget->m_dimensions.x, colorTarget->m_dimensions.y ); 
}

STATIC Texture2D* Texture2D::CreateColorTarget(RenderContext* ctx, unsigned width, unsigned height)
{
	//Texture2D* temp_tex = new Texture2D(ctx);
	//temp_tex->CreateColorTargetView();
	UNUSED(ctx);
	UNUSED(width);
	UNUSED(height);

	//TODO: need to implement this!
	return nullptr;
}

STATIC Texture2D* Texture2D::CreateMatchingColorTarget(Texture2D* other)
{
	//Texture2D* temp_tex = other;
	UNUSED(other);
	//TODO: need to implement this!
	return nullptr;
}

void Texture2D::MakeImage(Image& out_image)
{
	D3D11_MAPPED_SUBRESOURCE subresource;
	D3D11_MAP map_type = D3D11_MAP_READ;

	HRESULT hresult;
	hresult = m_owner->m_context->Map(m_handle, 0, map_type, 0, &subresource);

	size_t size = (m_dimensions.x * m_dimensions.y) * 4;
	memcpy(out_image.m_imageData, subresource.pData, size);

	m_owner->m_context->Unmap(m_handle, 0);
}
