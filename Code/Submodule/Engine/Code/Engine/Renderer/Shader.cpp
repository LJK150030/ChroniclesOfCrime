#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/Vertex_PCU.hpp"

#include <d3dcompiler.h>


static char const* GetEntryForStage(const eShaderStage stage)
{
	switch (stage)
	{
	case SHADER_STAGE_VERTEX: return "VertexFunction";
	case SHADER_STAGE_FRAGMENT: return "FragmentFunction";
	default: GUARANTEE_OR_DIE(false, "Unknown shader stage");
	}
}

static char const* GetShaderModelForStage(eShaderStage stage)
{
	switch (stage)
	{
	case SHADER_STAGE_VERTEX: return "vs_5_0";
	case SHADER_STAGE_FRAGMENT: return "ps_5_0";
	default: GUARANTEE_OR_DIE(false, "Unknown shader stage");
	}
}

static ID3D10Blob* CompileHLSLToByteCode(char const* opt_filename, // optional: used for error messages
                                         void const* source_code, // buffer containing source code.
                                         size_t const source_code_size, // size of the above buffer.
                                         char const* entrypoint,
                                         // Name of the Function we treat as the entry point for this stage
                                         char const* target)
// What stage we're compiling for (Vertex/Fragment/Hull/Compute... etc...)
{
	/* DEFINE MACROS - CONTROLS SHADER
	// You can optionally define macros to control compilation (for instance, DEBUG builds, disabling lighting, etc...)
	D3D_SHADER_MACRO defines[1];
	defines[0].Name = "TEST_MACRO";
	defines[0].Definition = nullptr;
	*/

	DWORD compile_flags = 0U;
#if defined(DEBUG_SHADERS)
	compile_flags |= D3DCOMPILE_DEBUG;
	compile_flags |= D3DCOMPILE_SKIP_OPTIMIZATION;
	compile_flags |= D3DCOMPILE_WARNINGS_ARE_ERRORS;   // cause, FIX YOUR WARNINGS
#else
	// compile_flags |= D3DCOMPILE_SKIP_VALIDATION;       // Only do this if you know for a fact this shader works with this device (so second run through of a game)
	compile_flags |= D3DCOMPILE_OPTIMIZATION_LEVEL3; // Yay, fastness (default is level 1)
#endif

	ID3D10Blob* code = nullptr;
	ID3D10Blob* errors = nullptr;

	HRESULT hr = D3DCompile(source_code,
	                        source_code_size, // plain text source code
	                        opt_filename,
	                        // optional, used for error messages (If you HLSL has includes - it will not use the includes names, it will use this name)
	                        nullptr,
	                        // pre-compiler defines - used more for compiling multiple versions of a single shader (different quality specs, or shaders that are mostly the same outside some constants)
	                        D3D_COMPILE_STANDARD_FILE_INCLUDE,
	                        // include rules - this allows #includes in the shader to work relative to the src_file path or my current working directly
	                        entrypoint, // Entry Point for this shader
	                        target, // Compile Target (MSDN - "Specifying Compiler Targets")
	                        compile_flags, // Flags that control compilation
	                        0, // Effect Flags (we will not be doing Effect Files)
	                        &code, // [OUT] ID3DBlob (buffer) that will store the byte code.
	                        &errors); // [OUT] ID3DBlob (buffer) that will store error information

	if (FAILED(hr) || (errors != nullptr))
	{
		if (errors != nullptr)
		{
			char* error_string = (char*)(errors->GetBufferPointer());
			ERROR_RECOVERABLE( Stringf("Failed to compile [%s].  Compiler gave the following output;\n%s",
				opt_filename,
				error_string) );
			DX_SAFE_RELEASE(errors);
		}
		else
		{
			ERROR_AND_DIE( Stringf( "Failed with HRESULT: %u", hr ));
		}
	}

	// will be nullptr if it failed to compile
	return code;
}

ShaderStage::ShaderStage(): m_handle(nullptr)
{
}

ShaderStage::~ShaderStage()
{
	// cleanup the D3D Resource
	DX_SAFE_RELEASE(m_handle);
	DX_SAFE_RELEASE(m_bytecode);
}

bool ShaderStage::LoadShaderFromSource(RenderContext* ctx, std::string const& filename, void* source, size_t source_len,
                                       eShaderStage stage)
{
	// The devie is what is used to 
	// create resources, so lets cache it off
	// for use in this method; 
	ID3D11Device* device = ctx->m_device;

	// save the stage; 
	m_stage = stage;

	// entry point is what function to call for this stage
	char const* entrypoint = GetEntryForStage(stage);

	// target is the shader model to compile against (determines feature set)
	// we'll be using shader model 5.0 in this class; 
	char const* target = GetShaderModelForStage(stage);

	// take the source and convert it to byte code;
	// note: this byte code can be saved to disk and loaded from
	// instead of having to recompile the shader - this is 
	// usually done during shader-caching (or done at ship for D3D titles)

	m_bytecode = CompileHLSLToByteCode(filename.c_str(), source, source_len, entrypoint, target);
	if (m_bytecode == nullptr)
	{
		return false;
	}

	// based on the stage; create the appropriate D3D object; 
	switch (stage)
	{
	case SHADER_STAGE_VERTEX: // Compile the byte code to the final shader (driver/hardware specific program)
		device->CreateVertexShader(m_bytecode->GetBufferPointer(),
		                           m_bytecode->GetBufferSize(),
		                           nullptr,
		                           &m_vs);
		break;

	case SHADER_STAGE_FRAGMENT:
		device->CreatePixelShader(m_bytecode->GetBufferPointer(),
		                          m_bytecode->GetBufferSize(),
		                          nullptr,
		                          &m_ps);
		break;
	}

	// we're done with the byte code.
	// later, when doing vertex buffers, we'll need
	// to keep the bytecode of the vertex shader around
	//DX_SAFE_RELEASE(bytecode); 

	// if we have a handle, this succeeded; 
	return IsValid();
}

static unsigned int num_shader = 0;

Shader::Shader(RenderContext* rc)
{
	m_renderContext = rc;
	num_shader++;
}

Shader::Shader(Shader* copy)
{
	num_shader++;
	m_renderContext				= copy->m_renderContext;

	m_vertexShader				= copy->m_vertexShader;
	m_vertexShader.m_handle->AddRef();
	m_pixelShader				= copy->m_pixelShader;
	m_pixelShader.m_handle->AddRef();

	m_blendMode					= copy->m_blendMode;
	m_depthCompareOp			= copy->m_depthCompareOp;    
	m_writeDepth				= copy->m_writeDepth;
	m_blendStateDirty			= copy->m_blendStateDirty;
	m_depthStateDirty			= copy->m_depthStateDirty;  
	m_depthStencilState			= copy->m_depthStencilState;
	m_inputLayout				= copy->m_inputLayout;
	m_blendState				= copy->m_blendState;

	m_fillMode					= copy->m_fillMode;
	m_cullMode					= copy->m_cullMode;
	m_frontCounterClockwise		= copy->m_frontCounterClockwise;
	m_rasterStateDirty			= copy->m_rasterStateDirty;
}

Shader::~Shader()
{
	DX_SAFE_RELEASE(m_inputLayout);
	DX_SAFE_RELEASE(m_blendState);
	DX_SAFE_RELEASE(m_depthStencilState);
	DX_SAFE_RELEASE(m_rasterState);
}

bool Shader::CreateInputLayoutFromBufferAttribute(const BufferAttributeT layout[])
{
	// Early out - we've already created it; 
	// TODO: If vertex type changes, we need to rebind; 
	if (m_inputLayout != nullptr)
	{
		return true;
	}

	// This describes the input data to the shader
	// The INPUT_ELEMENT_DESC describes each element of the structure; 
	// Since we have POSITION, COLOR, UV, we need three descriptions; 
	std::vector<D3D11_INPUT_ELEMENT_DESC> input_description;


	// initialize meomry to 0 (usually a sane default)
	//memset(&input_description., 0, sizeof(input_description));

	int desc_id = 0;
	while(!layout[desc_id].IsNull())
	{
		input_description.emplace_back();
		input_description[desc_id].SemanticName = layout[desc_id].m_name.c_str(); // __semantic__ name we gave this input -> float3 pos : POSITION; 
		input_description[desc_id].SemanticIndex = 0;
		// Semantics that share a name (or are large) are spread over multiple indices (matrix4x4s are four floats for instance)
		input_description[desc_id].Format = static_cast<DXGI_FORMAT>(layout[desc_id].GetFormatType()); // Type this data is (float3/vec3 - so 3 floats)
		input_description[desc_id].InputSlot = 0U; // Input Pipe this comes from (ignored unless doing instanced rendering)
		input_description[desc_id].AlignedByteOffset = static_cast<unsigned int>(layout[desc_id].m_memberOffset);
		// memory offset this data starts (where is position relative to the vertex, 0 in this case)
		input_description[desc_id].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		// What is this data for (per vertex data is the only one we'll be dealing with in this class)
		input_description[desc_id].InstanceDataStepRate = 0U;
		// If this were instance data - how often do we step it (0 for vertex data)
		++desc_id;
	}

	ID3D10Blob* vs_bytecode = m_vertexShader.m_bytecode;

	// Final create the layout
	HRESULT hr = m_renderContext->m_device->CreateInputLayout(&input_description[0],
															static_cast<unsigned int>(input_description.size()),
															vs_bytecode->GetBufferPointer(),
															vs_bytecode->GetBufferSize(),
															&m_inputLayout);

	return SUCCEEDED(hr);
}

bool Shader::UpdateBlendStateIfDirty()
{
	// if we already have a good blend state - keep it; 
	if ((m_blendState != nullptr) && (false == m_blendStateDirty)) {
		return true; 
	}

	// Free old state
	DX_SAFE_RELEASE(m_blendState); 

	// otherwise, let's make one; 
	D3D11_BLEND_DESC desc;
	memset( &desc, 0, sizeof(D3D11_BLEND_DESC) );

	desc.AlphaToCoverageEnable = false;  // used in MSAA to treat alpha as coverage (usual example is foliage rendering, we will not be using this)
	desc.IndependentBlendEnable = false;   // if you have multiple outputs bound, you can set this to true to have different blend state per output

										 // Blending is setting put the equation...
										 // FinalColor = BlendOp( SrcFactor * outputColor, DestFactor * destColor )
										 // where outputColor is what the pixel shader outputs
										 // and destColor is the color already in the pixel shader

										 // the below describes the equation...
										 // FinalColor = outputColor.a * outputColor + (1.0f - outputColor.a) * destColor;  

										 // since we disabled independent blend, we only have to setup the first blend state
										 // and I'm setting it up for "alpha blending"
	desc.RenderTarget[0].BlendEnable = TRUE;  // we want to blend

	if (m_blendMode == BLEND_MODE_ALPHA) {
		desc.RenderTarget[0].SrcBlend    = D3D11_BLEND_SRC_ALPHA;      // output color is multiplied by the output colors alpha and added to...
		desc.RenderTarget[0].DestBlend   = D3D11_BLEND_INV_SRC_ALPHA;  // the current destination multiplied by (1 - output.a); 
		desc.RenderTarget[0].BlendOp     = D3D11_BLEND_OP_ADD;        // we add the two results together

																	   // you can compute alpha seperately, in this case, we'll just set it to be the max alpha between the src & destination
		desc.RenderTarget[0].SrcBlendAlpha  = D3D11_BLEND_ONE;
		desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE; 
		desc.RenderTarget[0].BlendOpAlpha   = D3D11_BLEND_OP_MAX;
	} else if (m_blendMode == BLEND_MODE_ADDTIVE){ 
		desc.RenderTarget[0].SrcBlend    = D3D11_BLEND_ONE;      
		desc.RenderTarget[0].DestBlend   = D3D11_BLEND_ONE;  
		desc.RenderTarget[0].BlendOp     = D3D11_BLEND_OP_ADD;   

																
		desc.RenderTarget[0].SrcBlendAlpha  = D3D11_BLEND_ONE;
		desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE; 
		desc.RenderTarget[0].BlendOpAlpha   = D3D11_BLEND_OP_MAX;
	} else if (m_blendMode == BLEND_MODE_OPAQUE){ 
		desc.RenderTarget[0].SrcBlend    = D3D11_BLEND_ZERO;      
		desc.RenderTarget[0].DestBlend   = D3D11_BLEND_SRC_COLOR;  
		desc.RenderTarget[0].BlendOp     = D3D11_BLEND_OP_ADD;   


		desc.RenderTarget[0].SrcBlendAlpha  = D3D11_BLEND_ONE;
		desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE; 
		desc.RenderTarget[0].BlendOpAlpha   = D3D11_BLEND_OP_MAX;
	};

	desc.RenderTarget[0].RenderTargetWriteMask       = D3D11_COLOR_WRITE_ENABLE_ALL;  // can mask off outputs;  we won't be doing that; 

																	  // Finally, create the blend state
	//ASSERT_OR_DIE(false, " Need to CreateBlendState( desc, &m_blendState )");
	m_renderContext->m_device->CreateBlendState( &desc, &m_blendState );

	m_blendStateDirty = false; 
	return (m_blendState != nullptr); 
}

bool Shader::UpdateDepthStatesIfDirty()
{

	if (m_depthStateDirty || (m_depthStencilState == nullptr)) {
		D3D11_DEPTH_STENCIL_DESC ds_desc = {};

		ds_desc.DepthEnable = TRUE;  // for simplicity, just set to true (could set to false if write is false and comprae is always)
		ds_desc.DepthWriteMask = m_writeDepth ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO; 
		ds_desc.DepthFunc = D3D11_COMPARISON_FUNC( m_depthCompareOp ); //  

																  // Stencil - just use defaults for now; 
		ds_desc.StencilEnable = false; 
		ds_desc.StencilReadMask = 0xFF; 
		ds_desc.StencilWriteMask = 0xFF; 

		D3D11_DEPTH_STENCILOP_DESC default_stencil_op = {}; 
		default_stencil_op.StencilFailOp = D3D11_STENCIL_OP_KEEP;      // what to do if stencil fails
		default_stencil_op.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP; // What to do if stencil succeeds but depth fails
		default_stencil_op.StencilPassOp = D3D11_STENCIL_OP_KEEP;      // what to do if the stencil succeeds
		default_stencil_op.StencilFunc = D3D11_COMPARISON_ALWAYS;      // function to test against

																	   // can have different rules setup for front and backface
		ds_desc.FrontFace = default_stencil_op; 
		ds_desc.BackFace = default_stencil_op; 

		DX_SAFE_RELEASE(m_depthStencilState); 
		m_renderContext->m_device->CreateDepthStencilState( &ds_desc, &m_depthStencilState ); 
		m_depthStateDirty = false;
	}
	return true;
}

bool Shader::UpdateRasterStateIfDirty()
{
	if(m_rasterStateDirty)
	{
		D3D11_RASTERIZER_DESC raster_desc;

		raster_desc.FillMode = DXGetFillMode( m_fillMode );
		raster_desc.CullMode = DXGetCullMode( m_cullMode );
		raster_desc.FrontCounterClockwise = m_frontCounterClockwise;

		raster_desc.DepthBias = 0U;
		raster_desc.DepthBiasClamp = 0.0f;
		raster_desc.SlopeScaledDepthBias = 0.0f;
		raster_desc.DepthClipEnable = TRUE;
		raster_desc.ScissorEnable = FALSE;
		raster_desc.MultisampleEnable = FALSE;
		raster_desc.AntialiasedLineEnable = FALSE;

		DX_SAFE_RELEASE( m_rasterState );
		m_renderContext->m_device->CreateRasterizerState( &raster_desc, &m_rasterState );

		m_rasterStateDirty = false;
	}
	return true;
}

void Shader::SetBlendMode(eBlendMode mode)
{
	m_blendMode = mode;
	m_blendStateDirty = true;
}

void Shader::SetDepth(eCompareOp op, bool write)
{
	if(m_depthCompareOp != op || m_writeDepth != write){
		m_depthCompareOp = op;
		m_writeDepth = write;
		m_depthStateDirty = true;
	}
}

void Shader::SetRasterState(eFillMode fill_mode, eCullMode cull_mode, bool is_front_counter_clockwise)
{
	if(m_fillMode != fill_mode || m_cullMode != cull_mode || m_frontCounterClockwise != is_front_counter_clockwise)
	{
		m_fillMode = fill_mode;
		m_cullMode = cull_mode;
		m_frontCounterClockwise = is_front_counter_clockwise;
		m_rasterStateDirty = true;
	}
}

D3D11_FILL_MODE Shader::DXGetFillMode(eFillMode mode)
{
	switch(mode)
	{
	case NO_FILL:
		{
		ERROR_RECOVERABLE("Using wrong fill mode, defaulting to SOLID");
		return D3D11_FILL_SOLID;
		}
	case WIREFRAME:
		{
			return D3D11_FILL_WIREFRAME;
		}
	case SOLID:
		{
			return D3D11_FILL_SOLID;
		}
	case NUM_FILL_MODES:
	default:
		{
			ERROR_RECOVERABLE("Using wrong fill mode, defaulting to SOLID");
			return D3D11_FILL_SOLID;
		}
	}
}

D3D11_CULL_MODE Shader::DXGetCullMode(eCullMode mode)
{
	switch(mode)
	{
	case NO_CULL:
	{
		return D3D11_CULL_NONE;
	}
	case FRONT_CULL:
	{
		return D3D11_CULL_FRONT;
	}
	case BACK_CULL:
	{
		return D3D11_CULL_BACK;
	}
	case NUM_CULL_MODES:
	default:
	{
		ERROR_RECOVERABLE("Using wrong cull mode, defaulting to BACK_CULL");
		return D3D11_CULL_BACK;
	}
	}
}
