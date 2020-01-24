#pragma once
#include "Engine/EngineCommon.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/CPUMesh.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Math/MathUtils.hpp"
#include <vector>

struct Plane3;
class Camera;
class TextureView;
class Shader;
class GPUMesh;

enum eDebugRenderMode
{
	DEBUG_RENDER_NULL = -1,
	DEBUG_RENDER_USE_DEPTH,
	DEBUG_RENDER_ALWAYS,
	DEBUG_RENDER_GREATER,
	DEBUG_RENDER_XRAY,
	DEBUG_RENDER_WIREFRAME,
	NUM_DEBUG_RENDER_MODES
};

static float g_defaultVirtualScreenHeight = static_cast<float>(WINDOW_RES_Y);

static AABB2 g_defaultScreenBoarder(
	Vec2(g_defaultVirtualScreenHeight * 1.7777777777778f * -0.5f, g_defaultVirtualScreenHeight * -0.5f), 
	Vec2(g_defaultVirtualScreenHeight * 1.7777777777778f * 0.5f, g_defaultVirtualScreenHeight * 0.5f));

static AABB2 g_defaultTextBox(-1.0f, -1.0f, 1.0f, 1.0f);

class DebugRender
{
public:
	DebugRender();
	~DebugRender();

	//------------------------------------------------------------------------
	// System
	//------------------------------------------------------------------------
	// Determines the "screen size" to use for for the debug render system
	// I use 720.0f high, but use whatever makes sense to you for a "screen" coordinate
	void Startup(RenderContext* context, float virtual_screen_height = g_defaultVirtualScreenHeight);
	void Startup(RenderContext* context, const Vec2& half_extents);
	// how high I want to treat the screen for glyph/screen modes; 
	void Shutdown();

	// Common functionality
	void BeginFrame(); // ...placeholder - does nothing
	void EndFrame(); // Cleanup old objects (called AFTER frame has been presented)

	// Rendering
	void RenderToCamera(Camera* camera); // renders camera/world space objects to this camera; 
	void RenderToScreen(); // render screenspace debug objects to backbuffer

	static bool Show(EventArgs& args); // enable/disable rendering debug commands

	static bool Clear(EventArgs& args); // Clear all Debug Render commands that have duration; 

	//------------------------------------------------------------------------
	// 3D
	//------------------------------------------------------------------------
	//Command to render a Point in 3D Space (for a given time, start color, and end color)
	void DebugRenderPoint(float duration, eDebugRenderMode mode,
	                      const Vec3& pos,
	                      const Rgba& start_color, const Rgba& end_color,
	                      float size = 0.25f);
	//Command to render a billboarded quad in 3D space (for a given time, start color, end color, and optional texture)
	void DebugRenderQuad(float duration, eDebugRenderMode mode, bool is_billboard,
	                     const Vec3& pos, const Vec3& rotation,
						 const AABB2& quad,
	                     const Rgba& start_color, const Rgba& end_color,
	                     TextureView* view = nullptr);

	void DebugRenderQuad(float duration, eDebugRenderMode mode, 
						 const Vec3& bottom_left, const Vec3& bottom_right, const Vec3& top_left, 
						 const Rgba& start_color, const Rgba& end_color,
						 TextureView* view = nullptr);

	// Command to render a line in 3D space (for a given time, start color, and end color)
	void DebugRenderLine(float duration, eDebugRenderMode mode,
						const Vec3& position, float length, const Vec3& rotation,
						const Rgba& start_color, const Rgba& end_color,
						float width = 0.025f);

// 	// TODO: be able to draw from one point in 3D space to the other point in 3D space
// 	void DebugRenderLine(float duration, eDebugRenderMode mode,
// 	                     Vec3 p0, Vec3 p1,
// 	                     Rgba start_color, Rgba end_color,
// 	                     float width = 0.025f);
// 
// 	// TODO: Command to render a spherical wire volume in 3D space 
// 	void DebugRenderWireSphere(float duration, eDebugRenderMode mode,
// 	                           Vec3 pos,
// 	                           float radius,
// 	                           Rgba start_color, Rgba end_color);

	// Command to render billboarded text in 3D space (for a given time, start color, end color, and pivot point)
	// [X05.41  2%] Render Text in World (non billboarded)
	// [X05.43  2%] Option to include a background behind rendered text (color with alpha)
	void DebugRenderText(float duration, eDebugRenderMode mode, const Rgba& start_color, const Rgba& end_color, float cell_height,
						  const std::string& text, const Vec2& alignment = AABB2::ALIGN_CENTERED,
						  const AABB2& local_box = g_defaultTextBox, const Vec3& position = Vec3::ZERO, const Vec3& rotations = Vec3::ZERO, bool is_billboard = false, const Rgba& box_color = Rgba::CLEAR, 
						  float cell_aspect = 1.f, BitmapFont::TextDrawMode text_draw_mode = BitmapFont::TEXT_DRAW_SHRINK_TO_FIT,
						  int max_glyphs_to_draw = 99999999);

	void DebugRenderTextv(float duration, eDebugRenderMode mode, const Rgba& start_color, const Rgba& end_color, float cell_height,
						char const *format, va_list arguments, const Vec2& alignment = AABB2::ALIGN_CENTERED,
						const AABB2& local_box = g_defaultTextBox, const Vec3& position = Vec3::ZERO, const Vec3& rotations = Vec3::ZERO, bool is_billboard = false, const Rgba& box_color = Rgba::CLEAR, 
						float cell_aspect = 1.f, BitmapFont::TextDrawMode text_draw_mode = BitmapFont::TEXT_DRAW_SHRINK_TO_FIT,
						int max_glyphs_to_draw = 99999999);

	void DebugRenderTextf(float duration, eDebugRenderMode mode, const Rgba& start_color, const Rgba& end_color, float cell_height,
							char const *format, ...);

	void DebugRenderTextf(float duration, eDebugRenderMode mode, const Rgba& start_color, const Rgba& end_color, float cell_height,
							const AABB2& local_box, const Vec3& position, const Vec3& rotations, const Rgba& box_color, char const *format, ...);

	void DebugRenderTextf(float duration, eDebugRenderMode mode, const Rgba& start_color, const Rgba& end_color, float cell_height,
		const AABB2& local_box, const Vec3& position, const Rgba& box_color, char const *format, ...);


	// [X05.20  2%] DebugRenderSphere
	void DebugRenderSphere(float duration, eDebugRenderMode mode,
							const Vec3& pos,
							float radius,
							Rgba start_color, Rgba end_color);

	void DebugRenderSphere(float duration, eDebugRenderMode mode,
		const Vec3& pos,
		float radius,
		Rgba start_color, Rgba end_color, int wedges, int slices);

	// [X05.21  2%] DebugRenderBox 
	void DebugRenderBox(float duration, eDebugRenderMode mode,
						const AABB3& cube,
						Rgba start_color, Rgba end_color, TextureView* view = nullptr);

	// [X05.21  2%] DebugRenderBox 
	void DebugRenderBox(float duration, eDebugRenderMode mode,
						const OBB3& cube,
						const Rgba& start_color, const Rgba& end_color);


	//[X05.50  2%] DebugRenderCylinder
	void DebugRenderCylinder(float duration, eDebugRenderMode mode,
							 Vec3 position, float length, Vec3 rotation, float radius,
							 Rgba start_color, Rgba end_color);

	void DebugRenderCapsule(float duration, eDebugRenderMode mode,
			Vec3 position, float length, Vec3 rotation, float radius,
			Rgba start_color, Rgba end_color);

	void DebugRenderCapsule(float duration, const eDebugRenderMode& mode,
		const Capsule3& cap, const Rgba& start_color, const Rgba& end_color);

	// [X05.51  2%] DebugRenderCone
	void DebugRenderCone(float duration, eDebugRenderMode mode,
						 Vec3 position, float length, Vec3 rotation, float radius,
						 Rgba start_color, Rgba end_color);

	//[X05.22  2%] DrawArrow
	void DebugRenderArrow(float duration, eDebugRenderMode mode,
						  Vec3 position, float length, Vec3 rotation, float radius,
						  Rgba start_color, Rgba end_color);

	// [X05.23  2%] DrawBasis
	void DebugRenderBasis(float duration, eDebugRenderMode mode);

	// [X05.24  2%] DrawGrid  (3D)
	void DebugRenderGrid(float duration, eDebugRenderMode mode, unsigned int grid_length);

	//------------------------------------------------------------------------
	// 2D


	//------------------------------------------------------------------------
	void DebugRenderScreenPoint(float duration,
	                            const Vec2& pos,
	                            const Rgba& start_color, const Rgba& end_color,
	                            float size = 8.0f);

	void DebugRenderScreenQuad(float duration,
	                           const AABB2& quad,
	                           const Rgba& start_color, const Rgba& end_color,
	                           TextureView* view = nullptr);

	void DebugRenderScreenQuad(float duration,
								const OBB2& quad,
								const Rgba& start_color, const Rgba& end_color,
								TextureView* view = nullptr);

	void DebugRenderScreenLine(float duration,
	                           const Vec2& p0, const Vec2& p1,
	                           const Rgba& start_color, const Rgba& end_color,
	                           float width = 0.025f);

	//[X05.31  2%] DebugRenderScreenDisc
	void DebugRenderScreenRing(float duration,
	                           const Vec2& center,
	                           const Rgba& start_color, const Rgba& end_color,
	                           float radius, float thickness, int num_sides = 16);

	//[X05.43  2%] Option to include a background behind rendered text (color with alpha)
	void DebugRenderScreenTextBoxv(float duration, const Rgba& start_color, const Rgba& end_color, float cell_height,
									char const *format, va_list arguments, const Vec2& alignment = AABB2::ALIGN_CENTERED, 
									const AABB2& box = g_defaultScreenBoarder, const Rgba& box_color = Rgba::CLEAR, float cell_aspect = 1.f,
									BitmapFont::TextDrawMode mode = BitmapFont::TEXT_DRAW_SHRINK_TO_FIT, int max_glyphs_to_draw = 99999999);

	void DebugRenderScreenTextBoxf(float duration, const Rgba& start_color, const Rgba& end_color, float cell_height, char const *format, ...);
	
	void DebugRenderScreenTextBoxf(float duration, const Rgba& start_color, const Rgba& end_color, float cell_height, const Vec2& alignment,
									char const *format, ...);



	//[X05.30  2%] DebugRenderScreenArrow
	void DebugRenderScreenArrow(float duration,
								const Vec2& p0, const Vec2& p1,
								const Rgba& start_color, const Rgba& end_color,
								float size = 8.0f);

	// [X05.24  2%] DrawGrid (2D)
	void DebugRenderScreenGrid(float duration, const Rgba& start_color, const AABB2& box = g_defaultScreenBoarder, 
							   float primary_stride = 144.0f, bool sub_divide = false);

	//------------------------------------------------------------------------
	// Message System (renders with screen items)
	//------------------------------------------------------------------------
	void DebugRenderMessagef(float duration, const Rgba& start_color, const Rgba& end_color, const char* format, ...);

	inline void SetFontSize(float size) {m_fontSize = size;}

private:
	void CheckToClear();
	void RenderDebugLog();

private:
	struct Debug3Dobj
	{
		Debug3Dobj(const CPUMesh& mesh, const Rgba& start_color, const Rgba& end_color, const float duration,
			const Vec3& position, const Vec3& rotation, const bool is_billboarded, const eDebugRenderMode mode, TextureView* texture):
			m_mesh(mesh), m_texture(texture), m_drawMode(mode),
			m_startColor(start_color), m_endColor(end_color), m_isBillboarded(is_billboarded), m_duration(duration)
		{
			if(IsZero(duration)) m_duration = 1.175494351e-38F;

			Matrix44 translation_matrix;

			translation_matrix = m_moodleMatrix.MakeTranslation3D(position);
			Matrix44 rotation_matrix;  
			rotation_matrix = m_moodleMatrix.ForEuler(rotation, ROTATION_ORDER_ZXY);

			m_moodleMatrix = rotation_matrix * translation_matrix;
			//m_moodleMatrix = m_moodleMatrix.ForEulerZXY(rotation, position);
		};

		CPUMesh m_mesh;
		TextureView* m_texture;
		eDebugRenderMode m_drawMode;
		Matrix44 m_moodleMatrix =  Matrix44::IDENTITY;
		Rgba m_startColor;
		Rgba m_endColor;
		bool m_isBillboarded = false;
		float m_duration;
		float m_currentTime = 0.0f;
	};

	struct Debug2Dobj
	{
		Debug2Dobj(const CPUMesh& mesh, const Rgba& start_color, const Rgba& end_color, const float duration,
		           const Vec3& position, const Vec3& rotation, TextureView* texture):
			m_mesh(mesh), m_texture(texture), m_position(position), m_rotation(rotation), m_startColor(start_color),
			m_endColor(end_color), m_duration(duration)
		{
			if(IsZero(duration)) m_duration = 1.175494351e-38F;
		};

		CPUMesh m_mesh;
		TextureView* m_texture;
		Vec3 m_position;
		Vec3 m_rotation;
		Rgba m_startColor;
		Rgba m_endColor;
		float m_duration;
		float m_currentTime = 0.0f;
	};

	struct DebugLine
	{
		DebugLine(const std::string& line, const Rgba& start_color, const Rgba& end_color, const float duration):
			m_line(line), m_startColor(start_color), m_endColor(end_color), m_duration(duration)
		{
			//if(IsZero(duration)) m_duration = 1.175494351e-38F;
		};

		std::string m_line;
		Rgba m_startColor;
		Rgba m_endColor;
		float m_duration;
		float m_currentTime = 0.0f;
	};

	RenderContext* m_gameRenderContext = nullptr;
	const TextureView2D* m_defaultTexture = nullptr;
	AABB2 m_defaultUVBounds;
	Camera* m_debugLogCamera = nullptr;
	Camera* m_gameCamera = nullptr;

	std::string m_fontFile = "16x16-Dwarf.png";
	std::vector<DebugLine> m_debugLog;
	std::vector<Debug2Dobj> m_debug2Dobjs;
	std::vector<Debug3Dobj> m_debug3Dobjs;

	float m_fontSize = 100.0f;
	int m_frameNumber = 0;

	float m_timeBeginFrame = 0.0f;
	float m_timeEndFrame = 0.0f;

	Shader* m_alwaysShader = nullptr;
	Shader* m_lessEqualShader = nullptr;
	Shader* m_greaterShader = nullptr;
	Shader* m_wireframeShader = nullptr;

	GPUMesh* m_logMesh;
};
