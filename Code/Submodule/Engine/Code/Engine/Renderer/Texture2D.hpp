#pragma once
//------------------------------------------------------------------------
// Texture2D.hpp
//------------------------------------------------------------------------
#include "Engine/Renderer/texture.hpp"
#include "Engine/Renderer/DepthStencilTargetView.hpp"

class Image;

//------------------------------------------------------------------------
class Texture2D : public Texture
{
public:
	Texture2D(RenderContext* ctx); // texures always come from a context; 
	virtual ~Texture2D();

	bool LoadFromFile(std::string const& filename);
	bool LoadTextureFromImage(Image const& image);

	// Create a view of this texture usable in the shader; 
	TextureView2D* CreateTextureView2D() const;
	
	bool CreateDepthStencilTarget( unsigned int widht, unsigned int height ); 
	DepthStencilTargetView* CreateDepthStencilTargetView();

	bool CreateColorTarget( unsigned int widht, unsigned int height ); 
	ColorTargetView* CreateColorTargetView() const;

	static Texture2D* CreateDepthStencilTarget( RenderContext *ctx, unsigned int widht, unsigned int height );  
	static Texture2D* CreateDepthStencilTargetFor( Texture2D *colorTarget );

	// Currently do now have an enum for different file formats (RGBA8, R8, D24S8, etc..), assuming only RGBA8
	//static Texture2D* CreateColorTarget( RenderContext *ctx, unsigned int width, unsigned int height, eImageFormat format );
	static Texture2D* CreateColorTarget( RenderContext *ctx, unsigned int width, unsigned int height );
	static Texture2D* CreateMatchingColorTarget( Texture2D *other );  // A10

	void MakeImage(Image& out_image );
	
public:
	IntVec2 m_dimensions;
};
