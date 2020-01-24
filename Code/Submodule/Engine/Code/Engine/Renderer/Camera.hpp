#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Matrix44.hpp"
#include "Engine/Math/Ray3.hpp"

class DepthStencilTargetView;
class ColorTargetView;
class RenderContext;
class UniformBuffer;

class Camera
{
	friend class RenderContext;

public:
	Camera();
	~Camera();

	void SetOrthoView(const Vec2& bottom_left, const Vec2& top_right);
	void SetPerspectiveProjection(float field_of_view_degrees, float aspect, float near_plane, float far_plane);
	void SetDepthStencilTarget( DepthStencilTargetView* dsv );

	Vec2 GetOrthoBottomLeft() const;
	Vec2 GetOrthoTopRight() const;

	void TranslateXY(const Vec2& translate_vector);
	void SetColorTarget(ColorTargetView* ctv);
	ColorTargetView* GetColorTargetView() const { return m_colorTarget; }
	void UpdateUniformBuffer(RenderContext* ctx);
	void SetModelMatrix(const Matrix44& model);
	Matrix44 GetModelMatrix() const;
	Matrix44 GetProjectionMatrix() const;

	Ray3 ScreenPointToWorldRay( const IntVec2& client_point ) const;

private:
 	Vec2 m_bottomLeft = Vec2::ZERO;
 	Vec2 m_topRight = Vec2::ZERO;

	Matrix44 m_view = Matrix44::IDENTITY;
	Matrix44 m_projection = Matrix44::IDENTITY;
	Matrix44 m_model = Matrix44::IDENTITY;
	float m_fieldOfViewDegrees = 90.0f;

	Vec2 m_depth = Vec2::ZERO;
	Vec2 m_translateVector = Vec2::ZERO;
	ColorTargetView* m_colorTarget = nullptr;
	DepthStencilTargetView* m_depthStencilTarget = nullptr; 

	// UniformBuffer that will store our camera information
	UniformBuffer* m_cameraUBO = nullptr; // UBO -> Uniform Buffer Object
	UniformBuffer* m_timeUBO = nullptr; // UBO -> Uniform Buffer Object
};
