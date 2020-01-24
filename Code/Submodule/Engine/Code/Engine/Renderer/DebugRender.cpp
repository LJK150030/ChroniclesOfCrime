#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Renderer/TextureView2D.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/GPUMesh.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Math/Capsule3.hpp"
#include "Engine/Math/OBB2.hpp"
#include "Engine/Math/OBB3.hpp"
#include "Engine/Math/Plane3.hpp"
#include "Engine/Math/Matrix44.hpp"
#include "Engine/Tools/Profiler.hpp"

const int STRINGF_STACK_LOCAL_TEMP_LENGTH = 2048;

STATIC bool DebugRender::Show(EventArgs& args)
{
	UNUSED(args);
	DEBUG_RENDER_IN_USE = !DEBUG_RENDER_IN_USE;
	return true;
}

STATIC bool DebugRender::Clear(EventArgs& args)
{
	UNUSED(args);
	DEBUG_RENDER_LOG_CLEAR = true;
	return true;
}

//TODO:!!! there are certain debug render functions that will need the default UV
DebugRender::DebugRender():
	m_defaultTexture(g_theRenderer->CreateOrGetBitmapFont("16x16-Dwarf.png")->GetTexture())
{
	m_fontSize = g_gameConfigBlackboard.GetValue("debugRenderFontSize", m_fontSize);
	m_fontFile = g_gameConfigBlackboard.GetValue("debugRenderFontFile", m_fontFile);
}

DebugRender::~DebugRender()
{
}

void DebugRender::Startup(RenderContext* context, float virtual_screen_height)
{
	m_gameRenderContext = context;

	//setup camera
	m_debugLogCamera = new Camera();
	m_debugLogCamera->SetColorTarget(nullptr);
	m_debugLogCamera->SetOrthoView(Vec2(0.0f, 0.0f), 
		Vec2(virtual_screen_height * 1.7777777777778f, virtual_screen_height));

	g_defaultScreenBoarder = AABB2(0.0f, 0.0f, virtual_screen_height * 1.7777777777778f, virtual_screen_height);

	g_theEventSystem->SubscribeEventCallbackFunction("showDR", Show);
	g_theEventSystem->SubscribeEventCallbackFunction("clearDR", Clear);

	m_alwaysShader = new Shader(g_theRenderer->CreateOrGetShader("default_unlit.hlsl"));
	m_alwaysShader->SetDepth(COMPARE_ALWAYS, true);
	m_alwaysShader->SetBlendMode(BLEND_MODE_ALPHA);
	m_alwaysShader->SetRasterState(SOLID, BACK_CULL, true);

	m_lessEqualShader = new Shader(g_theRenderer->CreateOrGetShader("default_unlit.hlsl"));
	m_lessEqualShader->SetDepth(COMPARE_LESS_EQUAL, true);
	m_lessEqualShader->SetBlendMode(BLEND_MODE_ALPHA);
	m_lessEqualShader->SetRasterState(SOLID, BACK_CULL, true);

	m_greaterShader = new Shader(g_theRenderer->CreateOrGetShader("default_unlit.hlsl"));
	m_greaterShader->SetDepth(COMPARE_GREATER, true);
	m_greaterShader->SetBlendMode(BLEND_MODE_ALPHA);
	m_greaterShader->SetRasterState(SOLID, BACK_CULL, true);

	m_wireframeShader = new Shader(g_theRenderer->CreateOrGetShader("default_unlit.hlsl"));
	m_wireframeShader->SetDepth(COMPARE_ALWAYS, true);
	m_wireframeShader->SetBlendMode(BLEND_MODE_ALPHA);
	m_wireframeShader->SetRasterState(WIREFRAME, BACK_CULL, true);

	m_logMesh = new GPUMesh(g_theRenderer);

	//TODO: need to be able to calculate the coordinate through Bitmapfont
	m_defaultUVBounds = AABB2(0.9375f, 0.9375f, 1.0f, 1.0f);
	// testing helper function
	//DebugRenderMessagef(100.0f, Rgba::CYAN, Rgba::RED, "DebugRender: start up phase");
}

void DebugRender::Startup(RenderContext* context, const Vec2& half_extents)
{
	m_gameRenderContext = context;

	//setup camera
	m_debugLogCamera = new Camera();
	m_debugLogCamera->SetColorTarget(nullptr);
	m_debugLogCamera->SetOrthoView(half_extents * -1.0f, half_extents);
	m_debugLogCamera->SetModelMatrix(Matrix44::IDENTITY);

	g_theEventSystem->SubscribeEventCallbackFunction("showDR", Show);
	g_theEventSystem->SubscribeEventCallbackFunction("clearDR", Clear);

	m_alwaysShader = new Shader(g_theRenderer->CreateOrGetShader("default_unlit.hlsl"));
	m_alwaysShader->SetDepth(COMPARE_ALWAYS, true);
	m_alwaysShader->SetBlendMode(BLEND_MODE_ALPHA);
	m_alwaysShader->SetRasterState(SOLID, BACK_CULL, true);

	m_lessEqualShader = new Shader(g_theRenderer->CreateOrGetShader("default_unlit.hlsl"));
	m_lessEqualShader->SetDepth(COMPARE_LESS_EQUAL, true);
	m_lessEqualShader->SetBlendMode(BLEND_MODE_ALPHA);
	m_lessEqualShader->SetRasterState(SOLID, BACK_CULL, true);

	m_greaterShader = new Shader(g_theRenderer->CreateOrGetShader("default_unlit.hlsl"));
	m_greaterShader->SetDepth(COMPARE_GREATER, true);
	m_greaterShader->SetBlendMode(BLEND_MODE_ALPHA);
	m_greaterShader->SetRasterState(SOLID, BACK_CULL, true);

	m_wireframeShader = new Shader(g_theRenderer->CreateOrGetShader("default_unlit.hlsl"));
	m_wireframeShader->SetDepth(COMPARE_ALWAYS, true);
	m_wireframeShader->SetBlendMode(BLEND_MODE_ALPHA);
	m_wireframeShader->SetRasterState(WIREFRAME, BACK_CULL, true);

	m_logMesh = new GPUMesh(g_theRenderer);
	m_defaultUVBounds = AABB2(0.9375f, 0.9375f, 1.0f, 1.0f);
	// testing helper function
	//DebugRenderMessagef(100.0f, Rgba::CYAN, Rgba::RED, "DebugRender: start up phase");
}

void DebugRender::Shutdown()
{
	//DebugRenderMessagef(1.0f, Rgba::CYAN, Rgba::RED, "DebugRender: shut down phase");

	delete m_logMesh;
	m_logMesh = nullptr;
	
	delete m_debugLogCamera;
	m_debugLogCamera = nullptr;

	delete m_alwaysShader;
	m_alwaysShader = nullptr;

	delete m_lessEqualShader;
	m_lessEqualShader = nullptr;

	delete m_greaterShader;
	m_greaterShader = nullptr;

	delete m_wireframeShader;
	m_wireframeShader = nullptr;
}

void DebugRender::BeginFrame()
{
	if (!DEBUG_RENDER_IN_USE) return;

	
	m_timeBeginFrame = GetCurrentTimeSecondsF();
	m_frameNumber++;
}

void DebugRender::EndFrame()
{
	if (!DEBUG_RENDER_IN_USE) return;
	
	m_timeEndFrame = GetCurrentTimeSecondsF();
	float delta_time = m_timeEndFrame - m_timeBeginFrame;
	delta_time = ClampFloat(delta_time, 0.0f, 1.0f);

	for (std::vector<DebugLine>::iterator vec_line_it = m_debugLog.begin();
	     vec_line_it != m_debugLog.end();
		     /*++it*/)
	{
		if (vec_line_it->m_currentTime >= vec_line_it->m_duration)
			vec_line_it = m_debugLog.erase(vec_line_it);
		else
		{
			vec_line_it->m_currentTime += delta_time;
			++vec_line_it;
		}
	}

	for (std::vector<Debug2Dobj>::iterator vec_2D_it = m_debug2Dobjs.begin();
	     vec_2D_it != m_debug2Dobjs.end();
		     /*++it*/)
	{
		if (vec_2D_it->m_currentTime > vec_2D_it->m_duration)
			vec_2D_it = m_debug2Dobjs.erase(vec_2D_it);
		else
		{
			vec_2D_it->m_currentTime += delta_time;
			++vec_2D_it;
		}
	}

	for (std::vector<Debug3Dobj>::iterator vec_3D_it = m_debug3Dobjs.begin();
		vec_3D_it != m_debug3Dobjs.end();
		/*++it*/)
	{
		if (vec_3D_it->m_currentTime > vec_3D_it->m_duration)
			vec_3D_it = m_debug3Dobjs.erase(vec_3D_it);
		else
		{
			vec_3D_it->m_currentTime += delta_time;
			++vec_3D_it;
		}
	}

	CheckToClear();
}

void DebugRender::RenderToCamera(Camera* camera)
{
	if (!DEBUG_RENDER_IN_USE) return;
	
	m_gameCamera = camera;
	ColorTargetView* rtv = g_theRenderer->GetFrameColorTarget();
	m_gameCamera->SetColorTarget(rtv);

	// Move the camera to where it is in the scene
	// (right now, no rotation (looking forward), set 10 back (so looking at 0,0,0)
	//const Matrix44 cam_model = m_gameCamera->GetModelMatrix();
	//m_gameCamera->SetModelMatrix(cam_model);

	g_theRenderer->BindModelMatrix(Matrix44::IDENTITY);

	g_theRenderer->BeginCamera(m_gameCamera);
	g_theRenderer->BindSampler(SAMPLE_MODE_POINT);

	CPUMesh combined_meshes_per_render_mode[ NUM_DEBUG_RENDER_MODES ];
	GPUMesh* gpu_mesh = new GPUMesh(g_theRenderer); // scratch recycled VBO

	for (std::vector<Debug3Dobj>::iterator vec_3D_it = m_debug3Dobjs.begin();
		vec_3D_it != m_debug3Dobjs.end();
		/*++it*/)
	{
		const float fraction = vec_3D_it->m_currentTime / vec_3D_it->m_duration;
		const Rgba current_color = LinearInterpolationRgba(vec_3D_it->m_startColor, vec_3D_it->m_endColor, fraction);
		vec_3D_it->m_mesh.SetColor(current_color);

		if(vec_3D_it->m_isBillboarded)
		{
			vec_3D_it->m_moodleMatrix.SetRotationMatrix(m_gameCamera->GetModelMatrix().GetRotationMatrix());
		}

		switch(vec_3D_it->m_drawMode)
		{
			case DEBUG_RENDER_USE_DEPTH:
			{
				//g_theRenderer->BindShader(m_lessEqualShader);
				vec_3D_it->m_mesh.TransformVertexCollection(vec_3D_it->m_moodleMatrix, 1.0f );
				combined_meshes_per_render_mode[ DEBUG_RENDER_USE_DEPTH ].ConcatCPUMesh(vec_3D_it->m_mesh);
				break;
			}
			case DEBUG_RENDER_ALWAYS:
			{
				//g_theRenderer->BindShader(m_alwaysShader);
				vec_3D_it->m_mesh.TransformVertexCollection(vec_3D_it->m_moodleMatrix, 1.0f );
				combined_meshes_per_render_mode[ DEBUG_RENDER_ALWAYS ].ConcatCPUMesh(vec_3D_it->m_mesh);

				break;
			}
			case DEBUG_RENDER_GREATER:
			{
				//g_theRenderer->BindShader(m_greaterShader);
				vec_3D_it->m_mesh.TransformVertexCollection(vec_3D_it->m_moodleMatrix, 1.0f );
				combined_meshes_per_render_mode[ DEBUG_RENDER_GREATER ].ConcatCPUMesh(vec_3D_it->m_mesh);
				break;
			}
			case DEBUG_RENDER_XRAY:
			{
				//DebugRenderMessagef(10.0f, Rgba::MAGENTA, Rgba::RED, "Only using Depth, always, or greater.");
				vec_3D_it->m_mesh.TransformVertexCollection(vec_3D_it->m_moodleMatrix, 1.0f );
				combined_meshes_per_render_mode[ DEBUG_RENDER_XRAY ].ConcatCPUMesh(vec_3D_it->m_mesh);
				break;
			}
			case DEBUG_RENDER_WIREFRAME:
			{
				//g_theRenderer->BindShader(m_wireframeShader);
				vec_3D_it->m_mesh.TransformVertexCollection(vec_3D_it->m_moodleMatrix, 1.0f );
				combined_meshes_per_render_mode[ DEBUG_RENDER_WIREFRAME ].ConcatCPUMesh(vec_3D_it->m_mesh);
				break;
			}
			default:
			{
				DebugRenderMessagef(10.0f, Rgba::MAGENTA, Rgba::RED, "Attempting to render a 3D object with an unknown render mode.");
				//g_theRenderer->BindShader(m_alwaysShader);
				vec_3D_it->m_mesh.TransformVertexCollection(vec_3D_it->m_moodleMatrix, 1.0f );
				combined_meshes_per_render_mode[ DEBUG_RENDER_ALWAYS ].ConcatCPUMesh(vec_3D_it->m_mesh);
				break;
			}
		}
		++vec_3D_it;
	}

	//DEBUG_RENDER_USE_DEPTH
	if(!combined_meshes_per_render_mode[ DEBUG_RENDER_USE_DEPTH ].m_vertices.empty())
	{
		gpu_mesh->CreateFromCPUMesh<Vertex_PCU>(combined_meshes_per_render_mode[ DEBUG_RENDER_USE_DEPTH ]);
		g_theRenderer->BindShader(m_lessEqualShader);
		g_theRenderer->BindModelMatrix(Matrix44::IDENTITY);
		g_theRenderer->BindTextureViewWithSampler(0U, m_defaultTexture);
		g_theRenderer->DrawMesh(*gpu_mesh);
	}

	//DEBUG_RENDER_ALWAYS
	if(!combined_meshes_per_render_mode[ DEBUG_RENDER_ALWAYS ].m_vertices.empty())
	{
		gpu_mesh->CreateFromCPUMesh<Vertex_PCU>(combined_meshes_per_render_mode[ DEBUG_RENDER_ALWAYS ]);
		g_theRenderer->BindShader(m_alwaysShader);
		g_theRenderer->BindModelMatrix(Matrix44::IDENTITY);
		g_theRenderer->BindTextureViewWithSampler(0U, m_defaultTexture);
		g_theRenderer->DrawMesh(*gpu_mesh);
	}
	
	//DEBUG_RENDER_GREATER
	if(!combined_meshes_per_render_mode[ DEBUG_RENDER_GREATER ].m_vertices.empty())
	{
		gpu_mesh->CreateFromCPUMesh<Vertex_PCU>(combined_meshes_per_render_mode[ DEBUG_RENDER_GREATER ]);
		g_theRenderer->BindShader(m_greaterShader);
		g_theRenderer->BindModelMatrix(Matrix44::IDENTITY);
		g_theRenderer->BindTextureViewWithSampler(0U, m_defaultTexture);
		g_theRenderer->DrawMesh(*gpu_mesh);
	}

	//DEBUG_RENDER_XRAY
	if(!combined_meshes_per_render_mode[ DEBUG_RENDER_XRAY ].m_vertices.empty())
	{
		gpu_mesh->CreateFromCPUMesh<Vertex_PCU>(combined_meshes_per_render_mode[ DEBUG_RENDER_XRAY ]);
		g_theRenderer->BindShader(m_alwaysShader);
		g_theRenderer->BindModelMatrix(Matrix44::IDENTITY);
		g_theRenderer->BindTextureViewWithSampler(0U, m_defaultTexture);
		g_theRenderer->DrawMesh(*gpu_mesh);
	}

	//DEBUG_RENDER_WIREFRAME
	if(!combined_meshes_per_render_mode[ DEBUG_RENDER_WIREFRAME ].m_vertices.empty())
	{
		gpu_mesh->CreateFromCPUMesh<Vertex_PCU>(combined_meshes_per_render_mode[ DEBUG_RENDER_WIREFRAME ]);
		g_theRenderer->BindShader(m_wireframeShader);
		g_theRenderer->BindModelMatrix(Matrix44::IDENTITY);
		g_theRenderer->BindTextureViewWithSampler(0U, m_defaultTexture);
		g_theRenderer->DrawMesh(*gpu_mesh);
	}

	delete gpu_mesh;
	gpu_mesh = nullptr;


	g_theRenderer->EndCamera(m_gameCamera);
}

void DebugRender::RenderToScreen()
{
	if (!DEBUG_RENDER_IN_USE) return;

	m_debugLogCamera->SetColorTarget(m_gameRenderContext->GetFrameColorTarget());

	m_debugLogCamera->SetModelMatrix(Matrix44::IDENTITY);
	m_gameRenderContext->BeginCamera(m_debugLogCamera);
	m_gameRenderContext->ClearDepthStencilTarget(1.0f);
	g_theRenderer->BindShader( g_theRenderer->CreateOrGetShader("default_unlit.hlsl"));

	CPUMesh combined_meshes;
	GPUMesh* gpu_mesh = new GPUMesh(g_theRenderer); // scratch recycled VBO

	for (std::vector<Debug2Dobj>::iterator vec_2D_it = m_debug2Dobjs.begin();
	     vec_2D_it != m_debug2Dobjs.end();
		     /*++it*/)
	{
		const float fraction = vec_2D_it->m_currentTime / vec_2D_it->m_duration;
		const Rgba current_color = LinearInterpolationRgba(vec_2D_it->m_startColor, vec_2D_it->m_endColor, fraction);
		vec_2D_it->m_mesh.SetColor(current_color);
		Matrix44 transform = Matrix44::ForEulerZXY(vec_2D_it->m_rotation, vec_2D_it->m_position);
		vec_2D_it->m_mesh.TransformVertexCollection(transform, 1.0f );
		combined_meshes.ConcatCPUMesh(vec_2D_it->m_mesh);

		++vec_2D_it;
	}

	if(!combined_meshes.m_vertices.empty())
	{
		g_theRenderer->BindTextureViewWithSampler(0U, nullptr);
		g_theRenderer->BindModelMatrix(Matrix44::IDENTITY);
		gpu_mesh->CreateFromCPUMesh<Vertex_PCU>(combined_meshes); // we won't be updated this;
		g_theRenderer->DrawMesh(*gpu_mesh);
	}

	delete gpu_mesh;
	gpu_mesh = nullptr;

	RenderDebugLog();

	m_gameRenderContext->EndCamera(m_debugLogCamera);
}

void DebugRender::DebugRenderPoint(float duration, eDebugRenderMode mode, const Vec3& pos, const Rgba& start_color, const Rgba& end_color, float size)
{
	DebugRenderQuad(duration, mode, true, pos, Vec3::ZERO, AABB2(-size * 0.5f, -size * 0.5f, size * 0.5f, size * 0.5f), start_color, end_color, nullptr);
}

void DebugRender::DebugRenderQuad(float duration, eDebugRenderMode mode, bool is_billboard, const Vec3& pos, const Vec3& rotation,
                                  const AABB2& quad, const Rgba& start_color, const Rgba& end_color, TextureView* view)
{
	if (!DEBUG_RENDER_IN_USE) return;
	
	CPUMesh quad_mesh;
	//TODO: need to let the user define the UVs
	CpuMeshAddQuad(&quad_mesh, false, Rgba::WHITE, quad, m_defaultUVBounds);


	if(mode == DEBUG_RENDER_XRAY)
	{
		m_debug3Dobjs.emplace_back(quad_mesh, start_color, end_color, duration, pos, rotation, is_billboard, DEBUG_RENDER_USE_DEPTH, view);
		m_debug3Dobjs.emplace_back(quad_mesh, start_color * 0.5f, end_color * 0.5f, duration, pos, rotation, is_billboard, DEBUG_RENDER_GREATER, view);
	}
	else
	{
		m_debug3Dobjs.emplace_back(quad_mesh, start_color, end_color, duration, pos, rotation, is_billboard, mode, view);
	}
}

void DebugRender::DebugRenderQuad(float duration, eDebugRenderMode mode, const Vec3& bottom_left, const Vec3& bottom_right, const Vec3& top_left, const Rgba& start_color, const Rgba& end_color,
	TextureView* view)
{
	CPUMesh quad_mesh;
	CpuMeshAddQuad(&quad_mesh, bottom_left, bottom_right, top_left, m_defaultUVBounds);


	if(mode == DEBUG_RENDER_XRAY)
	{
		m_debug3Dobjs.emplace_back(quad_mesh, start_color, end_color, duration, Vec3::ZERO, Vec3::ZERO, false, DEBUG_RENDER_USE_DEPTH, view);
		m_debug3Dobjs.emplace_back(quad_mesh, start_color * 0.5f, end_color * 0.5f, duration, Vec3::ZERO, Vec3::ZERO, false, DEBUG_RENDER_GREATER, view);
	}
	else
	{
		m_debug3Dobjs.emplace_back(quad_mesh, start_color, end_color, duration, Vec3::ZERO, Vec3::ZERO, false, mode, view);
	}
}

void DebugRender::DebugRenderLine(float duration, eDebugRenderMode mode, const Vec3& position, float length,
	const Vec3& rotation, const Rgba& start_color, const Rgba& end_color, float width)
{
	if (!DEBUG_RENDER_IN_USE) return;

	
	CPUMesh cylinder_mesh;
	CpuMeshAddCylinder(&cylinder_mesh, width, length);
	//Vec3 rotation = Vec3::MakeFromSphericalDegrees(angular_displacement.x, angular_displacement.y);

	if(mode == DEBUG_RENDER_XRAY)
	{
		m_debug3Dobjs.emplace_back(cylinder_mesh, start_color, end_color, duration, position, rotation, false, DEBUG_RENDER_USE_DEPTH, nullptr);
		m_debug3Dobjs.emplace_back(cylinder_mesh, start_color * 0.5f, end_color * 0.5f, duration, position, rotation, false, DEBUG_RENDER_GREATER, nullptr);
	}
	else
	{
		m_debug3Dobjs.emplace_back(cylinder_mesh, start_color, end_color, duration, position, rotation, false, mode, nullptr);
	}
}

void DebugRender::DebugRenderText(float duration, eDebugRenderMode mode, const Rgba& start_color, const Rgba& end_color, float cell_height,
	const std::string& text, const Vec2& alignment, const AABB2& local_box, const Vec3& position, const Vec3& rotations,  bool is_billboard,  
	const Rgba& box_color, float cell_aspect, BitmapFont::TextDrawMode text_draw_mode, int max_glyphs_to_draw)
{
	if (!DEBUG_RENDER_IN_USE) return;
	
	CPUMesh text_box_mesh;
	BitmapFont* console_font = m_gameRenderContext->CreateOrGetBitmapFont(m_fontFile.c_str());
	const TextureView* font_texture = console_font->GetTexture();

	DebugRenderQuad(duration, mode, is_billboard, position, rotations, local_box, box_color, box_color, nullptr);

	console_font->AddMeshForTextInBox2D(&text_box_mesh, local_box, cell_height, text, start_color, cell_aspect, alignment,
		text_draw_mode, max_glyphs_to_draw);

	if(mode == DEBUG_RENDER_XRAY)
	{
		m_debug3Dobjs.emplace_back(text_box_mesh, start_color, end_color, duration, position, rotations, is_billboard, DEBUG_RENDER_USE_DEPTH,  const_cast<TextureView*>(font_texture));
		m_debug3Dobjs.emplace_back(text_box_mesh, start_color, end_color, duration, position, rotations, is_billboard, DEBUG_RENDER_GREATER,  const_cast<TextureView*>(font_texture));
	}
	else
	{
		m_debug3Dobjs.emplace_back(text_box_mesh, start_color, end_color, duration, position, rotations, is_billboard, mode,  const_cast<TextureView*>(font_texture));
	}
	// we are drawing the text at the position we want it, so no need to translate
}

void DebugRender::DebugRenderTextv(float duration, eDebugRenderMode mode, const Rgba& start_color,
	const Rgba& end_color, float cell_height, char const* format, va_list arguments, const Vec2& alignment,
	const AABB2& local_box, const Vec3& position, const Vec3& rotations, bool is_billboard, const Rgba& box_color,
	float cell_aspect, BitmapFont::TextDrawMode text_draw_mode, int max_glyphs_to_draw)
{
	if (!DEBUG_RENDER_IN_USE) return;

	
	char text_literal[ STRINGF_STACK_LOCAL_TEMP_LENGTH ];
	vsnprintf_s(text_literal, STRINGF_STACK_LOCAL_TEMP_LENGTH, _TRUNCATE, format, arguments);
	text_literal[STRINGF_STACK_LOCAL_TEMP_LENGTH - 1] = '\0';
	const std::string line(text_literal);
	
	CPUMesh text_box_mesh;
	BitmapFont* console_font = m_gameRenderContext->CreateOrGetBitmapFont(m_fontFile.c_str());
	const TextureView* font_texture = console_font->GetTexture();

	Vec3 slight_adjustment(position.x, position.y, position.z + 0.001f);
	DebugRenderQuad(duration, mode, is_billboard, slight_adjustment, rotations, local_box, box_color, box_color, nullptr);

	console_font->AddMeshForTextInBox2D(&text_box_mesh, local_box, cell_height, line, start_color, cell_aspect, alignment,
		text_draw_mode, max_glyphs_to_draw);

	if(mode == DEBUG_RENDER_XRAY)
	{
		m_debug3Dobjs.emplace_back(text_box_mesh, start_color, end_color, duration, position, rotations, is_billboard, DEBUG_RENDER_USE_DEPTH,  const_cast<TextureView*>(font_texture));
		m_debug3Dobjs.emplace_back(text_box_mesh, start_color, end_color, duration, position, rotations, is_billboard, DEBUG_RENDER_GREATER,  const_cast<TextureView*>(font_texture));
	}
	else
	{
		m_debug3Dobjs.emplace_back(text_box_mesh, start_color, end_color, duration, position, rotations, is_billboard, mode,  const_cast<TextureView*>(font_texture));
	}
	// we are drawing the text at the position we want it, so no need to translate
}


void DebugRender::DebugRenderTextf(float duration, eDebugRenderMode mode, const Rgba& start_color,
	const Rgba& end_color, float cell_height, char const* format, ...)
{
	if (!DEBUG_RENDER_IN_USE) return;

	
	va_list variable_argument_list;
	va_start( variable_argument_list, format );

	DebugRenderTextv(duration, mode, start_color, end_color, cell_height, format, variable_argument_list);

	va_end( variable_argument_list );
}

void DebugRender::DebugRenderTextf(float duration, eDebugRenderMode mode, const Rgba& start_color,
	const Rgba& end_color, float cell_height, const AABB2& local_box, const Vec3& position, const Vec3& rotations,
	const Rgba& box_color, char const* format, ...)
{
	if (!DEBUG_RENDER_IN_USE) return;


	va_list variable_argument_list;
	va_start( variable_argument_list, format );

	DebugRenderTextv(duration, mode, start_color, end_color, cell_height, format, variable_argument_list, AABB2::ALIGN_CENTERED,
		local_box, position, rotations, false, box_color);

	va_end( variable_argument_list );
}

void DebugRender::DebugRenderTextf(float duration, eDebugRenderMode mode, const Rgba& start_color,
	const Rgba& end_color, float cell_height, const AABB2& local_box, const Vec3& position, const Rgba& box_color, char const* format, ...)
{
	if (!DEBUG_RENDER_IN_USE) return;

	
	va_list variable_argument_list;
	va_start( variable_argument_list, format );

	DebugRenderTextv(duration, mode, start_color, end_color, cell_height, format, variable_argument_list, AABB2::ALIGN_CENTERED,
		local_box, position, Vec3::ZERO, true, box_color);

	va_end( variable_argument_list );
}

void DebugRender::DebugRenderSphere(float duration, eDebugRenderMode mode, const Vec3& pos, float radius, Rgba start_color,
	Rgba end_color)
{
	if (!DEBUG_RENDER_IN_USE) return;
	
	CPUMesh sphere_mesh;
	CpuMeshAddUVSphere(&sphere_mesh, Vec3::ZERO, radius, 32, 16);

	if(mode == DEBUG_RENDER_XRAY)
	{
		m_debug3Dobjs.emplace_back(sphere_mesh, start_color, end_color, duration, pos, Vec3::ZERO, false, DEBUG_RENDER_USE_DEPTH, nullptr);
		m_debug3Dobjs.emplace_back(sphere_mesh, start_color * 0.5f, end_color * 0.5f, duration, pos, Vec3::ZERO, false, DEBUG_RENDER_GREATER, nullptr);
	}
	else
	{
		m_debug3Dobjs.emplace_back(sphere_mesh, start_color, end_color, duration, pos, Vec3::ZERO, false, mode, nullptr);
	}
}

void DebugRender::DebugRenderSphere(float duration, eDebugRenderMode mode, const Vec3& pos, float radius,
	Rgba start_color, Rgba end_color, int wedges, int slices)
{
	if (!DEBUG_RENDER_IN_USE) return;

	CPUMesh sphere_mesh;
	CpuMeshAddUVSphere(&sphere_mesh, Vec3::ZERO, radius, wedges, slices);

	if(mode == DEBUG_RENDER_XRAY)
	{
		m_debug3Dobjs.emplace_back(sphere_mesh, start_color, end_color, duration, pos, Vec3::ZERO, false, DEBUG_RENDER_USE_DEPTH, nullptr);
		m_debug3Dobjs.emplace_back(sphere_mesh, start_color * 0.5f, end_color * 0.5f, duration, pos, Vec3::ZERO, false, DEBUG_RENDER_GREATER, nullptr);
	}
	else
	{
		m_debug3Dobjs.emplace_back(sphere_mesh, start_color, end_color, duration, pos, Vec3::ZERO, false, mode, nullptr);
	}
}

void DebugRender::DebugRenderBox(float duration, eDebugRenderMode mode, const AABB3& cube, Rgba start_color, Rgba end_color, TextureView* view)
{
	if (!DEBUG_RENDER_IN_USE) return;
	
	CPUMesh cube_mesh;
	CpuMeshAddCube(&cube_mesh, cube, m_defaultUVBounds);

	if(mode == DEBUG_RENDER_XRAY)
	{
		m_debug3Dobjs.emplace_back(cube_mesh, start_color, end_color, duration, Vec3::ZERO, Vec3::ZERO, false, DEBUG_RENDER_USE_DEPTH, view);
		m_debug3Dobjs.emplace_back(cube_mesh, start_color * 0.5f, end_color * 0.5f, duration, Vec3::ZERO, Vec3::ZERO, false, DEBUG_RENDER_GREATER, view);
	}
	else
	{
		m_debug3Dobjs.emplace_back(cube_mesh, start_color, end_color, duration, Vec3::ZERO, Vec3::ZERO, false, mode, view);
	}
}

void DebugRender::DebugRenderBox(float duration, eDebugRenderMode mode, const OBB3& cube, const Rgba& start_color,
	const Rgba& end_color)
{
	if (!DEBUG_RENDER_IN_USE) return;

	CPUMesh cube_mesh;
	CpuMeshAddCube(&cube_mesh, cube, m_defaultUVBounds);

	if(mode == DEBUG_RENDER_XRAY)
	{
		m_debug3Dobjs.emplace_back(cube_mesh, start_color, end_color, duration, Vec3::ZERO, Vec3::ZERO, false, DEBUG_RENDER_USE_DEPTH, nullptr);
		m_debug3Dobjs.emplace_back(cube_mesh, start_color * 0.5f, end_color * 0.5f, duration, Vec3::ZERO, Vec3::ZERO, false, DEBUG_RENDER_GREATER, nullptr);
	}
	else
	{
		m_debug3Dobjs.emplace_back(cube_mesh, start_color, end_color, duration, Vec3::ZERO, Vec3::ZERO, false, mode, nullptr);
	}
}

void DebugRender::DebugRenderCylinder(float duration, eDebugRenderMode mode, Vec3 position, float length, Vec3 rotation, float radius,
	Rgba start_color, Rgba end_color)
{
	if (!DEBUG_RENDER_IN_USE) return;
	
	
	CPUMesh cylinder_mesh;
	CpuMeshAddCylinder(&cylinder_mesh, radius, length);
	//Vec3 rotation = Vec3::MakeFromSphericalDegrees(angular_displacement.x, angular_displacement.y);

	if(mode == DEBUG_RENDER_XRAY)
	{
		m_debug3Dobjs.emplace_back(cylinder_mesh, start_color, end_color, duration, position, rotation, false, DEBUG_RENDER_USE_DEPTH, nullptr);
		m_debug3Dobjs.emplace_back(cylinder_mesh, start_color * 0.5f, end_color * 0.5f, duration, position, rotation, false, DEBUG_RENDER_GREATER, nullptr);
	}
	else
	{
		m_debug3Dobjs.emplace_back(cylinder_mesh, start_color, end_color, duration, position, rotation, false, mode, nullptr);
	}
}

void DebugRender::DebugRenderCapsule(float duration, eDebugRenderMode mode, Vec3 position, float length, Vec3 rotation,
	float radius, Rgba start_color, Rgba end_color)
{
	if (!DEBUG_RENDER_IN_USE) return;

 	CPUMesh capsule_mesh;
    CpuMeshAddUVSphere(&capsule_mesh, false, Vec3(0.0f, 0.0f, 0.0f), radius);
 	CpuMeshAddCylinder(&capsule_mesh, true, radius, radius, length, 0.0f);
 	CpuMeshAddUVSphere(&capsule_mesh, true, Vec3(0.0f + length, 0.0f, 0.0f), radius);
 	//Vec3 rotation = Vec3::MakeFromSphericalDegrees(angular_displacement.x, angular_displacement.y);

	if(mode == DEBUG_RENDER_XRAY)
	{
		m_debug3Dobjs.emplace_back(capsule_mesh, start_color, end_color, duration, position, rotation, false, DEBUG_RENDER_USE_DEPTH, nullptr);
		m_debug3Dobjs.emplace_back(capsule_mesh, start_color * 0.5f, end_color * 0.5f, duration, position, rotation, false, DEBUG_RENDER_GREATER, nullptr);
	}
	else
	{
		m_debug3Dobjs.emplace_back(capsule_mesh, start_color, end_color, duration, position, rotation, false, mode, nullptr);
	}
}

void DebugRender::DebugRenderCapsule(float duration, const eDebugRenderMode& mode, const Capsule3& cap, const Rgba& start_color, const Rgba& end_color)
{
	if (!DEBUG_RENDER_IN_USE) return;

	CPUMesh capsule_mesh;
	CpuMeshAddCapsule(&capsule_mesh, cap);

	if(mode == DEBUG_RENDER_XRAY)
	{
		m_debug3Dobjs.emplace_back(capsule_mesh, start_color, end_color, duration, cap.start, Vec3::ZERO, false, DEBUG_RENDER_USE_DEPTH, nullptr);
		m_debug3Dobjs.emplace_back(capsule_mesh, start_color * 0.5f, end_color * 0.5f, duration, cap.start, Vec3::ZERO, false, DEBUG_RENDER_GREATER, nullptr);
	}
	else
	{
		m_debug3Dobjs.emplace_back(capsule_mesh, start_color, end_color, duration, cap.start, Vec3::ZERO, false, mode, nullptr);
	}
}

void DebugRender::DebugRenderCone(float duration, eDebugRenderMode mode, Vec3 position, float length, Vec3 rotation,
	float radius, Rgba start_color, Rgba end_color)
{
	if (!DEBUG_RENDER_IN_USE) return;
	
	
	CPUMesh cylinder_mesh;
	CpuMeshAddCylinder(&cylinder_mesh, radius, 0.0f, length);
	//Vec3 rotation = Vec3::MakeFromSphericalDegrees(angular_displacement.x, angular_displacement.y);

	if(mode == DEBUG_RENDER_XRAY)
	{
		m_debug3Dobjs.emplace_back(cylinder_mesh, start_color, end_color, duration, position, rotation, false, DEBUG_RENDER_USE_DEPTH, nullptr);
		m_debug3Dobjs.emplace_back(cylinder_mesh, start_color * 0.5f, end_color * 0.5f, duration, position, rotation, false, DEBUG_RENDER_GREATER, nullptr);
	}
	else
	{
		m_debug3Dobjs.emplace_back(cylinder_mesh, start_color, end_color, duration, position, rotation, false, mode, nullptr);
	}
}

void DebugRender::DebugRenderArrow(float duration, eDebugRenderMode mode, Vec3 position, float length, Vec3 rotation,
	float radius, Rgba start_color, Rgba end_color)
{
	if (!DEBUG_RENDER_IN_USE) return;
	
	
	CPUMesh cylinder_mesh;
	CpuMeshAddCylinder(&cylinder_mesh, radius*0.5f, length * 0.75f);
	CpuMeshAddCylinder(&cylinder_mesh, true, radius, 0.0f, length * 0.25f, length * 0.75f);


	if(mode == DEBUG_RENDER_XRAY)
	{
		m_debug3Dobjs.emplace_back(cylinder_mesh, start_color, end_color, duration, position, rotation, false, DEBUG_RENDER_USE_DEPTH, nullptr);
		m_debug3Dobjs.emplace_back(cylinder_mesh, start_color * 0.5f, end_color * 0.5f, duration, position, rotation, false, DEBUG_RENDER_GREATER, nullptr);
	}
	else
	{
		m_debug3Dobjs.emplace_back(cylinder_mesh, start_color, end_color, duration, position, rotation, false, mode, nullptr);
	}
}

void DebugRender::DebugRenderBasis(float duration, eDebugRenderMode mode)
{
	if (!DEBUG_RENDER_IN_USE) return;
	
	
	DebugRenderArrow(duration, mode, Vec3::ZERO, 1.0f, Vec3(0.0f, 0.0f, 0.0f), 0.1f, Rgba::RED, Rgba::RED);
	DebugRenderArrow(duration, mode, Vec3::ZERO, 1.0f, Vec3(0.0f, 0.0f, 90.0f), 0.1f, Rgba::GREEN, Rgba::GREEN);
	DebugRenderArrow(duration, mode, Vec3::ZERO, 1.0f, Vec3(0.0f, -90.0f, 0.0f), 0.1f, Rgba::BLUE, Rgba::BLUE);
}

void DebugRender::DebugRenderGrid(float duration, eDebugRenderMode mode, unsigned int grid_length)
{
	if (!DEBUG_RENDER_IN_USE) return;
	
	
	CPUMesh grid_mesh;
	const float grid_length_float = static_cast<float>(grid_length);
	const float half_way = grid_length * 0.5f;
	
	for(float grid_x_id = 0.0f - half_way; grid_x_id <= grid_length - half_way; )
	{
		DebugRenderLine(duration, mode, Vec3(grid_x_id, 0.0f, -half_way), static_cast<float>(grid_length), Vec3(0.0f, -90.0f, 0.0f), Rgba::WHITE, Rgba::WHITE, 0.01f);
		grid_x_id += 1.0f;
	}

	for(float grid_y_id = 0 - half_way; grid_y_id <= grid_length - half_way; )
	{
		DebugRenderLine(duration, mode, Vec3(-half_way, 0.0f, grid_y_id), static_cast<float>(grid_length), Vec3(0.0f, 0.0f, 0.0f), Rgba::WHITE, Rgba::WHITE, 0.01f);
		grid_y_id += 1.0f;
	}
}

void DebugRender::DebugRenderScreenPoint(float duration, const Vec2& pos, const Rgba& start_color,
                                         const Rgba& end_color, float size)
{
	if (!DEBUG_RENDER_IN_USE) return;
	
	
	CPUMesh disc_mesh;
	CpuMeshAddDisc(&disc_mesh, start_color, size, 64);

	// we are asking for a disc at local position, use center as it's position
	m_debug2Dobjs.emplace_back(disc_mesh, start_color, end_color, duration, Vec3(pos), Vec3::ZERO, nullptr);
}

void DebugRender::DebugRenderScreenQuad(float duration, const AABB2& quad, const Rgba& start_color,
                                        const Rgba& end_color,
                                        TextureView* view)
{
	if (!DEBUG_RENDER_IN_USE) return;
	
	
	CPUMesh quad_mesh;
	CpuMeshAddQuad(&quad_mesh, quad);

	// we are drawing the quad at the position we want it, so no need to translate
	m_debug2Dobjs.emplace_back(quad_mesh, start_color, end_color, duration, Vec3::ZERO, Vec3::ZERO, view);
}

void DebugRender::DebugRenderScreenQuad(float duration, const OBB2& quad, const Rgba& start_color,
	const Rgba& end_color, TextureView* view)
{
	if (!DEBUG_RENDER_IN_USE) return;


	CPUMesh quad_mesh;
	CpuMeshAddQuad(&quad_mesh, quad);

	// we are drawing the quad at the position we want it, so no need to translate
	m_debug2Dobjs.emplace_back(quad_mesh, start_color, end_color, duration, Vec3::ZERO, Vec3::ZERO, view);
}

void DebugRender::DebugRenderScreenLine(float duration, const Vec2& p0, const Vec2& p1, const Rgba& start_color,
                                        const Rgba& end_color, float width)
{
	if (!DEBUG_RENDER_IN_USE) return;
	
	
	CPUMesh line_mesh;
	CpuMeshAddLine(&line_mesh, p0, p1, width, start_color);

	// we are drawing the line at the position we want it, so no need to translate
	m_debug2Dobjs.emplace_back(line_mesh, start_color, end_color, duration, Vec3::ZERO, Vec3::ZERO, nullptr);
}

void DebugRender::DebugRenderScreenRing(float duration, const Vec2& center, const Rgba& start_color,
                                        const Rgba& end_color,
                                        float radius, float thickness, int num_sides)
{
	if (!DEBUG_RENDER_IN_USE) return;
	
	
	CPUMesh disc_mesh;
	CpuMeshAddRing2D(&disc_mesh, radius, thickness, start_color, num_sides);

	m_debug2Dobjs.emplace_back(disc_mesh, start_color, end_color, duration, Vec3(center), Vec3::ZERO, nullptr);
}

void DebugRender::DebugRenderScreenTextBoxv(float duration, const Rgba& start_color, const Rgba& end_color,
	float cell_height, char const* format, va_list arguments, const Vec2& alignment, const AABB2& box,
	const Rgba& box_color, float cell_aspect, BitmapFont::TextDrawMode mode, int max_glyphs_to_draw)
{
	if (!DEBUG_RENDER_IN_USE) return;
	
	
	char text_literal[ STRINGF_STACK_LOCAL_TEMP_LENGTH ];
	vsnprintf_s(text_literal, STRINGF_STACK_LOCAL_TEMP_LENGTH, _TRUNCATE, format, arguments);
	text_literal[STRINGF_STACK_LOCAL_TEMP_LENGTH - 1] = '\0';
	const std::string line(text_literal);

	CPUMesh text_box_mesh;
	BitmapFont* console_font = m_gameRenderContext->CreateOrGetBitmapFont(m_fontFile.c_str());
	const TextureView* font_texture = console_font->GetTexture();

	DebugRenderScreenQuad(duration, box, box_color, box_color, nullptr);

	console_font->AddMeshForTextInBox2D(&text_box_mesh, box, cell_height, line, start_color, cell_aspect, alignment,
		mode, max_glyphs_to_draw);

	// we are drawing the text at the position we want it, so no need to translate
	m_debug2Dobjs.emplace_back(text_box_mesh, start_color, end_color, duration, Vec3::ZERO, Vec3::ZERO,
		const_cast<TextureView*>(font_texture));
}

void DebugRender::DebugRenderScreenTextBoxf(float duration, const Rgba& start_color, const Rgba& end_color,
	float cell_height, char const* format, ...)
{
	if (!DEBUG_RENDER_IN_USE) return;
	
	
	va_list variable_argument_list;
	va_start( variable_argument_list, format );
	
	DebugRenderScreenTextBoxv(duration, start_color, end_color, cell_height, format, variable_argument_list);

	va_end( variable_argument_list );
}

void DebugRender::DebugRenderScreenTextBoxf(float duration, const Rgba& start_color, const Rgba& end_color,
	float cell_height, const Vec2& alignment, char const* format, ...)
{
	if (!DEBUG_RENDER_IN_USE) return;
	
	
	va_list variable_argument_list;
	va_start( variable_argument_list, format );

	DebugRenderScreenTextBoxv(duration, start_color, end_color, cell_height, format, variable_argument_list, alignment);

	va_end( variable_argument_list );
}

void DebugRender::DebugRenderScreenArrow(float duration, const Vec2& p0, const Vec2& p1, const Rgba& start_color,
	const Rgba& end_color, float size)
{
	if (!DEBUG_RENDER_IN_USE) return;
	
	
	Vec2 direction(p1 - p0);
	float magnitude = direction.GetLength();
	const float rotation = direction.GetAngleDegrees();
	float minus_arrowhead_length = magnitude - (SQRT_3_OVER_2 * size);
	direction.SetLength(minus_arrowhead_length);
	Vec2 new_endpoint = p0 + direction;

	DebugRenderScreenLine(duration, p0, new_endpoint, start_color, end_color, size*0.5f);

	CPUMesh arrow_mesh;
	CpuMeshAddTriangle(&arrow_mesh, size, start_color);
	arrow_mesh.TransformVertexCollection(size, rotation, new_endpoint);

	// we are drawing the text at the position we want it, so no need to translate
	m_debug2Dobjs.emplace_back(arrow_mesh, start_color, end_color, duration, Vec3::ZERO, Vec3::ZERO, nullptr);
}

void DebugRender::DebugRenderScreenGrid(float duration, const Rgba& start_color,
	const AABB2& box, float primary_stride, bool sub_divide)
{
	if (!DEBUG_RENDER_IN_USE) return;
	
	
	// ask the box it's dimentions
	float box_width = box.maxs.x - box.mins.x;
	float box_height = box.maxs.y - box.mins.y;

	float new_stride = (sub_divide) ? primary_stride * 0.5f : primary_stride;
	
	unsigned int stride_step = 0;
	for(float width_stride = 0.0f; width_stride <= box_width; )
	{
		Vec2 line_start(box.mins.x + width_stride, box.mins.y);
		Vec2 line_end(box.mins.x + width_stride, box.maxs.y);

		//float new_thickness = 1.0f / pow(4.0f, static_cast<float>(stride_step));
		Rgba line_color = start_color;

		if(sub_divide)
		{
			line_color = (stride_step == 0) ? start_color : 
			Rgba(start_color.r, start_color.g, start_color.b, start_color.a * 0.5f);
		}

		DebugRenderScreenLine(duration, line_start, line_end, line_color, line_color, 1.0f);

		stride_step = (++stride_step) % 2;
		width_stride += new_stride;
      	}

	stride_step = 0;
	for(float height_stride = 0.0f; height_stride <= box_height; )
	{
		Vec2 line_start(box.mins.x, box.mins.y + height_stride);
		Vec2 line_end(box.maxs.x, box.mins.y + height_stride);

		//float new_thickness = 1.0f / pow(4.0f, static_cast<float>(stride_step));
		Rgba line_color = start_color;

		if(sub_divide)
		{
			line_color = (stride_step == 0) ? start_color : 
				Rgba(start_color.r, start_color.g, start_color.b, start_color.a * 0.5f);	
		}

		DebugRenderScreenLine(duration, line_start, line_end, line_color, line_color, 1.0f);

		stride_step = (++stride_step) % 2;
		height_stride += new_stride;
	}
}

void DebugRender::DebugRenderMessagef(float duration, const Rgba& start_color, const Rgba& end_color,
                                      const char* format, ...)
{
	if (!DEBUG_RENDER_IN_USE) return;
	
	
	char text_literal[ STRINGF_STACK_LOCAL_TEMP_LENGTH ];
	va_list variable_argument_list;
	va_start( variable_argument_list, format );
	vsnprintf_s(text_literal, STRINGF_STACK_LOCAL_TEMP_LENGTH, _TRUNCATE, format, variable_argument_list);
	va_end( variable_argument_list );
	text_literal[STRINGF_STACK_LOCAL_TEMP_LENGTH - 1] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)
	const std::string line(text_literal);

	std::vector<std::string> lines = SplitStringOnDelimiter(line, '\n');

	for (int lineID = 0; lineID < static_cast<int>(lines.size()); lineID++)
	{
		m_debugLog.emplace_back(lines[lineID], start_color, end_color, duration);
	}
}

void DebugRender::CheckToClear()
{
	if (DEBUG_RENDER_LOG_CLEAR)
	{
		m_debugLog.clear();
		m_debug2Dobjs.clear();
		m_debug3Dobjs.clear();
		DEBUG_RENDER_LOG_CLEAR = false;
	}
}

void DebugRender::RenderDebugLog()
{
	if (!DEBUG_RENDER_IN_USE) return;

	if(m_debugLog.size() == 0) return;

	g_theRenderer->BindModelMatrix(Matrix44::IDENTITY);

	// create the screen boundaries
	m_gameRenderContext->BindTextureView(0U, "white");
	const AABB2 debug_bounds(m_debugLogCamera->GetOrthoBottomLeft(), m_debugLogCamera->GetOrthoTopRight());
	std::vector<Vertex_PCU> debug_verts;
	AddVertsForAABB2D(debug_verts, debug_bounds, Rgba(0.0f, 0.0f, 0.0f, 0.00f));
	m_gameRenderContext->DrawVertexArray(debug_verts);

	// create the log
	const int number_of_lines_allowed = RoundToNearestInt((debug_bounds.maxs.y - debug_bounds.mins.y) / m_fontSize);
	const int minimum_index = static_cast<int>(m_debugLog.size()) - (1 + number_of_lines_allowed);

	BitmapFont* console_font = m_gameRenderContext->CreateOrGetBitmapFont(m_fontFile.c_str());
	m_gameRenderContext->BindTextureView(0U, (console_font->GetTexture()));
	CPUMesh log_mesh;

	// write log history
	for (int lineID = static_cast<int>(m_debugLog.size()) - 1; lineID > minimum_index; lineID--)
	{
		if (lineID < 0) break;

		float fraction = m_debugLog[lineID].m_currentTime / m_debugLog[lineID].m_duration;
		if (isnan(fraction)) fraction = 1.0f;
		Rgba currentColor = LinearInterpolationRgba(m_debugLog[lineID].m_startColor, m_debugLog[lineID].m_endColor,
		                                            fraction);

		float text_pos_y = (static_cast<int>(m_debugLog.size()) - lineID - 1) * m_fontSize;

		console_font->AddMeshForText2D(&log_mesh, Vec2(debug_bounds.mins.x + 1.0f, debug_bounds.mins.y + text_pos_y + 1.0f), m_fontSize, m_debugLog[lineID].m_line,
			currentColor);
	}

	//m_gameRenderContext->BindTextureView(0U, console_font->GetTexture());
	//m_gameRenderContext->DrawVertexArray(text_verts);

	m_logMesh->CreateFromCPUMesh<Vertex_PCU>(log_mesh);
	g_theRenderer->DrawMesh(*m_logMesh);
}
