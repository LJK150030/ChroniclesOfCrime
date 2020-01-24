#include "Engine/Renderer/Material.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/UniformBuffer.hpp"
#include "Engine/Renderer/Texture2D.hpp"
#include "Engine/Renderer/TextureView2D.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/Image.hpp"
#include "Engine/Renderer/Sampler.hpp"

Material::Material(RenderContext* ctx)
{
	m_renderContext = ctx;
	m_textures.resize(NUM_TEXTURE_MAPS);
	SetTextureView(TEXTURE_MAP_DIFFUSE, "white");
	SetTextureView(TEXTURE_MAP_NORMAL, "flat");
	SetTextureView(TEXTURE_MAP_EMISSIVE, "black");
	SetTextureView(TEXTURE_MAP_SPECULAR, "white");
	SetTextureView(TEXTURE_MAP_AMBIENT_OCCLUSION, "gray");
	SetTextureView(TEXTURE_MAP_BUMP, "gray");
	m_samplers.resize(NUM_TEXTURE_MAPS);
	SetSampler(TEXTURE_MAP_DIFFUSE, nullptr);
	SetSampler(TEXTURE_MAP_NORMAL, nullptr);
	SetSampler(TEXTURE_MAP_EMISSIVE, nullptr);
	SetSampler(TEXTURE_MAP_SPECULAR, nullptr);
	SetSampler(TEXTURE_MAP_AMBIENT_OCCLUSION, nullptr);
	SetSampler(TEXTURE_MAP_BUMP, nullptr);
}

Material::Material(RenderContext* ctx, tinyxml2::XMLElement* xml)
{
	m_renderContext = ctx;
	m_textures.resize(NUM_TEXTURE_MAPS);
	m_samplers.resize(NUM_TEXTURE_MAPS);

	//parsing the xml file and setting member variables
	// only assumes there is a root element, and one level of children elements
	// with each element, we know what the attributes there are, and can immediately parse and set members
	// very hard code style TODO: see if we can make this modular
	
	for(const tinyxml2::XMLElement* pass = xml->FirstChildElement(); pass; 
		pass = pass->NextSiblingElement())
	{
		std::string pass_name = pass->Name();

		NamedStrings element;
		element.PopulateFromXmlElementAttributes(*pass);

		if(pass_name == "shader") // src="default_unlit.hlsl"
		{
			std::string shader_file_name = element.GetValue("src", "default_unlit.hlsl");
			SetShader(shader_file_name.c_str());
		}
		else if(pass_name == "texture") // either slot and color, or slot and 
		{
			if(element.HasKey("color")) //  index="0" color="0xrrggbbaa"
			{
				int slot = element.GetValue("index", 0);
				if(slot >= NUM_TEXTURE_MAPS) slot = 0;
				std::string color_hex = element.GetValue("color", "0xffffffff");
				SetTextureView( static_cast<eTextureMaps>(slot), m_renderContext->CreateOrGetTextureView2D(color_hex));

				//moved this to render context because we were creating a texture without render context knowing
// 				Rgba color;
// 				color.SetFromHex(color_hex);
// 
// 				Image image;
// 				image.SetTexelColor(IntVec2::ZERO, color);
// 				Texture2D* color_texture = new Texture2D(m_renderContext);
// 				color_texture->LoadTextureFromImage(image);
// 				SetTextureView( static_cast<eTextureMaps>(slot), reinterpret_cast<TextureView*>(color_texture->CreateTextureView2D()));
// 				delete color_texture;
// 				color_texture = nullptr;
			}
			else // index="1" src="flat"
			{
				int slot = element.GetValue("index", 0);
				std::string texture_file_name = element.GetValue("src", "white");
				SetTextureView(static_cast<eTextureMaps>(slot), texture_file_name.c_str());
			}
		}
		else if(pass_name == "sampler")
		{
			int slot = element.GetValue("index", 0);
			std::string texture_file_name = element.GetValue("name", "linear");
			Sampler* sampler(m_renderContext->CreateOrGetSampler(texture_file_name));
			SetSampler(static_cast<eTextureMaps>(slot), sampler);
		}
		else if(pass_name == "diffuse")
		{
			std::string texture_src = element.GetValue("src", "Test_StbiFlippedAndOpenGL.png");
			TextureView* texture(reinterpret_cast<TextureView*>(m_renderContext->CreateOrGetTextureView2D(texture_src)));
			SetDiffuseMap(texture);
		}
		else if(pass_name == "normal")
		{
			std::string texture_src = element.GetValue("src", "Test_StbiFlippedAndOpenGL.png");
			TextureView* texture(reinterpret_cast<TextureView*>(m_renderContext->CreateOrGetTextureView2D(texture_src)));
			SetNormalMap(texture);
		}
		else if(pass_name == "specular")
		{
			std::string texture_src = element.GetValue("src", "Test_StbiFlippedAndOpenGL.png");
			TextureView* texture(reinterpret_cast<TextureView*>(m_renderContext->CreateOrGetTextureView2D(texture_src)));
			SetSpecularMap(texture);
		}
		else if(pass_name == "emissive")
		{
			std::string texture_src = element.GetValue("src", "Test_StbiFlippedAndOpenGL.png");
			TextureView* texture(reinterpret_cast<TextureView*>(m_renderContext->CreateOrGetTextureView2D(texture_src)));
			SetEmissiveMap(texture);
		}
		else if(pass_name == "ambientocclusion")
		{
			std::string texture_src = element.GetValue("src", "Test_StbiFlippedAndOpenGL.png");
			TextureView* texture(reinterpret_cast<TextureView*>(m_renderContext->CreateOrGetTextureView2D(texture_src)));
			SetAmbientOcclusionMap(texture);
		}
		else if(pass_name == "bump")
		{
			std::string texture_src = element.GetValue("src", "Test_StbiFlippedAndOpenGL.png");
			TextureView* texture(reinterpret_cast<TextureView*>(m_renderContext->CreateOrGetTextureView2D(texture_src)));
			SetBumpMap(texture);
		}
	}

}

Material::~Material()
{
	//m_textures is refrencing textures that the render context is loading
	//m_samplers is refrencing textures that the render context is loading
}

void Material::SetShader(Shader* shader)
{
	m_shader = shader;
}

void Material::SetShader(char const* shader_name)
{
	m_shader = m_renderContext->CreateOrGetShader(shader_name);
}

void Material::SetTextureView(const eTextureMaps slot, TextureView* view)
{
	//m_renderContext->BindTextureView(slot, view);
	m_textures[slot] = view;
}

void Material::SetTextureView(const eTextureMaps slot, char const* name)
{
	TextureView2D* temp_texture_view2_d = m_renderContext->CreateOrGetTextureView2D(name);
	m_textures[slot] = reinterpret_cast<TextureView*>(temp_texture_view2_d);
	//m_renderContext->BindTextureView(slot, name);
}

void Material::SetSampler(const eTextureMaps slot, Sampler* sampler)
{
	//m_renderContext->BindSampler(slot, sampler);
	m_samplers[slot] = sampler;
}

TextureView* Material::GetTextureView(const eTextureMaps slot) const
{
	return m_textures[slot];
}

Sampler* Material::GetSampler(const eTextureMaps slot) const
{
	return m_samplers[slot];
}

void Material::SetDiffuseMap(TextureView* view)
{
	if(view == nullptr) SetTextureView(TEXTURE_MAP_DIFFUSE, "white");
	SetTextureView(TEXTURE_MAP_DIFFUSE, view);
}

void Material::SetNormalMap(TextureView* view)
{
	if(view == nullptr) SetTextureView(TEXTURE_MAP_NORMAL, "flat");
	SetTextureView(TEXTURE_MAP_NORMAL, view);
}

void Material::SetEmissiveMap(TextureView* view)
{
	if(view == nullptr) SetTextureView(TEXTURE_MAP_EMISSIVE, "black");
	SetTextureView(TEXTURE_MAP_EMISSIVE, view);
}

void Material::SetSpecularMap(TextureView* view)
{
	if(view == nullptr) SetTextureView(TEXTURE_MAP_SPECULAR, "white");
	SetTextureView(TEXTURE_MAP_SPECULAR, view);
}

void Material::SetAmbientOcclusionMap(TextureView* view)
{
	if(view == nullptr) SetTextureView(TEXTURE_MAP_AMBIENT_OCCLUSION, "gray");
	SetTextureView(TEXTURE_MAP_AMBIENT_OCCLUSION, view);
}

void Material::SetBumpMap(TextureView* view)
{
	if(view == nullptr) SetTextureView(TEXTURE_MAP_BUMP, "gray");
	SetTextureView(TEXTURE_MAP_BUMP, view);
}

void Material::SetUniforms(void const* data, size_t const size)
{
	if(m_materialBuffer == nullptr) m_materialBuffer = new UniformBuffer(m_renderContext);
	m_materialBuffer->CopyCPUToGPU(data, size);
}

void Material::SetUniforms(UniformBuffer* ubo)
{
	m_materialBuffer = ubo;
}

UniformBuffer* Material::GetUniformBuffer() const
{
	return m_materialBuffer;
}
