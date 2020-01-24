#pragma once
#include "Engine/EngineCommon.hpp"
#include <vector>

class Image
{
public:
	unsigned char* m_imageData;

public:
	Image();
	explicit Image( const IntVec2& size );
	explicit Image(const char* image_file_path);
	~Image();
	Rgba GetTexelColor(const IntVec2& texel_coords) const;
	void SetTexelColor(const IntVec2& texel_coords, const Rgba& color);
	IntVec2 GetDimensions() const;
	std::string GetFilePath() const { return m_imageFilePath; }
	unsigned char* GetBuffer() const { return m_imageData; }

	void SaveImage( std::string path );

private:
	std::string m_imageFilePath;
	IntVec2 m_dimensions = IntVec2::ZERO;
	std::vector<Rgba> m_texels;
};
