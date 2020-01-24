#include "Engine/Core/BufferAttributeT.hpp"
#include "ErrorWarningAssert.hpp"
#include <dxgiformat.h>

BufferAttributeT::BufferAttributeT() = default;

BufferAttributeT::~BufferAttributeT() = default;

BufferAttributeT::BufferAttributeT(std::string name, const eRenderDataFormat format, const size_t offset): 
	m_name(std::move(name)), m_format(format), m_memberOffset(offset) 
{
}

int BufferAttributeT::GetFormatType() const
{
	switch(m_format)
	{
		case RDF_FLOAT:
		{
			return DXGI_FORMAT_R32_FLOAT;
		}
		case RDF_VEC2:
		{
			return DXGI_FORMAT_R32G32_FLOAT;
		}
		case RDF_VEC3:
		{
			return DXGI_FORMAT_R32G32B32_FLOAT;
		}
		case RDF_RGBA32:
		{
			return DXGI_FORMAT_R32G32B32A32_FLOAT;
		}
		case RDF_VEC4:
		{
			return DXGI_FORMAT_R32G32B32A32_FLOAT;
		}
		case RDF_NULL:
		default:
		{
			ERROR_AND_DIE("format type is not specified in BufferAttribute");
		}
	}
}
