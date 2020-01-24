#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/UniformBuffer.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/DepthStencilTargetView.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/WindowContext.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

Camera::Camera() = default;

Camera::~Camera()
{
	delete m_cameraUBO;
	m_cameraUBO = nullptr;

	delete m_timeUBO;
	m_timeUBO = nullptr;
}

void Camera::SetPerspectiveProjection(float field_of_view_degrees, float aspect, float near_plane, float far_plane)
{
	//m_bottomLeft = bottom_left;
	//m_topRight = top_right;
	m_translateVector = Vec2(0.0f, 0.0f);
	m_depth = Vec2(0.0f, far_plane - near_plane);
	m_fieldOfViewDegrees = field_of_view_degrees;

	ASSERT_OR_DIE(field_of_view_degrees < 180.0f, "cannot have a field of view greater than 180 degrees" );
	
	m_projection = Matrix44::MakeProjectionPerspective(field_of_view_degrees, aspect, near_plane, far_plane);
// 	float fovy = field_of_view_degrees * 0.5f;
// 	
// 	float top = ;
// 	float bottom =;
// 
// 	float left;
// 	float right;
// 	float near;
// 	float far;

//	m_projection = Matrix44::MakeProjectionPerspective(field_of_view_degrees, aspect, near_plane, far_plane);
}

void Camera::SetDepthStencilTarget(DepthStencilTargetView* dsv)
{
	m_depthStencilTarget = dsv;
}

void Camera::SetOrthoView(const Vec2& bottom_left, const Vec2& top_right)
{
	m_bottomLeft = bottom_left;
	m_topRight = top_right;
	m_translateVector = Vec2(0.0f, 0.0f);
	m_depth = Vec2(0.0f, 1.0f);
	m_fieldOfViewDegrees = 90.0f;

	m_projection = Matrix44::MakeProjectionOrthographic(bottom_left.x, top_right.x, bottom_left.y, top_right.y, NONZERO_ZERO, 1.0f);
}

Vec2 Camera::GetOrthoBottomLeft() const
{
	return m_bottomLeft + m_translateVector;
}

Vec2 Camera::GetOrthoTopRight() const
{
	return m_topRight + m_translateVector;
}

void Camera::TranslateXY(const Vec2& translate_vector)
{
	m_translateVector = translate_vector;
	Matrix44 translation_matrix = Matrix44::MakeTranslation2D(m_translateVector);
	m_model = translation_matrix * m_model;
	SetModelMatrix(m_model);

}

void Camera::SetColorTarget(ColorTargetView* ctv)
{
	m_colorTarget = ctv;
}

void Camera::UpdateUniformBuffer(RenderContext* ctx)
{
	if (m_cameraUBO == nullptr)
	{
		m_cameraUBO = new UniformBuffer(ctx);
	}
	if (m_timeUBO == nullptr)
	{
		m_timeUBO = new UniformBuffer(ctx);
	}

	// TODO: This method is called during BeginCamera - so we know our outputs
	// are final - so if you have enough data to figure out an aspect correct
	// projection, you can do it here;  For now, we'll keep with the SD1
	// version that just sets a fixed ortho; 
	CameraBufferT cpu_camera_data;
// 	cpu_camera_data.m_orthoMin = m_bottomLeft;
// 	cpu_camera_data.m_orthoMax = m_topRight;
	cpu_camera_data.m_view = m_view;
	cpu_camera_data.m_projection = m_projection;

	// copy the cpu to the gpu (will create or update the buffer)
	m_cameraUBO->CopyCPUToGPU(&cpu_camera_data, sizeof(cpu_camera_data));

	const float current_time = static_cast<float>(GetCurrentTimeSecondsF());
	FrameBufferT cpu_time_data;
	cpu_time_data.m_time = current_time;
	cpu_time_data.m_halfTime = 0.5f * current_time;
	cpu_time_data.m_doubleTime = 2.0f * current_time;
	cpu_time_data.m_quadTime = 4.0f * current_time;

	// copy the cpu to the gpu (will create or update the buffer)
	m_timeUBO->CopyCPUToGPU(&cpu_time_data, sizeof(cpu_time_data));
}

void Camera::SetModelMatrix(const Matrix44& model)
{
	m_model = model;
	m_view = m_model.GetInverseMatrix();
}

Matrix44 Camera::GetModelMatrix() const
{
	return m_model;
}

Matrix44 Camera::GetProjectionMatrix() const
{
	return m_projection;
}

// Math was confirmed with Scratchapixel Ray-Tracing: Generating Camera Rays
// https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-generating-camera-rays/generating-camera-rays
Ray3 Camera::ScreenPointToWorldRay(const IntVec2& client_point) const
{
	const AABB2 client_bounds = g_theWindow->m_windowBounds;
	//const float client_aspect = client_bounds.maxs.x / client_bounds.maxs.y;

	// remap the pixel from raster space (0:ImageWidth, ImageHeight:0) to NDC space (0:1, 1:0)
	const float pixel_ndc_x = (static_cast<float>(client_point.x) + 0.5f) / (client_bounds.maxs.x + 1.0f);
	const float pixel_ndc_y = (static_cast<float>(client_point.y) + 0.5f) / (client_bounds.maxs.y + 1.0f);

	// remap the pixel from NDC space (0:1, 1:0) to Screen space (-1:1, -1:1)
	const float pixel_screen_x = 2.0f * pixel_ndc_x - 1.0f;
	const float pixel_screen_y = 1.0f - 2.0f * pixel_ndc_y;

	// move the pixel into the camera local space
	//const float pixel_camera_x = pixel_screen_x * TanDegrees(m_fieldOfViewDegrees * 0.5f) * client_aspect;
	const float pixel_camera_x = pixel_screen_x * TanDegrees(m_fieldOfViewDegrees * 0.5f);
	const float pixel_camera_y = pixel_screen_y * TanDegrees(m_fieldOfViewDegrees * 0.5f);

	// Vec4 representation of of the near and far points
	Vec4 near_point(pixel_camera_x, pixel_camera_y, NONZERO_ZERO, 1.0f);
	Vec4 far_point(pixel_camera_x, pixel_camera_y, 1.0f, 1.0f);

	Matrix44 projection = m_projection;
	Matrix44 view = m_view;

	projection = projection.GetInverseMatrix();
	view = view.GetInverseMatrix();

	near_point = (projection * near_point);
	far_point = (projection * far_point);

	near_point = (view * near_point);
	far_point = (view * far_point);

	near_point /=  near_point.w;
	far_point /=  far_point.w;


	return Ray3(Vec3(near_point), Vec3(far_point - near_point));
}
