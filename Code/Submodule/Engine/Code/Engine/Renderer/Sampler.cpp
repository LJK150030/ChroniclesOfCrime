//------------------------------------------------------------------------
// Sampler.cpp
//------------------------------------------------------------------------
#include "Engine/Renderer/Sampler.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"


//------------------------------------------------------------------------
static D3D11_FILTER DXGetFilter(eFilterMode min, eFilterMode mag)
{
	if (min == FILTER_MODE_POINT)
	{
		switch (mag)
		{
		case FILTER_MODE_POINT: return D3D11_FILTER_MIN_MAG_MIP_POINT;
		case FILTER_MODE_LINEAR: return D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
		}
	}
	else
	{
		switch (mag)
		{
		case FILTER_MODE_POINT: return D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT;
		case FILTER_MODE_LINEAR: return D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
		}
	}

	ASSERT_RECOVERABLE(false, "A DXGetFilter combination was not handled"); // this combination was not handled?  
	return D3D11_FILTER_MIN_MAG_MIP_POINT;
}

Sampler::Sampler()
{
}

Sampler::~Sampler()
{
	DX_SAFE_RELEASE(m_handle);
}

//------------------------------------------------------------------------
void Sampler::CreateStateIfDirty(RenderContext* ctx)
{
	// no changes needed
	if (!m_isDirty)
	{
		return;
	}

	// Release old state; 
	DX_SAFE_RELEASE( m_handle );

	// Create new state
	D3D11_SAMPLER_DESC desc;
	memset(&desc, 0, sizeof(D3D11_SAMPLER_DESC));

	desc.Filter = DXGetFilter(m_minFilter, m_magFilter);
	desc.MaxAnisotropy = 1U; // anistropic filtering (we're not using this... yet)

	// set texture to wrap on UV, but clamp on W (mip).  
	// (if you're smapling 3D wrapping noise, you may want wrap in all dimensions)
	// (if you're doing single sprites, you may want clamp all so you don't get edge bleeding)
	desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

	// set tha tcan can sample from all mip levels; 
	desc.MinLOD = -FLT_MAX;
	desc.MaxLOD = FLT_MAX;
	desc.MipLODBias = 0.0f; // will shift the mip level for a sample by this

	desc.ComparisonFunc = D3D11_COMPARISON_NEVER; // will come into play doing shadow maps; 

	// leaving border color black (only useful if our wrap mode is BORDER

	// Create!
	ctx->m_device->CreateSamplerState(&desc, &m_handle);

	m_isDirty = false;
	ASSERT_OR_DIE( m_handle != nullptr, "handle was null, check your output" );
	// this should honestly never fail.  If it does - check your output!
};
