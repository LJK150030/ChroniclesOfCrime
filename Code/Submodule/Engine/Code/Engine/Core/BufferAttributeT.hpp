#pragma once
#include <string>

enum eRenderDataFormat
{
	RDF_NULL, 

	RDF_FLOAT, 
	RDF_VEC2, 
	RDF_VEC3, 
	RDF_RGBA32,
	RDF_VEC4
};

struct BufferAttributeT
{
public:
	std::string m_name = ""; 
	eRenderDataFormat m_format = RDF_NULL; 
	size_t m_memberOffset = 0;

public:
	BufferAttributeT();
	~BufferAttributeT();
	explicit BufferAttributeT(std::string name, eRenderDataFormat format, size_t offset);
	inline bool IsNull() const { return m_format == RDF_NULL; }
	int GetFormatType( ) const;
};