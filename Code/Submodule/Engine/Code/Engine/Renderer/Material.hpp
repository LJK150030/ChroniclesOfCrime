#pragma once
#include "ThirdParty/TinyXML2/tinyxml2.h"
#include <vector>

class UniformBuffer;
class Sampler;
class TextureView;
class Shader;
class RenderContext;

enum eTextureMaps
{
	UNKNOWN_MAP = -1,
	TEXTURE_MAP_DIFFUSE, // Color and Albedo // default to "white" on nullptr
	TEXTURE_MAP_NORMAL, // default to "flat" on nullptr; {128 128 255} 
	TEXTURE_MAP_EMISSIVE, // default to "black" on nullptr
	TEXTURE_MAP_SPECULAR, // default to "white" on nullptr
	TEXTURE_MAP_AMBIENT_OCCLUSION, // default to "gray" on nullptr
	TEXTURE_MAP_BUMP, // default to "gray" on nullptr
	NUM_TEXTURE_MAPS
};

class Material
{
public:

	Material( RenderContext* ctx ); 
	Material( RenderContext* ctx, tinyxml2::XMLElement* xml ); 
	~Material();

	void SetShader( Shader* shader ); 
	void SetShader( char const* shader_name ); 

	void SetTextureView( const eTextureMaps slot, TextureView* view ); 
	void SetTextureView( const eTextureMaps slot, char const* name );

	void SetSampler( const eTextureMaps slot, Sampler* sampler );
	

	TextureView* GetTextureView( const eTextureMaps slot ) const; 
	Sampler* GetSampler( const eTextureMaps slot ) const; 

	// Convenience Helpers - meaning it can pick a sane default when set to nullptr;
 	void SetDiffuseMap( TextureView* view );  
 	void SetNormalMap( TextureView* view );   
 	void SetSpecularMap( TextureView* view );
 	void SetEmissiveMap( TextureView* view ); 
	void SetAmbientOcclusionMap( TextureView* view );
 	void SetBumpMap( TextureView* view ); ;

	void SetUniforms( void const* data, size_t const size );
	void SetUniforms( UniformBuffer* ubo ); 
	UniformBuffer* GetUniformBuffer() const;

	// templated SetUniforms could be useful as well
	// ...

public:
	RenderContext *m_renderContext;

	Shader *m_shader; 
	std::vector<TextureView*> m_textures; 
	std::vector<Sampler*> m_samplers; 

	UniformBuffer *m_materialBuffer     = nullptr;

};
