//------------------------------------------------------------------------
// RenderContext.hpp
//------------------------------------------------------------------------

#pragma once
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec4.hpp"
#include "Engine/Core/EventSystem.hpp"

//#define INITGUID
#include <d3d11.h>
#pragma comment( lib, "d3d11.lib" )
#include <map>

#define DX_SAFE_RELEASE(dx_resource)   if ((dx_resource) != nullptr) { dx_resource->Release(); dx_resource = nullptr; }

//------------------------------------------------------------------------

class Image;
class Model;
class Material;
struct BufferAttributeT;
class GPUMesh;
class WindowContext;
class DepthStencilTargetView;
class Texture;
class TextureView;
class TextureView2D;
class Shader;
class Sampler;
class VertexBuffer;
class IndexBuffer;

// As the engine designer, we're going to make the decision to 
// just reserve certain slot indices for specific constant buffers
// We will gruop uniform buffers by how often they change, and for now, we
// really only use the CAMERA
enum eCoreUniformSlot
{
	UNIFORM_SLOT_FRAME = 1,
	UNIFORM_SLOT_CAMERA = 2,
	UNIFORM_SLOT_MODEL = 3,
	UNIFORM_SLOT_LIGHT = 4,
	UNIFORM_SLOT_EFFECT = 8
};

// A enum to pick a samplerstate to use
enum eSampleMode
{
	SAMPLE_MODE_POINT = 0,
	SAMPLE_MODEL_LINEAR,
	// SAMPLE_MODE_BILINEAR

	SAMPLE_MODE_COUNT,
	SAMPLE_MODE_DEFAULT = SAMPLE_MODEL_LINEAR
	// different games may want to change this.  My projects will use Linear, SD4 will want point; 
};

struct LightT 
{
	LightT();
	LightT( const Rgba& color, float intensity, const Vec3& position, float is_directional, 
		const Vec3& direction, const Vec3& diffuse_attenuation, const Vec3& specular_attenuation);

public:	
	Vec3 m_color; 
	float m_intensity; 

	Vec3 m_position; 
	float m_isDirectional;

	Vec3 m_direction;

private:
	float pad10;

public:
	Vec3 m_diffuseAttenuation;

private:
	float pad20;

public:
	Vec3 m_specularAttenuation;

private:
	float pad30; 
};

// A constant buffer can address up 64KB,
// with 15 available slots (docs usually say 14, as one gets used
// up by the $GLOBAL constant buffer; 

//------------------------------------------------------------------------
// FRAME data is information that is constant for an entire
// frame - you can update this UBO during RenderContext::BeginFrame, 
// as well as bind it (as long as you don't accidently bind over it
//------------------------------------------------------------------------

//------------------------------------------------------------------------
// I usually use this to store time, as it is a common
// input for shader effects
// A UBO should usually be a multipole of 16-bytes large, so we'll just add some 
// variations of time for our shaders to use; 
// slot: 1
struct FrameBufferT
{
	float m_time;
	float m_halfTime;
	float m_doubleTime;
	float m_quadTime;
};

//------------------------------------------------------------------------
// slot 2; 
// What data does the camera provide the shader (slot:2)
// This must follow a 16-byte element alignemnt (for now, this only contains
// ortho information; 
// camera slot I usually bind all constants that change per view; 
struct CameraBufferT
{
// 	Vec2 m_orthoMin;
// 	Vec2 m_orthoMax;
	Matrix44 m_view;
	Matrix44 m_projection;
	Matrix44 m_viewProjection;
	Vec3 m_cameraPosition;
	float m_camID;
};

struct ModelBufferT
{
	Matrix44 m_model;
};

struct LightBufferT
{
	Vec4 m_ambient = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
	float m_specFactor = 1.0f;
	float m_specPower = 1.0f;
	float m_emissiveFactor = 0.0f;
	float pad0;
	LightT m_lights[8];
};

struct EffectBufferT
{
	float m_effectStrength = 0.0f;
	float padding_0 = 0.0f;
	float padding_1 = 0.0f;
	float padding_2 = 0.0f;
};


// I start at slot 1 out of habit.  I reserve slot 0 for what I call the "SYTEM" buffer, which
// is usually just debug constants and configurations as I'm working. 
// The uniform buffer system is very free-form, you can bind any data to any slot
// but I like to usually have my engine renderer reserve a few slots
// grouped by update frequency.

// In D3D11 and GL - buffers also are supplied hints on 
// desired usage patterns to help the GPU optimize for it
enum eGPUMemoryUsage
{
	GPU_MEMORY_USAGE_GPU,
	// Can be written/read from GPU only (Color Targets are a good example)
	GPU_MEMORY_USAGE_STATIC,
	// Created, and are read only after that (ex: textures from images, sprite atlas)
	GPU_MEMORY_USAGE_DYNAMIC,
	// Update often from CPU, used by the GPU (CPU -> GPU updates, used by shaders.  ex: Uniform Buffers)
	GPU_MEMORY_USAGE_STAGING,
	// For getting memory from GPU to CPU (can be copied into, but not directly bound as an output.  ex: Screenshots system)
};

class RenderContext
{
	friend class ShaderStage;
	friend class RenderBuffer;
	friend class Shader;
	friend class Texture2D;
	friend class Sampler;

public:
// 	enum BlendMode
// 	{
// 		BLEND_MODE_OPAQUE,
// 		BLEND_MODE_ALPHA,
// 		BLEND_MODE_ADDITIVE,
// 		NUM_BLEND_MODES
// 	};

	RenderContext(WindowContext* window_context);
	~RenderContext();

	void Startup();
	void Shutdown();

	void BeginFrame();
	void EndFrame();

	void BeginCamera(Camera* camera);
	void EndCamera(Camera* camera);

	TextureView2D* CreateOrGetTextureView2D(std::string const& filename); 
	BitmapFont* CreateOrGetBitmapFont(const char* bitmap_font_name);
	Shader* CreateOrGetShader(std::string const& filename);
	Material* CreateOrGetMaterial( std::string const &filename );
	Sampler* CreateOrGetSampler( std::string const& sampler_type );
	GPUMesh* CreateOrGetMesh( std::string const &filename ); 

	Shader* CreateShadersFromXML(std::string const& filename);
	
	void BindShader(Shader* shader);
	void BindTextureView(unsigned int slot, const TextureView* view);
	void BindTextureView(unsigned int slot, std::string const& name);
	void BindSampler(unsigned int slot, Sampler* sampler);
	void BindSampler(eSampleMode mode);
	void BindVertexStream(VertexBuffer* vbo); // Stream Data
	void BindIndexStream(IndexBuffer* ibo);
	void BindUniformBuffer(unsigned int slot, UniformBuffer* ubo); // Uniform/Constant Data
	void BindModelMatrix( Matrix44 const& model);
	void BindMaterial(const Material& mat);
	void BindEffect( float amount );

	//void BindIndexStream( IndexBuffer *ibo);

	// sampler I'm storing with the view in this design - but still giving
	// the context the option of binding a view with a different sampler if we so choose; 
	void BindTextureViewWithSampler(unsigned int slot, const TextureView* view);
	void BindTextureViewWithSampler(unsigned int slot, std::string const& name);
	void BindTextureViewWithSampler(unsigned int slot, const TextureView* view, Sampler* sampler);
	void BindTextureViewWithSampler(unsigned int slot, const TextureView* view, eSampleMode mode);
	void BindTextureViewWithSampler(unsigned int slot, std::string const& name, eSampleMode mode);
	void ClearScreen(const Rgba& clear_color);
	void ClearDepthStencilTarget( float depth = 1.0f, uint8_t stencil = 0U );

	//void DrawVertexArray(size_t num_vertexes, const Vertex_PCU* vertexes) const;
	void DrawVertexArray(const std::vector<Vertex_PCU>& vertexes);
	void DrawVertexArray(Vertex_PCU const* vertices, unsigned int count);
	void Draw(unsigned int vertex_count);
	void Draw(unsigned int vertex_count, const BufferAttributeT layout[], unsigned int byte_offset = 0U);
	void DrawMesh( GPUMesh const& mesh);
	void DrawIndexed( unsigned int elem_count, const BufferAttributeT layout[]);
	void DrawModel( Model* model );

	ColorTargetView* GetFrameColorTarget();
	DepthStencilTargetView* GetFrameDepthStencilTarget();

	void CopyTexture( Texture2D* dst, Texture2D* src );   // A10
	void ApplyEffect( Material& mat );
	void ApplyEffect( ColorTargetView* dst, TextureView* src, Material* mat );

	// Resurrected Functions
	void SetupDefaultRasterState();
	bool D3D11Setup(WindowContext* window_context);

	static D3D11_USAGE DXUsageFromMemoryUsage(eGPUMemoryUsage usage);

	void AddLight(const LightT& light);
	void UpdateLightAtIndex(unsigned int light_id, const LightT& light);
	void RemoveLastLight();
	void RemoveLightAtIndex(unsigned int light_id);
	static bool SetDirectionalColor(EventArgs& args); // enable/disable rendering debug commands
	static bool SetDirectionalDirection(EventArgs& args); // Clear all Debug Render commands that have duration; 

	bool IsMeshLoaded( std::string const& file_name );
	CPUMesh* CreateCPUMeshFromFile( std::string const& file_name );
	GPUMesh* CreateGPUMeshFromFile( CPUMesh& mesh, std::string const& file_name );

	bool IsImageLoaded( const std::string& file_name );
	TextureView2D* CreateTextureView2DFromImage( Image* image, const std::string& file_name );

	ID3D11Device* GetDx11Device();
	ID3D11DeviceContext* GetDx11Context();
	bool LoadTextureFromFile(const char* filename, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height);	

	void RequestScreenShot() { m_requestScreenShot = true; }
	
public:
	LightBufferT m_lightContainer;
	EffectBufferT m_effectContainer;

private: //private functions
	//Texture* CreateTextureFromFile(const char* image_file_path);
	TextureView2D* CreateTextureViewFromFile(std::string const& filename);
	BitmapFont* CreateBitmapFontFromFile(std::string const& filename);
	Shader* CreateShadersFromFile(std::string const& filename);
	Material* CreateMaterialFromFile(std::string const& filename);
	GPUMesh* CreateMeshFromFile( std::string const& file_dir );
	bool CreateStagingTexture(Texture2D* staging_texture, Texture2D* back_buffer);
	
	TextureView2D* CreateTextureViewFromColor(std::string const& hex_number);


	void CacheSamplers();
	void PrintDebug();

	//D3D11 specific functions

private: //private members
	//std::map<std::string, Texture*> m_loadedTextures;
	std::map<std::string, BitmapFont*> m_loadedFonts;
	std::map<std::string, TextureView2D*> m_loadedTextureViews;
	std::map<std::string, Shader*> m_loadedShader;
	std::map<std::string, Material*> m_loadedMaterials; 
	std::map<std::string, GPUMesh*> m_meshDatabase;

	ID3D11Device* m_device = nullptr;
	ID3D11DeviceContext* m_context = nullptr;

	Camera* m_currentCamera = nullptr;
	Camera* m_postProcessingCamera = nullptr;

	IDXGISwapChain*			m_swapChain = nullptr;
	ColorTargetView*		m_frameBackbufferCtv = nullptr;
	Texture2D*				m_cpuColorTexture2D = nullptr;
	ColorTargetView*		m_cpuColorTargetView = nullptr;
	Texture2D*				m_postProcessingColorTarget2D = nullptr;
	ColorTargetView*		m_postProcessingColorTargetView = nullptr;


	DepthStencilTargetView* m_defaultDepthStencilView = nullptr;
	Texture2D*				m_depthTexture = nullptr;

	Shader*				m_currentShader = nullptr;
	VertexBuffer*		m_immediateVBO = nullptr;
	GPUMesh*			m_immediateMesh;   
	UniformBuffer*		m_modelBuffer = nullptr;
	UniformBuffer*		m_lightBuffer = nullptr;
	UniformBuffer*		m_effectBuffer = nullptr;
	unsigned int		m_lightID = 0;
	void*				m_hwnd = nullptr;
	bool				m_requestScreenShot = false;
	
	// There are a small set of sampler state that
	// we can get by with just reusing everywhere, 
	// so instead of creating a sampler per texture, 
	// we'll pull from this store; 
	Sampler* m_cachedSamplers[SAMPLE_MODE_COUNT];
	ID3D11RasterizerState* m_defaultRasterState = nullptr;
};
