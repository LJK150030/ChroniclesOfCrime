#include "Engine/EngineCommon.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/ColorTargetView.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/UniformBuffer.hpp"
#include "Engine/Renderer/VertexBuffer.hpp"
#include "Engine/Renderer/IndexBuffer.hpp"
#include "Engine/Renderer/Texture2D.hpp"
#include "Engine/Renderer/TextureView2D.hpp"
#include "Engine/Renderer/GPUMesh.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Renderer/Model.hpp"
#include "Engine/Core/WindowContext.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/FileIOUtils.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Core/Image.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Renderer/Sampler.hpp"
#include "Engine/Async/Request.hpp"
#include "Engine/Async/Dispatcher.hpp"


#include "ThirdParty/stb/stb_image.h"

#define WIN32_LEAN_AND_MEAN		// Always #define this before #including <windows.h>
#include <Windows.h>			// #include this (massive, platform-specific) header in very few places

// Required Headers
//#define INITGUID
#include <d3d11.h>
#include <DXGI.h>

#pragma comment( lib, "d3d11.lib" )
#pragma comment( lib, "DXGI.lib" )

// DEBUG STUFF
#include <dxgidebug.h>
// #pragma comment( lib, "dxguid.lib" )

// NEEDED FOR COMPILING
// Note:  This is not allowed for Windows Store Apps.
// Shaders must be compiled to byte-code offline. 
// but for development - great if the program knows how to compile it.
#include <d3dcompiler.h>
#pragma comment( lib, "d3dcompiler.lib" )

#define RENDER_DEBUG

typedef unsigned int uint;


//Temporary for lights
STATIC bool RenderContext::SetDirectionalColor(EventArgs& args)
{
	LightT directional = g_theRenderer->m_lightContainer.m_lights[0];
	directional.m_color = args.GetValue(std::string("color"), Vec3(0.0f, 0.0f, 0.0f));
	g_theRenderer->UpdateLightAtIndex(0, directional);
	return false;
}

STATIC bool RenderContext::SetDirectionalDirection(EventArgs& args)
{
	LightT directional = g_theRenderer->m_lightContainer.m_lights[0];
	directional.m_direction = args.GetValue(std::string("color"), Vec3(0.0f, 0.0f, 0.0f));
	g_theRenderer->UpdateLightAtIndex(0, directional);
	return false;

}

bool RenderContext::IsMeshLoaded(std::string const& file_name)
{
	const std::map<std::string, GPUMesh*>::iterator mesh_iterator = m_meshDatabase.find(file_name);
	return mesh_iterator != m_meshDatabase.end();
}

CPUMesh* RenderContext::CreateCPUMeshFromFile( std::string const& file_name )
{
	const std::string concat_filename = "Data/Models/" + file_name;
	CPUMesh* temp_mesh = new CPUMesh();
	NamedStrings obj_meta_data = temp_mesh->ReadMeshXml(concat_filename, "model");
	bool inverse_face = obj_meta_data.GetValue("invert_faces", false);
	bool inverse_uvs = obj_meta_data.GetValue("invert_UVs", false);
	float scale_model = obj_meta_data.GetValue("scale", 1.0f);
	std::string transform_model = obj_meta_data.GetValue("transform", "x y z");
	temp_mesh->PopulateFromMeshFile(obj_meta_data, inverse_face, inverse_uvs);
	Matrix44 transformMatrix = Matrix44::MakeTransformMat44(transform_model);
	temp_mesh->TransformVertexCollection(transformMatrix, scale_model);
	return temp_mesh;
}

GPUMesh* RenderContext::CreateGPUMeshFromFile( CPUMesh& mesh, std::string const& file_name )
{
	const std::string concat_filename = "Data/Models/" + file_name;
	GPUMesh* temp_gpu = new GPUMesh(this);
	NamedStrings material_meta_data = mesh.ReadMeshXml(concat_filename, "material");
	temp_gpu->m_defaultMaterial = material_meta_data.GetValue("src", temp_gpu->m_defaultMaterial);
	temp_gpu->m_useIndexBuffer = false;
	temp_gpu->CreateFromCPUMesh<Vertex_Lit>(mesh);

	m_meshDatabase[file_name] = temp_gpu;
	return temp_gpu;
}

bool RenderContext::IsImageLoaded(const std::string& file_name)
{
	const std::map<std::string, TextureView2D*>::iterator image_iterator = m_loadedTextureViews.find(file_name);
	return image_iterator != m_loadedTextureViews.end();
}

TextureView2D* RenderContext::CreateTextureView2DFromImage(Image* image, const std::string& file_name)
{
	Texture2D* tex = new Texture2D(this);
	tex->LoadTextureFromImage(*image);
	TextureView2D* view = tex->CreateTextureView2D();
	m_loadedTextureViews[file_name] = view;

	// THIS SHOULD BE FINE - the view should hold onto the D3D11 resource;
	// (I'm not 100% on this though, so if not, please Slack me)
	delete tex;
	tex = nullptr;
	return view;
}

ID3D11Device* RenderContext::GetDx11Device()
{
	return m_device;
}

ID3D11DeviceContext* RenderContext::GetDx11Context()
{
	return m_context;
}

bool RenderContext::LoadTextureFromFile(const char* filename, ID3D11ShaderResourceView** out_srv, int* out_width,
	int* out_height)
{
	// Load from disk into a raw RGBA buffer
	int image_width = 0;
	int image_height = 0;
	unsigned char* image_data = stbi_load(filename, &image_width, &image_height, NULL, 4);
	if (image_data == NULL)
		return false;

	// Create texture
	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.Width = image_width;
	desc.Height = image_height;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;

	ID3D11Texture2D *pTexture = NULL;
	D3D11_SUBRESOURCE_DATA subResource;
	subResource.pSysMem = image_data;
	subResource.SysMemPitch = desc.Width * 4;
	subResource.SysMemSlicePitch = 0;
	m_device->CreateTexture2D(&desc, &subResource, &pTexture);

	// Create texture view
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = desc.MipLevels;
	srvDesc.Texture2D.MostDetailedMip = 0;
	m_device->CreateShaderResourceView(pTexture, &srvDesc, out_srv);
	pTexture->Release();

	*out_width = image_width;
	*out_height = image_height;
	stbi_image_free(image_data);

	return true;
}

//////


void RenderContext::CacheSamplers()
{
	Sampler* point = new Sampler();
	point->SetFilterModes(FILTER_MODE_LINEAR, FILTER_MODE_POINT);
	// still min linear - to get blending as it gets smaller; 
	m_cachedSamplers[SAMPLE_MODE_POINT] = point;

	Sampler* linear = new Sampler();
	linear->SetFilterModes(FILTER_MODE_LINEAR, FILTER_MODE_LINEAR);
	m_cachedSamplers[FILTER_MODE_LINEAR] = linear;
}

void RenderContext::PrintDebug()
{
	//if( m_device == nullptr ) {
	//	return;
	//}
	ID3D11Debug* debug = nullptr;
	const HRESULT result = m_device->QueryInterface(__uuidof(ID3D11Debug), (void**)&debug);
	if (SUCCEEDED(result))
	{
		debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
	}
	DX_SAFE_RELEASE( debug );
}

LightT::LightT()
{
	m_color = Vec3(Rgba::WHITE); 
	m_intensity = 0.0f; 
	m_position = Vec3::ZERO; 
	m_isDirectional = 1;
	m_direction =  Vec3::ONE;
	m_diffuseAttenuation = Vec3(1.0f, 0.0f, 0.0f);
	m_specularAttenuation = Vec3(0.0f, 0.0f, 1.0f);;
	
	pad10 = 0;
	pad20 = 0;
	pad30 = 0; 
}

LightT::LightT(const Rgba& color, float intensity, const Vec3& position, float is_directional, const Vec3& direction,
	const Vec3& diffuse_attenuation = Vec3(1.0f, 0.0f, 0.0f), const Vec3& specular_attenuation = Vec3(0.0f, 0.0f, 1.0f))
{
	m_color = Vec3(color); 
	m_intensity = intensity; 
	m_position = position; 
	m_isDirectional = is_directional;
	m_direction = direction;
	m_diffuseAttenuation = diffuse_attenuation;
	m_specularAttenuation = specular_attenuation;

	pad10 = 0;
	pad20 = 0;
	pad30 = 0; 
}

RenderContext::RenderContext(WindowContext* window_context)
{
	D3D11Setup(window_context);
}

RenderContext::~RenderContext()
{

}

void RenderContext::Startup()
{
	SetupDefaultRasterState();

	g_theEventSystem->SubscribeEventCallbackFunction("color", SetDirectionalColor);
	g_theEventSystem->SubscribeEventCallbackFunction("direction", SetDirectionalDirection);

	m_context->RSSetState(m_defaultRasterState);
	
	CacheSamplers();

	//loading default shaders
	/*m_currentShader = CreateOrGetShader("default_unlit.hlsl");*/
	
	//loading default images
	Image image;
	Texture2D* white = new Texture2D(this);
	white->LoadTextureFromImage(image);
	m_loadedTextureViews["white"] = white->CreateTextureView2D();
	delete white;
	white = nullptr;
	
	image.SetTexelColor(IntVec2::ZERO, Rgba::GRAY);
	Texture2D* gray = new Texture2D(this);
	gray->LoadTextureFromImage(image);
	m_loadedTextureViews["gray"] = gray->CreateTextureView2D();
	delete gray;
	gray = nullptr;
	
	image.SetTexelColor(IntVec2::ZERO, Rgba::BLACK);
	Texture2D* black = new Texture2D(this);
	black->LoadTextureFromImage(image);
	m_loadedTextureViews["black"] = black->CreateTextureView2D();
	delete black;
	black = nullptr;
	
	image.SetTexelColor(IntVec2::ZERO, Rgba::FLAT);
	Texture2D* flat = new Texture2D(this);
	flat->LoadTextureFromImage(image);
	m_loadedTextureViews["flat"] = flat->CreateTextureView2D();
	delete flat;
	flat = nullptr;

	image.SetTexelColor(IntVec2::ZERO, Rgba::BLUE);
	Texture2D* blue = new Texture2D(this);
	blue->LoadTextureFromImage(image);
	m_loadedTextureViews["blue"] = blue->CreateTextureView2D();
	delete blue;
	blue = nullptr;

	ModelBufferT modle_buffer; 
	modle_buffer.m_model = Matrix44::IDENTITY; 
	m_modelBuffer = new UniformBuffer( this ); 
	m_modelBuffer->CopyCPUToGPU( &modle_buffer, sizeof(modle_buffer) );

// 	LightBufferT light_buffer;
// 	light_buffer.m_ambient = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
// 	light_buffer.m_specFactor = 1.0f;
// 	light_buffer.m_specPower = 1.0f;

	m_lightBuffer = new UniformBuffer( this ); 
	m_lightBuffer->CopyCPUToGPU( &m_lightContainer, sizeof(m_lightContainer) );

	m_immediateVBO = new VertexBuffer(this);


	m_postProcessingCamera = new Camera();
	m_effectBuffer = new UniformBuffer(this);
	m_effectBuffer->CopyCPUToGPU( &m_effectContainer, sizeof(m_effectContainer) );
}

void RenderContext::BeginFrame()
{
	// first, acquire the backbuffer (this adds one reference to the backbuffer)
	ID3D11Texture2D* back_buffer = nullptr;
	m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&back_buffer);

	// Okay, we have an rtv, store it in our own class
	m_frameBackbufferCtv = new ColorTargetView(m_device);

	// storing back buffer onto a color target view 
	m_frameBackbufferCtv->CreateForInternalTexture(back_buffer);

	if(m_defaultDepthStencilView == nullptr)
	{
		m_depthTexture = Texture2D::CreateDepthStencilTarget(this, static_cast<unsigned int>(m_frameBackbufferCtv->GetWidth()), static_cast<unsigned int>(m_frameBackbufferCtv->GetHeight()));
		m_defaultDepthStencilView = m_depthTexture->CreateDepthStencilTargetView();
	}
//	BindUniformBuffer( 3, m_modelBuffer);

	//our own color target view
	m_cpuColorTexture2D = new Texture2D(this);
	m_cpuColorTexture2D->CreateColorTarget(static_cast<unsigned int>(m_frameBackbufferCtv->GetWidth()), static_cast<unsigned int>(m_frameBackbufferCtv->GetHeight()));
	m_cpuColorTargetView = new ColorTargetView(m_device);
	m_cpuColorTargetView->CreateForInternalTexture(static_cast<ID3D11Texture2D*>(m_cpuColorTexture2D->m_handle));

	// effect color target view
	m_postProcessingColorTarget2D = new Texture2D(this);
	m_postProcessingColorTarget2D->CreateColorTarget(static_cast<unsigned int>(m_frameBackbufferCtv->GetWidth()), static_cast<unsigned int>(m_frameBackbufferCtv->GetHeight()));
	m_postProcessingColorTargetView = new ColorTargetView(m_device);
	m_postProcessingColorTargetView->CreateForInternalTexture(static_cast<ID3D11Texture2D*>(m_postProcessingColorTarget2D->m_handle));

	// ctv will hold a reference to the backbuffer, so we
	// can release this functions hold on it; 
	DX_SAFE_RELEASE( back_buffer ); // I'm done using this - so release my hold on it (does not destroy it!)
}

void RenderContext::EndFrame()
{
	Texture2D* back_buffer = new Texture2D(this);
	m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&back_buffer->m_handle);
	
	m_context->CopyResource(reinterpret_cast<ID3D11Resource*>(back_buffer->m_handle), m_cpuColorTexture2D->m_handle);
	
	if(m_requestScreenShot)
	{
		m_requestScreenShot = false;

		Texture2D* staging_texture = new Texture2D(this);
		CreateStagingTexture(staging_texture, m_cpuColorTexture2D);
		CopyTexture(staging_texture, m_cpuColorTexture2D);

		Image* image = new Image(staging_texture->m_dimensions);

		RequestToMakeImageFromTexture* screen_shot_rq = new RequestToMakeImageFromTexture();
		screen_shot_rq->SetCategory(JOB_RENDER);
		screen_shot_rq->SetTexture(staging_texture);
		screen_shot_rq->SetImage(image);
		Dispatcher::AddRequest(screen_shot_rq);
	}
	
	// We're done rendering, so tell the swap chain they can copy the back buffer to the front (desktop/window) buffer
	// https://msdn.microsoft.com/en-us/library/windows/desktop/bb509554(v=vs.85).aspx
	m_swapChain->Present(0, 0);

	
	// TODO: We don't need to new and delete this object each frame, just update the m_rtv (being sure to release the old one)
	delete back_buffer;
	back_buffer = nullptr;
	
	delete m_frameBackbufferCtv;
	m_frameBackbufferCtv = nullptr;

	delete m_cpuColorTexture2D;
	m_cpuColorTexture2D = nullptr;

	delete m_cpuColorTargetView;
	m_cpuColorTargetView = nullptr;

	delete m_postProcessingColorTarget2D;
	m_postProcessingColorTarget2D = nullptr;

	delete	m_postProcessingColorTargetView;
	m_postProcessingColorTargetView = nullptr;
}

void RenderContext::Shutdown()
{
	std::map<std::string, Shader*>::iterator itr_shader = m_loadedShader.begin();
	while (itr_shader != m_loadedShader.end())
	{
		delete itr_shader->second;
		itr_shader = m_loadedShader.erase(itr_shader);
	}
	m_loadedShader.clear();

	std::map<std::string, BitmapFont*>::iterator itr_bitmap_font = m_loadedFonts.begin();
	while (itr_bitmap_font != m_loadedFonts.end())
	{
		delete itr_bitmap_font->second;
		itr_bitmap_font = m_loadedFonts.erase(itr_bitmap_font);
	}
	m_loadedFonts.clear();


	std::map<std::string, TextureView2D*>::iterator itr_texture_view2_d = m_loadedTextureViews.begin();
	while (itr_texture_view2_d != m_loadedTextureViews.end())
	{
		delete itr_texture_view2_d->second;
		itr_texture_view2_d = m_loadedTextureViews.erase(itr_texture_view2_d);
	}
	m_loadedTextureViews.clear();

	std::map<std::string, Material*>::iterator itr_material = m_loadedMaterials.begin();
	while (itr_material != m_loadedMaterials.end())
	{
		delete itr_material->second;
		itr_material = m_loadedMaterials.erase(itr_material);
	}
	m_loadedMaterials.clear();

	std::map<std::string, GPUMesh*>::iterator itr_mesh = m_meshDatabase.begin();
	while (itr_mesh != m_meshDatabase.end())
	{
		delete itr_mesh->second;
		itr_mesh = m_meshDatabase.erase(itr_mesh);
	}
	m_meshDatabase.clear();

	for (int samplers_id = 0; samplers_id < SAMPLE_MODE_COUNT; samplers_id++)
	{
		delete m_cachedSamplers[samplers_id];
		m_cachedSamplers[samplers_id] = nullptr;
	}

	delete m_postProcessingCamera;
	m_postProcessingCamera = nullptr;
	delete m_immediateVBO;
	m_immediateVBO = nullptr;

	delete m_modelBuffer;
	m_modelBuffer = nullptr;

	delete m_defaultDepthStencilView;
	m_defaultDepthStencilView = nullptr;

	delete m_depthTexture;
	m_depthTexture = nullptr;

	delete m_lightBuffer;
	m_lightBuffer = nullptr;

	delete m_effectBuffer;
	m_effectBuffer = nullptr;

	DX_SAFE_RELEASE(m_swapChain);
	DX_SAFE_RELEASE(m_context);
	DX_SAFE_RELEASE( m_defaultRasterState );
	//PrintDebug();
	DX_SAFE_RELEASE(m_device);
}

BitmapFont* RenderContext::CreateOrGetBitmapFont(const char* bitmap_font_name)
{
	const std::string concat_filename = "Data/Fonts/" + Stringf(bitmap_font_name);
	const std::map<std::string, BitmapFont*>::iterator bitmap_iterator =
		m_loadedFonts.find(bitmap_font_name);

	if (bitmap_iterator != m_loadedFonts.end())
	{
		return bitmap_iterator->second;
	}

	//else
	m_loadedFonts[bitmap_font_name] = CreateBitmapFontFromFile(concat_filename);
	return m_loadedFonts[bitmap_font_name];
}

BitmapFont* RenderContext::CreateBitmapFontFromFile(std::string const& font_file_path)
{
	TextureView2D* font_texture = CreateTextureViewFromFile(font_file_path);
	BitmapFont* new_bitmap_font = new BitmapFont(font_file_path.c_str(), font_texture);
	return new_bitmap_font;
}

//ClearColorTargets 
void RenderContext::ClearScreen(const Rgba& clear_color)
{
	// Clear the buffer.
	float clear_color_float[4] = {clear_color.r, clear_color.g, clear_color.b, clear_color.a};
	ColorTargetView* ctv = m_currentCamera->GetColorTargetView();
	m_context->ClearRenderTargetView(ctv->m_rtv, clear_color_float);
	// DX_SAFE_RELEASE( m_currentCamera->GetColorTargetView()->m_rtv );
	// done with the view - can release it (if you save it frame to frame, skip this step)
}

void RenderContext::ClearDepthStencilTarget(float depth, uint8_t stencil)
{
	ID3D11DepthStencilView *dsv = nullptr;
	
	if(m_currentCamera->m_depthStencilTarget)
	{
		dsv = m_currentCamera->m_depthStencilTarget->m_rtv;
	}
	
	dsv = m_defaultDepthStencilView->m_rtv;
	

	m_context->ClearDepthStencilView( dsv, D3D11_CLEAR_DEPTH , depth, stencil );
}

void RenderContext::BeginCamera(Camera* camera)
{
	// this just set ortho before - it will
	// do that again, plus these features; 
	m_currentCamera = camera; // remember the camera; 

	// first, figure out what we're rendering to; 
	ColorTargetView* view = camera->m_colorTarget;
	ID3D11RenderTargetView *dx_rtv = nullptr; 
	uint colorCount = 0U; 
	if (view != nullptr) {
		dx_rtv = view->m_rtv;
		colorCount = 1U; 
	}

	DepthStencilTargetView* dsv = camera->m_depthStencilTarget;
	ID3D11DepthStencilView* dx_dsv = nullptr; 
	if (dsv != nullptr) {
		dx_dsv = dsv->m_rtv;
	}
	else
	{
		dx_dsv = m_defaultDepthStencilView->m_rtv;
	}

	// Bind this as our output (this method takes an array, so 
	// this is binding an array of one)
	m_context->OMSetRenderTargets(colorCount,  &dx_rtv, dx_dsv);

	// Next, we have to describe WHAT part of the texture we're rendering to (called the viewport)
	// This is also usually managed by the camera, but for now, we will just render to the whole texture
	D3D11_VIEWPORT viewport;
	memset(&viewport, 0, sizeof(D3D11_VIEWPORT));
	viewport.TopLeftX = 0U;
	viewport.TopLeftY = 0U;
	viewport.Width = view->GetWidth();
	viewport.Height = view->GetHeight();
	viewport.MinDepth = 0.0f; // must be between 0 and 1 (defualt is 0);
	viewport.MaxDepth = 1.0f; // must be between 0 and 1 (default is 1)
	m_context->RSSetViewports(1, &viewport);

	// Next, update the uniform data, and bind it as an input for the shader
	// Camera data

	camera->UpdateUniformBuffer(this);
	BindUniformBuffer(UNIFORM_SLOT_CAMERA, camera->m_cameraUBO);
//	BindUniformBuffer(UNIFORM_SLOT_TIME, camera->m_timeUBO);

	// Bind model matrix; 
	BindModelMatrix( Matrix44::IDENTITY ); 
	BindUniformBuffer( UNIFORM_SLOT_MODEL, m_modelBuffer );

	m_lightBuffer->CopyCPUToGPU( &m_lightContainer, sizeof(m_lightContainer) );
	BindUniformBuffer(UNIFORM_SLOT_LIGHT, m_lightBuffer);

	BindUniformBuffer(UNIFORM_SLOT_EFFECT, m_effectBuffer);
}

void RenderContext::EndCamera(Camera* camera)
{
	UNUSED(camera);

	// unbind the color targets 
	m_context->OMSetRenderTargets(0, nullptr, nullptr);

	// no current camera being drawn; 
	m_currentCamera = nullptr;
}

TextureView2D* RenderContext::CreateOrGetTextureView2D(std::string const& filename)
{
	// m_normal stuff - if it exists, return it; 
	std::string file_directory = "Data/Images/" + filename;
	const auto item = m_loadedTextureViews.find(filename);
	if (item != m_loadedTextureViews.end())
	{
		return item->second;
	}

	//check if the filename is actually a collor
	if(filename.substr(0,2) == "0x")
		m_loadedTextureViews[filename] = CreateTextureViewFromColor(filename);
	else
		m_loadedTextureViews[filename] = CreateTextureViewFromFile(file_directory);
		
	return m_loadedTextureViews[filename];
}

void RenderContext::DrawVertexArray(const std::vector<Vertex_PCU>& vertexes)
{
	if (vertexes.empty())
		return;

	DrawVertexArray(&vertexes[0], static_cast<int>(vertexes.size()));
}

void RenderContext::DrawVertexArray(Vertex_PCU const* vertices, unsigned count)
{
	// copy to a vertex buffer
 	m_immediateVBO->CopyCPUToGPU(vertices, count);
 	BindVertexStream(m_immediateVBO);
	BufferAttributeT default_layout("default", eRenderDataFormat::RDF_VEC3, 0);
 	Draw(count, &default_layout);

//	m_immediateMesh->CopyVertexArray( vertices, count ); 
//	m_immediateMesh->SetDrawCall( false, count ); 
//	DrawMesh( *m_immediateMesh );
}

void RenderContext::Draw(unsigned vertex_count)
{
	m_context->Draw(vertex_count, 0U);
}

bool RenderContext::D3D11Setup(WindowContext* window_context)
{
	
	HWND hwnd = (HWND)window_context->m_hwnd;
	m_hwnd = window_context->m_hwnd;
	
	// Creation Flags
	// For options, see;
	// https://www.google.com/webhp?sourceid=chrome-instant&ion=1&espv=2&ie=UTF-8#safe=off&q=device_flags+%7C%3D+D3D11_CREATE_DEVICE_DEBUG%3B
	uint device_flags = 0U;
#if defined(RENDER_DEBUG)
	device_flags |= D3D11_CREATE_DEVICE_DEBUG;

	// This flag fails unless we' do 11.1 (which we're not), and we query that
	// the adapter support its (which we're not).  Just here to let you know it exists.
	// device_flags |= D3D11_CREATE_DEVICE_DEBUGGABLE; 
#endif

	// Setup our Swap Chain
	// For options, see;
	// https://www.google.com/webhp?sourceid=chrome-instant&ion=1&espv=2&ie=UTF-8#safe=off&q=DXGI_SWAP_CHAIN_DESC

	DXGI_SWAP_CHAIN_DESC swap_desc;
	memset(&swap_desc, 0, sizeof(swap_desc));

	// fill the swap chain description struct
	swap_desc.BufferCount = 2; // two buffers (one front, one back?)

	swap_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT // how swap chain is to be used
		| DXGI_USAGE_BACK_BUFFER;
	swap_desc.OutputWindow = hwnd; // the window to be copied to on present
	swap_desc.SampleDesc.Count = 1; // how many multisamples (1 means no multi sampling)

	RECT client_rect;
	GetClientRect(hwnd, &client_rect);
	uint width = client_rect.right - client_rect.left;
	uint height = client_rect.bottom - client_rect.top;

	// Default options.
	swap_desc.Windowed = TRUE; // windowed/full-screen mode
	swap_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // use 32-bit color
	swap_desc.BufferDesc.Width = width;
	swap_desc.BufferDesc.Height = height;


	// Actually Create
	HRESULT hr = D3D11CreateDeviceAndSwapChain(nullptr, // Adapter, if nullptr, will use adapter window is primarily on.
	                                           D3D_DRIVER_TYPE_HARDWARE,
	                                           // Driver Type - We want to use the GPU (HARDWARE)
	                                           nullptr,
	                                           // Software Module - DLL that implements software mode (we do not use)
	                                           device_flags, // device creation options
	                                           nullptr, // feature level (use default)
	                                           0U, // number of feature levels to attempt
	                                           D3D11_SDK_VERSION, // SDK Version to use
	                                           &swap_desc, // Description of our swap chain
	                                           &m_swapChain, // Swap Chain we're creating
	                                           &m_device, // [out] The device created
	                                           nullptr, // [out] Feature Level Acquired
	                                           &m_context); // Context that can issue commands on this pipe.

	// SUCCEEDED & FAILED are macros provided by Windows to checking
	// the results.  Almost every D3D call will return one - be sure to check it.
	return SUCCEEDED(hr);
}

//------------------------------------------------------------------------
// translate external enum to D3D11 specific options; 
D3D11_USAGE RenderContext::DXUsageFromMemoryUsage(eGPUMemoryUsage const usage)
{
	switch (usage)
	{
	case GPU_MEMORY_USAGE_GPU: return D3D11_USAGE_DEFAULT;
	case GPU_MEMORY_USAGE_STATIC: return D3D11_USAGE_IMMUTABLE;
	case GPU_MEMORY_USAGE_DYNAMIC: return D3D11_USAGE_DYNAMIC;
	case GPU_MEMORY_USAGE_STAGING: return D3D11_USAGE_STAGING;
	default:
		{
			ASSERT_RECOVERABLE(false, "DXUsageFromMemoryUsage was not assigned to a proper usage");
			return D3D11_USAGE_DYNAMIC;
		}
	}
}

void RenderContext::AddLight(const LightT& light)
{
	m_lightContainer.m_lights[m_lightID] = light;
	++m_lightID;
}

void RenderContext::UpdateLightAtIndex(const unsigned light_id, const LightT& light)
{
	m_lightContainer.m_lights[light_id] = light;
}

void RenderContext::RemoveLastLight()
{
	--m_lightID;
	m_lightContainer.m_lights[m_lightID] = LightT();
}

void RenderContext::RemoveLightAtIndex(const unsigned light_id)
{
	m_lightContainer.m_lights[light_id] = LightT();
}


TextureView2D* RenderContext::CreateTextureViewFromFile(std::string const& file_directory)
{
	Texture2D* tex = new Texture2D(this);
	if (!tex->LoadFromFile(file_directory))
	{
		delete tex;
		tex = nullptr;

		// optional -> store a null at this spot so we don't 
		// bother trying to reload it later; 
		return nullptr;
	}
	// create the view
	TextureView2D* view = tex->CreateTextureView2D();

	// THIS SHOULD BE FINE - the view should hold onto the D3D11 resource;
	// (I'm not 100% on this though, so if not, please Slack me)
	delete tex;
	tex = nullptr;
	return view;
}

Shader* RenderContext::CreateOrGetShader(std::string const& filename)
{
	const std::string concat_filename = "Data/HLSL/" + filename;
	const std::map<std::string, Shader*>::iterator shader_iterator = m_loadedShader.find(filename);

	if (shader_iterator != m_loadedShader.end())
	{
		return shader_iterator->second;
	}

	Shader* shader = CreateShadersFromFile(concat_filename);
	//m_currentShader = shader;
	
	if (shader->m_vertexShader.IsValid() && shader->m_pixelShader.IsValid())
	{
		m_loadedShader[filename] = shader;
		return shader;
	}

	ERROR_AND_DIE("shader invalid")
	return nullptr;
}

Material* RenderContext::CreateOrGetMaterial(std::string const& filename)
{
	const std::string concat_filename = "Data/Material/" + filename;
	const std::map<std::string, Material*>::iterator material_iterator = m_loadedMaterials.find(filename);

	if (material_iterator != m_loadedMaterials.end())
	{
		return material_iterator->second;
	}

	Material* material = CreateMaterialFromFile(concat_filename);
	//m_currentShader = material->m_shader;
	return material;
}

Sampler* RenderContext::CreateOrGetSampler(std::string const& sampler_type)
{
	if(sampler_type == "point")
	{
		return m_cachedSamplers[SAMPLE_MODE_POINT];
	}
	else if(sampler_type == "linear")
	{
		return m_cachedSamplers[SAMPLE_MODEL_LINEAR];
	}
	else
	{
		return m_cachedSamplers[SAMPLE_MODEL_LINEAR];
	}
}

GPUMesh* RenderContext::CreateMeshFromFile(std::string const& file_dir)
{
	CPUMesh temp_mesh;
	NamedStrings obj_meta_data = temp_mesh.ReadMeshXml(file_dir, "model");
	bool inverse_face = obj_meta_data.GetValue("invert_faces", false);
	bool inverse_uvs = obj_meta_data.GetValue("invert_UVs", false);
	float scale_model = obj_meta_data.GetValue("scale", 1.0f);
	std::string transform_model = obj_meta_data.GetValue("transform", "x y z");
	temp_mesh.PopulateFromMeshFile(obj_meta_data, inverse_face, inverse_uvs);
	Matrix44 transformMatrix = Matrix44::MakeTransformMat44(transform_model);
	temp_mesh.TransformVertexCollection(transformMatrix, scale_model);
	

	GPUMesh* temp_gpu = new GPUMesh(this);
	NamedStrings material_meta_data = temp_mesh.ReadMeshXml(file_dir, "material");
	temp_gpu->m_defaultMaterial = material_meta_data.GetValue("src", temp_gpu->m_defaultMaterial);
	temp_gpu->m_useIndexBuffer = false;
	temp_gpu->CreateFromCPUMesh<Vertex_Lit>(temp_mesh);

	return temp_gpu;
}

bool RenderContext::CreateStagingTexture(Texture2D* staging_texture, Texture2D* back_buffer)
{
	ID3D11Device* dd = m_device;

	// If created from image, we'll assume it is only needed
	// as a read-only texture resource (if this is not true, change the
	// signature to take in the option)
	//eTextureUsageBit textureUsage = TEXTURE_USAGE_TEXTURE_BIT;
	staging_texture->m_textureUsage = 0;
	// we are not picking static here because
	// we will eventually want to generate mipmaps,
	// which requires a GPU access pattern to generate.
	//eGPUMemoryUsage memoryUsage = GPU_MEMORY_USAGE_STAGING;
	staging_texture->m_memoryUsage = GPU_MEMORY_USAGE_STAGING;

	// Setup the Texture Description (what the resource will be like on the GPU)
	D3D11_TEXTURE2D_DESC texDesc;
	((ID3D11Texture2D*)back_buffer->m_handle)->GetDesc(&texDesc);
	
	IntVec2 dimensions = back_buffer->m_dimensions;
	texDesc.Usage = DXUsageFromMemoryUsage(staging_texture->m_memoryUsage);
	texDesc.BindFlags = 0U;
	texDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;                            // Determines how I can access this resource CPU side 
	texDesc.MiscFlags = 0U;

	// Actually create it
	DX_SAFE_RELEASE(staging_texture->m_handle);
	ID3D11Texture2D *tex2D = (ID3D11Texture2D*)staging_texture->m_handle;
	HRESULT hr = dd->CreateTexture2D(&texDesc,
		nullptr,
		&tex2D);
	//g_renderContext->GetDXContext()->CopyResource(m_backBuffer->m_handle, m_outTexture->m_handle);
	//ID3D11DeviceContext* d3dContext = m_context;
	
	if (SUCCEEDED(hr))
	{
		// save off the info; 
		staging_texture->m_dimensions = dimensions;
		staging_texture->m_handle = tex2D;
	}
	else
	{
		ASSERT_OR_DIE(tex2D == nullptr, "The texture was null"); // should be, just like to have the postcondition; 
	}
	
	return true;
}

TextureView2D* RenderContext::CreateTextureViewFromColor(std::string const& hex_number)
{
	Rgba color;
	color.SetFromHex(hex_number);

	Image image;
	image.SetTexelColor(IntVec2::ZERO, color);

	Texture2D* tex = new Texture2D(this);

	tex->LoadTextureFromImage(image);
	TextureView2D* view = tex->CreateTextureView2D();
	m_loadedTextureViews[hex_number] = view;

	delete tex;
	tex = nullptr;
	return view;	
}

GPUMesh* RenderContext::CreateOrGetMesh(std::string const& filename)
{
	const std::string concat_filename = "Data/Models/" + filename;
	const std::map<std::string, GPUMesh*>::iterator mesh_iterator = m_meshDatabase.find(filename);

	if (mesh_iterator != m_meshDatabase.end())
	{
		return mesh_iterator->second;
	}

	GPUMesh* model = CreateMeshFromFile(concat_filename);
	//m_currentShader = shader;

	if (model->m_indexBuffer->GetSize() == 0)
	{
		m_meshDatabase[filename] = model;
		return model;
	}

	ERROR_AND_DIE("Mesh invalid")
	return nullptr;
}

void RenderContext::BindShader(Shader* shader)
{
	if(m_currentShader == shader) return;
	
	m_currentShader = shader;

	m_context->VSSetShader(shader->m_vertexShader.m_vs, nullptr, 0U);
	m_context->PSSetShader(shader->m_pixelShader.m_ps, nullptr, 0U);

	shader->UpdateBlendStateIfDirty();
	shader->UpdateDepthStatesIfDirty();
	shader->UpdateRasterStateIfDirty();
	
	float black[] = {0.0f, 0.0f, 0.0f, 1.0f};
	m_context->OMSetBlendState(shader->m_blendState, // the d3d11 blend state object; 
	                           black, // blend factor (some blend options use this) 
	                           0xffffffff); // mask (what channels will get blended, this means ALL)  
	
	m_context->OMSetDepthStencilState(shader->m_depthStencilState, 0U);

	m_context->RSSetState(shader->m_rasterState);
}

void RenderContext::BindTextureView(unsigned slot, const TextureView* view)
{
	ID3D11ShaderResourceView* srv = nullptr;
	if (view != nullptr)
	{
		srv = view->m_handle;
	}
	else
	{
		srv = m_loadedTextureViews["white"]->m_handle;
	}

	// You can bind textures to the Vertex stage, but not samplers
	// We're *not* for now since no effect we do at Guildhall requires it, but
	// be aware it is an option; 
	// m_context->VSSetShaderResource( slot, 1U, &srv ); 

	m_context->PSSetShaderResources(slot, 1U, &srv);
}

void RenderContext::BindTextureView(unsigned slot, std::string const& name)
{
	TextureView2D* temp_texture_view2_d = CreateOrGetTextureView2D(name);
	BindTextureView(slot, temp_texture_view2_d);
}

void RenderContext::BindSampler(unsigned int slot, Sampler* sampler)
{
	if (sampler == nullptr)
	{
		sampler = m_cachedSamplers[SAMPLE_MODE_DEFAULT]; // bind the default if nothing is set
	}

	// create the dx handle; 
	sampler->CreateStateIfDirty(this);

	ID3D11SamplerState* handle = sampler->GetHandle();
	m_context->PSSetSamplers(slot, 1U, &handle);
}

void RenderContext::BindUniformBuffer(unsigned int slot, UniformBuffer* ubo)
{
	// The API allows us to bind multiple constant buffers at once
	// and binds to each stage seperately.  For simplicity, we'll
	// just assume a slot is uniform for the entire pipeline
	ID3D11Buffer* buffer = (ubo != nullptr) ? ubo->m_handle : nullptr;
	m_context->VSSetConstantBuffers(slot, 1U, &buffer);
	m_context->PSSetConstantBuffers(slot, 1U, &buffer);
}

void RenderContext::BindModelMatrix(Matrix44 const& model)
{
	if (m_modelBuffer == nullptr)
	{
		m_modelBuffer = new UniformBuffer(this);
	}

	ModelBufferT buffer;
 	buffer.m_model = model;
 
 	m_modelBuffer->CopyCPUToGPU( &buffer, sizeof(buffer));
}

void RenderContext::BindMaterial(const Material& mat)
{
	BindShader(mat.m_shader);
	BindTextureViewWithSampler(TEXTURE_MAP_DIFFUSE, mat.m_textures[TEXTURE_MAP_DIFFUSE], mat.m_samplers[TEXTURE_MAP_DIFFUSE]);
	BindTextureViewWithSampler(TEXTURE_MAP_NORMAL, mat.m_textures[TEXTURE_MAP_NORMAL], mat.m_samplers[TEXTURE_MAP_NORMAL]);
	BindTextureViewWithSampler(TEXTURE_MAP_EMISSIVE, mat.m_textures[TEXTURE_MAP_EMISSIVE],  mat.m_samplers[TEXTURE_MAP_EMISSIVE]);
	BindTextureViewWithSampler(TEXTURE_MAP_SPECULAR, mat.m_textures[TEXTURE_MAP_SPECULAR], mat.m_samplers[TEXTURE_MAP_SPECULAR]);
	BindTextureViewWithSampler(TEXTURE_MAP_AMBIENT_OCCLUSION, mat.m_textures[TEXTURE_MAP_AMBIENT_OCCLUSION], mat.m_samplers[TEXTURE_MAP_AMBIENT_OCCLUSION]);
	BindTextureViewWithSampler(TEXTURE_MAP_BUMP, mat.m_textures[TEXTURE_MAP_BUMP],  mat.m_samplers[TEXTURE_MAP_BUMP]);
	BindUniformBuffer(0, mat.GetUniformBuffer());
}

void RenderContext::BindEffect(float amount)
{
	if (m_effectBuffer == nullptr)
	{
		m_effectBuffer = new UniformBuffer(this);
	}

	EffectBufferT buffer;
	buffer.m_effectStrength = amount;
	
	m_effectBuffer->CopyCPUToGPU( &buffer, sizeof(buffer));

	//BindUniformBuffer(8, m_effectBuffer); binding in begin camera
}

void RenderContext::BindTextureViewWithSampler(unsigned slot, const TextureView* view)
{
	BindTextureView(slot, view);

	if (view != nullptr)
	{
		BindSampler(slot, view->m_sampler);
	}
	else
	{
		BindSampler(slot, nullptr);
	}
}

void RenderContext::BindTextureViewWithSampler(unsigned slot, std::string const& name)
{
	TextureView* tx_view = CreateOrGetTextureView2D(name);
	BindTextureViewWithSampler(slot, tx_view);
}

void RenderContext::BindTextureViewWithSampler(unsigned slot, const TextureView* view, Sampler* sampler)
{
	BindTextureView(slot, view);
	BindSampler(slot, sampler);
}

void RenderContext::BindTextureViewWithSampler(unsigned slot, const TextureView* view, eSampleMode mode)
{
	BindTextureView(slot, view);
	BindSampler(mode);
}

void RenderContext::BindTextureViewWithSampler(unsigned slot, std::string const& name, eSampleMode mode)
{
	TextureView* tx_view = CreateOrGetTextureView2D("Data/Images/" + name);
	BindTextureViewWithSampler(slot, tx_view, mode);
}

void RenderContext::BindSampler(eSampleMode mode)
{
	Sampler* sampler = m_cachedSamplers[mode];

	//create the dx handler
	sampler->CreateStateIfDirty(this);

	ID3D11SamplerState* handle = sampler->GetHandle();
	m_context->PSSetSamplers(0, 1U, &handle);
}

void RenderContext::BindVertexStream(VertexBuffer* vbo)
{
	// Bind the input stream; 
	uint stride = vbo->m_elementStride;
	uint offset = 0U;
	m_context->IASetVertexBuffers(0, // Start slot index
	                              1, // Number of buffers we're binding
	                              &vbo->m_handle, // Array of buffers
	                              &stride,
	                              // Stride (read: vertex size, or amount we move forward each vertex) for each buffer
	                              &offset); // Offset into each buffer (array - we are only passing one. 
}

void RenderContext::BindIndexStream(IndexBuffer* ibo)
{
	ID3D11Buffer *handle = nullptr; 
	if (ibo != nullptr) {
		handle = ibo->m_handle; 
	}

	m_context->IASetIndexBuffer( handle, 
		DXGI_FORMAT_R32_UINT,      // 32-bit indices;            
		0 );  // byte offset 
}

void RenderContext::Draw(unsigned vertex_count, const BufferAttributeT layout[], unsigned byte_offset)
{
	m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// We need to describe the input to the shader
	// TODO: only create an input layout if the vertex type changes; 
	// TODO: When different vertex types come on-line, look at the current bound
	//       input streams (VertexBuffer) for the layout

	// TODO: need to catch layouts with nullptrs
	if(layout == nullptr)
		return;

	bool result = m_currentShader->CreateInputLayoutFromBufferAttribute(layout);
	// TODO: ID3D11InputLayout *layout = m_currentShader->GetInputLayoutForBufferLayout( mesh->GetBufferLayout() ); 

	ASSERT_OR_DIE(result, "Was not able to create input layout for vertex PCU.");

	m_context->IASetInputLayout(m_currentShader->m_inputLayout);
	m_context->Draw(vertex_count, byte_offset);

}

void RenderContext::DrawMesh(GPUMesh const& mesh)
{
	BindVertexStream( mesh.m_vertexBuffer ); 
	BindIndexStream( mesh.m_indexBuffer ); 

	if (mesh.m_useIndexBuffer) {
		DrawIndexed( mesh.m_elementCount, mesh.m_vertexBuffer->m_layout ); 
	} else {
		Draw( mesh.m_elementCount, mesh.m_vertexBuffer->m_layout ); 
	}
}

void RenderContext::DrawIndexed(unsigned elem_count, const BufferAttributeT layout[])
{
	m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	bool result = m_currentShader->CreateInputLayoutFromBufferAttribute(layout);
	//TODO: ID3D11InputLayout *layout = m_currentShader->GetInputLayoutForBufferLayout( mesh->GetBufferLayout() ); 

	ASSERT_OR_DIE(result, "Was not able to create input layout for vertex PCU.");

	m_context->IASetInputLayout(m_currentShader->m_inputLayout);
	m_context->DrawIndexed( elem_count, 
		0,       // elem offset 
		0 );     // vert offset 
}

void RenderContext::DrawModel(Model* model)
{
	//BindModelMatrix(model->m_modelMatrix);
	BindMaterial(*model->m_material);
	DrawMesh(*model->m_mesh);
}

ColorTargetView* RenderContext::GetFrameColorTarget()
{
	// Create a `ColorTargetView` matching your current backbuffer
	//ColorTargetView* ctv = m_frameBackbufferCtv;

	// Create a `Texture2D` to use as a color target matching your swapchain
	//Texture2D* texture = nullptr;
	//ctv->CreateForTexture(texture);
	//m_frameBackbufferCtv = ctv;


	//return m_frameBackbufferCtv;
	return m_cpuColorTargetView;
}

DepthStencilTargetView* RenderContext::GetFrameDepthStencilTarget()
{
	return m_defaultDepthStencilView;
}

void RenderContext::CopyTexture(Texture2D* dst, Texture2D* src)
{
	m_context->CopyResource(dst->m_handle, src->m_handle);
}

void RenderContext::ApplyEffect(Material& mat)
{
	m_postProcessingCamera->SetOrthoView(Vec2::ZERO, Vec2(m_postProcessingColorTargetView->GetWidth(), m_postProcessingColorTargetView->GetHeight()));
	m_postProcessingCamera->SetColorTarget(m_postProcessingColorTargetView);

	//bind texture
	TextureView* texture_view = m_cpuColorTexture2D->CreateTextureView2D();
	mat.SetTextureView(TEXTURE_MAP_DIFFUSE, texture_view);
	BindMaterial(mat);
		
	BeginCamera(m_postProcessingCamera);

	Draw(3); // draw full screen triagnle

	EndCamera(m_postProcessingCamera);

	BindTextureView(0U, nullptr);
	CopyTexture(m_cpuColorTexture2D, m_postProcessingColorTarget2D);
	
	delete texture_view;
	texture_view = nullptr;
}

void RenderContext::ApplyEffect(ColorTargetView* dst, TextureView* src, Material* mat)
{
	UNUSED(dst);
	UNUSED(src);
	UNUSED(mat);
}

void RenderContext::SetupDefaultRasterState()
{
	D3D11_RASTERIZER_DESC desc;

	desc.FillMode = D3D11_FILL_SOLID;
	desc.CullMode = D3D11_CULL_BACK;
	desc.FrontCounterClockwise = TRUE;
	desc.DepthBias = 0U;
	desc.DepthBiasClamp = 0.0f;
	desc.SlopeScaledDepthBias = 0.0f;
	desc.DepthClipEnable = TRUE;
	desc.ScissorEnable = FALSE;
	desc.MultisampleEnable = FALSE;
	desc.AntialiasedLineEnable = FALSE;

	// ID3D11RasterizerState *m_defaultRasterState; 
	m_device->CreateRasterizerState(&desc, &m_defaultRasterState);
}

Shader* RenderContext::CreateShadersFromFile(std::string const& filename)
{
	char* buffer = nullptr;
	const unsigned long buffer_len = CreateBufferForFile(filename.c_str(), &buffer);

	Shader* shader = new Shader(this);

	shader->m_vertexShader.LoadShaderFromSource(this, filename, buffer, buffer_len, SHADER_STAGE_VERTEX);
	shader->m_pixelShader.LoadShaderFromSource(this, filename, buffer, buffer_len, SHADER_STAGE_FRAGMENT);

	delete(buffer);
	
	return shader;
}

Material* RenderContext::CreateMaterialFromFile(std::string const& filename)
{
	const char* material_xml_file_path = filename.c_str();
	tinyxml2::XMLDocument shader_doc;
	shader_doc.LoadFile(material_xml_file_path);

	if(shader_doc.ErrorID() != tinyxml2::XML_SUCCESS)
	{
		std::string error_message = "\n";

		error_message.append(Stringf(">> ERROR loading XML doc \"%s\"\n", material_xml_file_path));
		error_message.append(Stringf(">> errorID = %i\n", shader_doc.ErrorID()));
		error_message.append(Stringf(">> errorLineNum = %i\n", shader_doc.ErrorLineNum()));
		error_message.append(Stringf(">> errorName = \"%s\"\n", shader_doc.ErrorName()));

		ERROR_AND_DIE(error_message);
	}

	tinyxml2::XMLElement* root_material = shader_doc.RootElement();
	Material* material = new Material(this, root_material);
	std::string value = root_material->FirstAttribute()->Value();
	m_loadedMaterials[value] = material;
	return material;
}

Shader* RenderContext::CreateShadersFromXML(std::string const& dir)
{
	const char* shader_xml_file_path = dir.c_str();
	tinyxml2::XMLDocument shader_doc;
	shader_doc.LoadFile(shader_xml_file_path);

	if(shader_doc.ErrorID() != tinyxml2::XML_SUCCESS)
	{
		std::string error_message = "\n";

		error_message.append(Stringf(">> ERROR loading XML doc \"%s\"\n", shader_xml_file_path));
		error_message.append(Stringf(">> errorID = %i\n", shader_doc.ErrorID()));
		error_message.append(Stringf(">> errorLineNum = %i\n", shader_doc.ErrorLineNum()));
		error_message.append(Stringf(">> errorName = \"%s\"\n", shader_doc.ErrorName()));

		ERROR_AND_DIE(error_message);
	}

	tinyxml2::XMLElement* root_shader = shader_doc.RootElement();
	//tinyxml2::XMLNode* child_def = shader_doc.FirstChild();
	NamedStrings unlit_shader_doc;

	for(const tinyxml2::XMLElement* pass = root_shader->FirstChildElement(); pass; 
		pass = pass->NextSiblingElement())
	{
		std::string pass_name = pass->Name();

		unlit_shader_doc.SetValue(pass_name, pass->Attribute("src"));

		for(const tinyxml2::XMLElement* child_element = pass->FirstChildElement(); child_element; 
			child_element = child_element->NextSiblingElement())
		{
			for(const tinyxml2::XMLAttribute* child_attribute = child_element->FirstAttribute(); child_attribute; 
				child_attribute = child_attribute->Next())
			{
				std::string element_name = pass_name;
				element_name += "_";
				element_name += child_element->Name();
				element_name += "_";
				element_name += child_attribute->Name();
				unlit_shader_doc.SetValue(element_name, child_attribute->Value());
			}

			if(!strcmp(child_element->Name(), "blend"))
			{
				std::string blend_name = "blend";
				for(const tinyxml2::XMLElement* grand_child_element = child_element->FirstChildElement(); grand_child_element; 
					grand_child_element = grand_child_element->NextSiblingElement())
				{
					for(const tinyxml2::XMLAttribute* grand_child_attribute = grand_child_element->FirstAttribute(); grand_child_attribute; 
						grand_child_attribute = grand_child_attribute->Next())
					{
						std::string element_name = pass_name;
						element_name += "_";
						element_name += blend_name;
						element_name += "_";
						element_name += grand_child_element->Name();
						element_name += "_";
						element_name += grand_child_attribute->Name();
						unlit_shader_doc.SetValue(element_name, grand_child_attribute->Value());
					}
				}
			}
		}
	}

	std::string filename = unlit_shader_doc.GetValue("pass", "HLSL/unlit.hlsl");

	char* buffer = nullptr;
	const unsigned long buffer_len = CreateBufferForFile(filename.c_str(), &buffer);

	Shader* shader = new Shader(this);

	std::string vert_stage_string = unlit_shader_doc.GetValue("pass_vert_entry", "VertexFunction");
	std::string frag_stage_string = unlit_shader_doc.GetValue("pass_frag_entry", "FragmentFunction");
	eShaderStage vert_stage = SHADER_STAGE_VERTEX;
	eShaderStage frag_stage = SHADER_STAGE_FRAGMENT;

	if(vert_stage_string == "VertexFunction") vert_stage = SHADER_STAGE_VERTEX;
	if(frag_stage_string == "FragmentFunction") frag_stage = SHADER_STAGE_FRAGMENT;

	shader->m_vertexShader.LoadShaderFromSource(this, filename, buffer, buffer_len, vert_stage);
	shader->m_pixelShader.LoadShaderFromSource(this, filename, buffer, buffer_len, frag_stage);

	std::string color_op = unlit_shader_doc.GetValue("pass_blend_color_op", "add");
	std::string color_src = unlit_shader_doc.GetValue("pass_blend_color_src", "one");
	std::string color_dst = unlit_shader_doc.GetValue("pass_blend_color_dst", "zero");
	std::string alpha_op = unlit_shader_doc.GetValue("pass_blend_alpha_op", "add");
	std::string alpha_src = unlit_shader_doc.GetValue("pass_blend_alpha_src", "inv_dst_alpha");
	std::string alpha_dst = unlit_shader_doc.GetValue("pass_blend_alpha_dst", "one");

	eBlendMode blend_mode = BLEND_MODE_ALPHA;

	if(color_op == "add" && color_src == "one" && color_dst == "zero" && alpha_op == "add" &&
		alpha_src == "inv_dst_alpha" && alpha_dst == "one")
		blend_mode = BLEND_MODE_OPAQUE;

	shader->SetBlendMode(blend_mode);

	bool depth_write = unlit_shader_doc.GetValue("pass_depth_write", true);
	std::string depth_test = unlit_shader_doc.GetValue("pass_depth_test", "lequal");

	//test: never, less, equal, lequal, greater, gequal, not, always 
	eCompareOp test = NO_COMPARE_OP;
	if(depth_test == "never") test = COMPARE_NEVER;
	else if (depth_test == "less") test = COMPARE_LESS;
	else if (depth_test == "equal") test = COMPARE_EQUAL;
	else if (depth_test == "lequal") test = COMPARE_LESS_EQUAL;
	else if (depth_test == "greater") test = COMPARE_GREATER;
	else if (depth_test == "gequal") test = COMPARE_GREATER_EQUAL;
	else if (depth_test == "not") test = COMPARE_NOT_EQUAL;
	else if (depth_test == "always") test = COMPARE_ALWAYS;

	shader->SetDepth(test ,depth_write);

	return shader;
}
