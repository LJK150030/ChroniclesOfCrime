#pragma once

// Required Headers
//#define INITGUID
#include <d3d11.h>
#pragma comment( lib, "d3d11.lib" )

#include <string>

#define DX_SAFE_RELEASE(dx_resource)   if ((dx_resource) != nullptr) { dx_resource->Release(); dx_resource = nullptr; }

struct BufferAttributeT;
class RenderContext;
struct ID3D10Blob;

enum eShaderStage
{
	SHADER_STAGE_VERTEX,
	SHADER_STAGE_FRAGMENT,
};

// A programmable shader stage, either vertex or pixel fornow; 
class ShaderStage
{
public:
	ShaderStage();
	~ShaderStage();

	bool LoadShaderFromSource(RenderContext* ctx, std::string const& filename, void* source, size_t source_len,
	                          eShaderStage stage);
	bool IsValid() const { return m_handle != nullptr; }

	ID3D10Blob* m_bytecode;

public:
	eShaderStage m_stage;

	union
	{
		ID3D11Resource* m_handle;
		ID3D11VertexShader* m_vs;
		ID3D11PixelShader* m_ps;
	};
};

//------------------------------------------------------------------------

// two options for this - either you can name common blend mode configurations
// using an enum, or you can fully expose it using a color and alpha blend operation; 
// Believe the enum is what Squirrel is intended for your engines; 
enum eBlendMode
{
	// op,  src_factor,  dst_factor
	BLEND_MODE_OPAQUE,
	// ADD, ONE,         ZERO              (or blend disabled)
	BLEND_MODE_ALPHA,
	// ADD, SRC_ALPHA,   INV_SRC_ALPHA,
	BLEND_MODE_ADDTIVE,
	// ADD, ONE,         ONE
};

enum eCompareOp
{
	NO_COMPARE_OP = 0,
	COMPARE_NEVER,
	COMPARE_LESS,
	COMPARE_EQUAL,
	COMPARE_LESS_EQUAL,
	COMPARE_GREATER,
	COMPARE_NOT_EQUAL,
	COMPARE_GREATER_EQUAL,
	COMPARE_ALWAYS,
	NUM_COMPARE_OPS
};

enum eFillMode
{
	NO_FILL = -1,
	WIREFRAME,
	SOLID,
	NUM_FILL_MODES
};

enum eCullMode
{
	NO_CULL = -1,
	FRONT_CULL,
	BACK_CULL,
	NUM_CULL_MODES
};

enum eVertexType
{
	NO_VERTEX = -1,
	VERTEX_PCU,
	VERTEX_LIT,
	NUM_VERTEX_TYPES
};

// other options - fully expose the options; 
// enum eBlendOp { ... }; // maps to https://docs.microsoft.com/en-us/windows/desktop/api/d3d11/ne-d3d11-d3d11_blend_op
// enum eBlendFactor { ... }; // maps to https://docs.microsoft.com/en-us/windows/desktop/api/d3d11/ne-d3d11-d3d11_blend
// struct blend_info_t { eBlendOp op; eBlendFactor srcFactor;  eBlendFactor dstFactor; }; 

class Shader
{
public:
	explicit Shader(RenderContext* rc);
	explicit Shader(Shader* copy);
	~Shader();

	//bool CreateInputLayoutForVertexPCU();
	bool CreateInputLayoutFromBufferAttribute( BufferAttributeT const layout[] );
	bool UpdateBlendStateIfDirty();
	bool UpdateDepthStatesIfDirty();
	bool UpdateRasterStateIfDirty();
	void SetBlendMode(enum eBlendMode mode);
	void SetDepth( eCompareOp op, bool write );
	void SetRasterState( eFillMode fill_mode, eCullMode cull_mode, bool is_front_counter_clockwise);

private:
	D3D11_FILL_MODE DXGetFillMode(eFillMode mode);
	D3D11_CULL_MODE DXGetCullMode(eCullMode mode);

public:
	RenderContext* m_renderContext = nullptr;

	ShaderStage m_vertexShader;
	ShaderStage m_pixelShader;

	// blend state - not sure how your SD1 exposes this.  Some Engines expose the blend factors
	// and some engines will expose an enum for common setups (like alpha, additive, etc...)

	eBlendMode m_blendMode			= BLEND_MODE_ALPHA;
	bool m_blendStateDirty			= true;
	// blend_info_t m_colorBlend; 
	// blend_info_t m_alphaBlend; // eBlendOp m_blendOp; 
	// eBlendFactor m_srcFactor;  
	// eBlendFactor m_dstFactor; 

	eCompareOp m_depthCompareOp     = COMPARE_LESS_EQUAL;    
	bool m_writeDepth               = true;
	bool m_depthStateDirty			= true;

	eFillMode m_fillMode			= SOLID;
	eCullMode m_cullMode			= BACK_CULL;
	bool m_frontCounterClockwise	= true;
	bool m_rasterStateDirty			= true;

	ID3D11DepthStencilState* m_depthStencilState = nullptr;
	ID3D11InputLayout* m_inputLayout = nullptr;
	ID3D11BlendState* m_blendState = nullptr;
	ID3D11RasterizerState* m_rasterState = nullptr;
};
