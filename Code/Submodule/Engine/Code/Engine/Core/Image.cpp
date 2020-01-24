#include "Engine/Core/Image.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/Time.hpp"

#pragma warning (disable: 4100 ) // unreferenced formal parameter
#define STB_IMAGE_IMPLEMENTATION
#include "ThirdParty/stb/stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "ThirdParty/stb/stb_write.h"

Image::Image()
{
	m_imageFilePath = "";
	m_texels.push_back(Rgba::WHITE);
	m_dimensions = IntVec2::ONE;
	m_imageData = new unsigned char[4];
	m_imageData[0] = static_cast<unsigned char>(-1); //setting each bit to the highest char value
	m_imageData[1] = static_cast<unsigned char>(-1);
	m_imageData[2] = static_cast<unsigned char>(-1);
	m_imageData[3] = static_cast<unsigned char>(-1);
}

Image::Image( const IntVec2& size )
{
	m_dimensions.x = size.x;
	m_dimensions.y = size.y;

	int num_texels = size.x * size.y;

	m_imageData = new unsigned char[num_texels * 4];
	m_texels.resize(num_texels);

	for(int texel_idx = 0; texel_idx < num_texels; ++texel_idx)
	{
		m_texels[texel_idx].SetFromByte(255,255,255,255);

		m_imageData[texel_idx] = 255;
		m_imageData[texel_idx + 1] = 255;
		m_imageData[texel_idx + 2] = 255;
		m_imageData[texel_idx + 3] = 255;
	}
	
}

Image::Image(const char* image_file_path)
{
	int image_texel_size_x = 0; // Filled in for us to indicate image width
	int image_texel_size_y = 0; // Filled in for us to indicate image height
	int num_components = 0;
	// Filled in for us to indicate how many color components the image had (e.g. 3=RGB=24bit, 4=RGBA=32bit)
	int num_components_requested = 4; // don't care; we support 3 (24-bit RGB) or 4 (32-bit RGBA)

	// Load (and decompress) the image RGB(A) bytes from a file on disk into a memory buffer (array of bytes)
	//stbi_set_flip_vertically_on_load(FLIP_TEXTURE_VERTICALLY); // We prefer uvTexCoords has origin (0,0) at BOTTOM LEFT
	m_imageData = stbi_load(image_file_path, &image_texel_size_x, &image_texel_size_y, &num_components,
	                                      num_components_requested);

	// Check if the load was successful
	if(!m_imageData) return;

	//GUARANTEE_OR_DIE( m_imageData, Stringf( "Failed to load image \"%s\"", image_file_path ) );
	GUARANTEE_OR_DIE( num_components >= 3 && num_components <= 4 && image_texel_size_x > 0 && image_texel_size_y > 0,
		Stringf(
			"ERROR loading image \"%s\" (Bpp=%i, size=%i,%i)", image_file_path, num_components, image_texel_size_x,
			image_texel_size_y ) );

	m_dimensions = IntVec2(image_texel_size_x, image_texel_size_y);
	const int num_texels = image_texel_size_x * image_texel_size_y;
	m_texels.resize(num_texels);
	for (int texel_id = 0; texel_id < num_texels; ++ texel_id)
	{
		const int red_byte_index = (texel_id * num_components);
		const int green_byte_index = red_byte_index + 1;
		const int blue_byte_index = red_byte_index + 2;
		const int alpha_byte_index = red_byte_index + 3; //Maybe????

		const unsigned char red_byte = m_imageData[red_byte_index];
		const unsigned char green_byte = m_imageData[green_byte_index];
		const unsigned char blue_byte = m_imageData[blue_byte_index];
		unsigned char alpha_byte = 255;

		if (num_components == 4)
			alpha_byte = m_imageData[alpha_byte_index];

		m_texels[texel_id].SetFromByte(red_byte, green_byte, blue_byte, alpha_byte);
	}
}

Image::~Image()
{
	stbi_image_free(m_imageData);
}

Rgba Image::GetTexelColor(const IntVec2& texel_coords) const
{
	const int texel_index = texel_coords.y * m_dimensions.x + texel_coords.x;
	return Rgba(m_texels[texel_index].r, m_texels[texel_index].g, m_texels[texel_index].b, m_texels[texel_index].a);
}

void Image::SetTexelColor(const IntVec2& texel_coords, const Rgba& color)
{
	const int texel_index = texel_coords.y * m_dimensions.x + texel_coords.x;
	m_texels[texel_index].r = color.r;
	m_texels[texel_index].g = color.g;
	m_texels[texel_index].b = color.b;
	m_texels[texel_index].a = color.a;

	const int red_byte_index = (texel_index * 4);
	const int green_byte_index = (texel_index * 4) + 1;
	const int blue_byte_index = (texel_index * 4) + 2;
	const int alpha_byte_index = (texel_index * 4) + 3; //Maybe???? the image we are using could just only be an RGB not a RGBA

	m_imageData[red_byte_index] = static_cast<const unsigned char>(color.r * 255.0f);
	m_imageData[green_byte_index] = static_cast<const unsigned char>(color.g * 255.0f);
	m_imageData[blue_byte_index] = static_cast<const unsigned char>(color.b * 255.0f);
	m_imageData[alpha_byte_index] = static_cast<const unsigned char>(color.a * 255.0f);
}

IntVec2 Image::GetDimensions() const
{
	return m_dimensions;
}

void Image::SaveImage(std::string path)
{
	stbi_write_png(path.c_str(), m_dimensions.x, m_dimensions.y, 4, m_imageData, m_dimensions.x * 4);
}
