#pragma once
#include "Engine/Renderer/Material.hpp"
#include "Engine/Math/Matrix44.hpp"

class CPUMesh;
class GPUMesh;
class RenderContext;

class Model          // A09
{
public: 
	Model( RenderContext *ctx, char const *mesh_name );
	Model( RenderContext *ctx, CPUMesh* mesh_name, const std::string& file_name);
	explicit Model( GPUMesh *mesh );
	~Model();

public:
	GPUMesh *m_mesh; 
	Material *m_material; 
	Matrix44 m_modelMatrix;  
};
