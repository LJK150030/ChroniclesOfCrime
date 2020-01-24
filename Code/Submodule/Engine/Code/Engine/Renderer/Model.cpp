#include "Engine/Renderer/Model.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/GPUMesh.hpp"

Model::Model(RenderContext* ctx, char const* mesh_name)
{
	m_mesh = ctx->CreateOrGetMesh(mesh_name);

	if (m_mesh != nullptr) {
		m_material = ctx->CreateOrGetMaterial( m_mesh->GetDefaultMaterialName() ); 
	}

	m_modelMatrix = Matrix44::IDENTITY;
}

Model::Model(RenderContext* ctx, CPUMesh* mesh_name, const std::string& file_name)
{
	m_mesh = ctx->CreateGPUMeshFromFile(*mesh_name, file_name);

	if (m_mesh != nullptr) {
		m_material = ctx->CreateOrGetMaterial( m_mesh->GetDefaultMaterialName() ); 
	}

	m_modelMatrix = Matrix44::IDENTITY;
}

Model::Model(GPUMesh* mesh)
{
	m_mesh = mesh;
	m_modelMatrix = Matrix44::IDENTITY;

}

Model::~Model() = default;
