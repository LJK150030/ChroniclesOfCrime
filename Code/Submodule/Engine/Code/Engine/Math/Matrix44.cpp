#include "Engine/Math/Matrix44.hpp"
#include "Engine/EngineCommon.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Vec4.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include <vector>
#include "Engine/Core/StringUtils.hpp"

const STATIC Matrix44 Matrix44::IDENTITY = 
	Matrix44(1.0f, 0.0f, 0.0f, 0.0f,
	 0.0f, 1.0f, 0.0f, 0.0f,
	 0.0f, 0.0f, 1.0f, 0.0f,
	 0.0f, 0.0f, 0.0f, 1.0f);

Matrix44::Matrix44()
{
	m_values[I_X] = 1.0f;
	m_values[I_Y] = 0.0f;
	m_values[I_Z] = 0.0f;
	m_values[I_W] = 0.0f;

	m_values[J_X] = 0.0f;
	m_values[J_Y] = 1.0f;
	m_values[J_Z] = 0.0f;
	m_values[J_W] = 0.0f;

	m_values[K_X] = 0.0f;
	m_values[K_Y] = 0.0f;
	m_values[K_Z] = 1.0f;
	m_values[K_W] = 0.0f;

	m_values[T_X] = 0.0f;
	m_values[T_Y] = 0.0f;
	m_values[T_Z] = 0.0f;
	m_values[T_W] = 1.0f;
}

Matrix44::Matrix44(const Vec2& i_basis, const Vec2& j_basis, const Vec2& translation_2d)
{
	m_values[I_X] = i_basis.x;
	m_values[I_Y] = i_basis.y;
	m_values[I_Z] = 0.0f;
	m_values[I_W] = 0.0f;

	m_values[J_X] = j_basis.x;
	m_values[J_Y] = j_basis.y;
	m_values[J_Z] = 0.0f;
	m_values[J_W] = 0.0f;

	m_values[K_X] = 0.0f;
	m_values[K_Y] = 0.0f;
	m_values[K_Z] = 1.0f;
	m_values[K_W] = 0.0f;

	m_values[T_X] = translation_2d.x;
	m_values[T_Y] = translation_2d.y;
	m_values[T_Z] = 0.0f;
	m_values[T_W] = 1.0f;
}

Matrix44::Matrix44(const Vec3& i_basis, const Vec3& j_basis, const Vec3& k_basis, const Vec3& translation_3d)
{
	m_values[I_X] = i_basis.x;
	m_values[I_Y] = i_basis.y;
	m_values[I_Z] = i_basis.z;
	m_values[I_W] = 0.0f;

	m_values[J_X] = j_basis.x;
	m_values[J_Y] = j_basis.y;
	m_values[J_Z] = j_basis.z;
	m_values[J_W] = 0.0f;

	m_values[K_X] = k_basis.x;
	m_values[K_Y] = k_basis.y;
	m_values[K_Z] = k_basis.z;
	m_values[K_W] = 0.0f;

	m_values[T_X] = translation_3d.x;
	m_values[T_Y] = translation_3d.y;
	m_values[T_Z] = translation_3d.z;
	m_values[T_W] = 1.0f;
}

Matrix44::Matrix44(const Vec4& i_basis, const Vec4& j_basis, const Vec4& k_basis, const Vec4& translation_4d)
{
	m_values[I_X] = i_basis.x;
	m_values[I_Y] = i_basis.y;
	m_values[I_Z] = i_basis.z;
	m_values[I_W] = i_basis.w;

	m_values[J_X] = j_basis.x;
	m_values[J_Y] = j_basis.y;
	m_values[J_Z] = j_basis.z;
	m_values[J_W] = j_basis.w;

	m_values[K_X] = k_basis.x;
	m_values[K_Y] = k_basis.y;
	m_values[K_Z] = k_basis.z;
	m_values[K_W] = k_basis.w;

	m_values[T_X] = translation_4d.x;
	m_values[T_Y] = translation_4d.y;
	m_values[T_Z] = translation_4d.z;
	m_values[T_W] = translation_4d.w;
}

Matrix44::Matrix44(const float param[16])
{
	m_values[I_X] = param[0];
	m_values[I_Y] = param[1];
	m_values[I_Z] = param[2];
	m_values[I_W] = param[3];

	m_values[J_X] = param[4];
	m_values[J_Y] = param[5];
	m_values[J_Z] = param[6];
	m_values[J_W] = param[7];

	m_values[K_X] = param[8];
	m_values[K_Y] = param[9];
	m_values[K_Z] = param[10];
	m_values[K_W] = param[11];

	m_values[T_X] = param[12];
	m_values[T_Y] = param[13];
	m_values[T_Z] = param[14];
	m_values[T_W] = param[15];
}

Matrix44::Matrix44(const float i_x, const float i_y, const float i_z, const float i_w, 
				   const float j_x, const float j_y, const float j_z, const float j_w, 
				   const float k_x,	const float k_y, const float k_z, const float k_w, 
				   const float t_x, const float t_y, const float t_z, const float t_w)
{
	m_values[I_X] = i_x;
	m_values[I_Y] = i_y;
	m_values[I_Z] = i_z;
	m_values[I_W] = i_w;

	m_values[J_X] = j_x;
	m_values[J_Y] = j_y;
	m_values[J_Z] = j_z;
	m_values[J_W] = j_w;

	m_values[K_X] = k_x;
	m_values[K_Y] = k_y;
	m_values[K_Z] = k_z;
	m_values[K_W] = k_w;

	m_values[T_X] = t_x;
	m_values[T_Y] = t_y;
	m_values[T_Z] = t_z;
	m_values[T_W] = t_w;
}

const Vec2 Matrix44::GetPosition2D() const
{
	return Vec2(m_values[T_X], m_values[T_Y]);
}

const Vec2 Matrix44::GetTransformVector2D(const Vec2& vector_quantity) const
{
	const float x = (vector_quantity.x * m_values[I_X]) + (vector_quantity.y * m_values[J_X]);
	const float y = (vector_quantity.x * m_values[I_Y]) + (vector_quantity.y * m_values[J_Y]);

	return Vec2(x, y);
}

const Vec2 Matrix44::GetTransformPosition2D(const Vec2& position) const
{
	const float x = (position.x * m_values[I_X]) + (position.y * m_values[J_X]) + m_values[T_X];
	const float y = (position.x * m_values[I_Y]) + (position.y * m_values[J_Y]) + m_values[T_Y];

	return Vec2(x, y);
}

const Vec3 Matrix44::GetPosition3D() const
{
	return Vec3(m_values[T_X], m_values[T_Y], m_values[T_Z]);
}

const Vec3 Matrix44::GetTransformVector3D(const Vec3& vector_quantity) const
{
	const float x = (vector_quantity.x * m_values[I_X]) + (vector_quantity.y * m_values[J_X]) + (vector_quantity.z *
		m_values[K_X]);
	const float y = (vector_quantity.x * m_values[I_Y]) + (vector_quantity.y * m_values[J_Y]) + (vector_quantity.z *
		m_values[K_Y]);
	const float z = (vector_quantity.x * m_values[I_Z]) + (vector_quantity.y * m_values[J_Z]) + (vector_quantity.z *
		m_values[K_Z]);

	return Vec3(x, y, z);
}

const Vec3 Matrix44::GetTransformPosition3D(const Vec3& position) const
{
	const float x = (position.x * m_values[I_X]) + (position.y * m_values[J_X]) + (position.z * m_values[K_X]) +
		m_values[T_X];
	const float y = (position.x * m_values[I_Y]) + (position.y * m_values[J_Y]) + (position.z * m_values[K_Y]) +
		m_values[T_Y];
	const float z = (position.x * m_values[I_Z]) + (position.y * m_values[J_Z]) + (position.z * m_values[K_Z]) +
		m_values[T_Z];

	return Vec3(x, y, z);
}

const Vec4 Matrix44::GetTransformHomogeneousPoint3D(const Vec4& vector_quantity) const
{
	const float x = (vector_quantity.x * m_values[I_X]) + (vector_quantity.y * m_values[J_X]) + (vector_quantity.z *
		m_values[K_X]) + (vector_quantity.w * m_values[T_X]);
	const float y = (vector_quantity.x * m_values[I_Y]) + (vector_quantity.y * m_values[J_Y]) + (vector_quantity.z *
		m_values[K_Y]) + (vector_quantity.w * m_values[T_Y]);
	const float z = (vector_quantity.x * m_values[I_Z]) + (vector_quantity.y * m_values[J_Z]) + (vector_quantity.z *
		m_values[K_Z]) + (vector_quantity.w * m_values[T_Z]);
	const float w = (vector_quantity.x * m_values[I_W]) + (vector_quantity.y * m_values[J_W]) + (vector_quantity.z *
		m_values[K_W]) + (vector_quantity.w * m_values[T_W]);

	return Vec4(x, y, z, w);
}

const Vec3 Matrix44::GetIvec3() const
{
	return Vec3(m_values[I_X], m_values[I_Y], m_values[I_Z]);
}

const Vec3 Matrix44::GetJvec3() const
{
	return Vec3(m_values[J_X], m_values[J_Y], m_values[J_Z]);
}

const Vec3 Matrix44::GetKvec3() const
{
	return Vec3(m_values[K_X], m_values[K_Y], m_values[K_Z]);
}

const Vec3 Matrix44::GetTvec3() const
{
	return Vec3(m_values[T_X], m_values[T_Y], m_values[T_Z]);
}

STATIC const Matrix44 Matrix44::MakeZRotationDegrees(const float degrees_about_z)
{
	const float c = CosDegrees(degrees_about_z);
	const float s = SinDegrees(degrees_about_z);

	Matrix44 rotation_matrix;
	rotation_matrix.m_values[I_X] = c;
	rotation_matrix.m_values[I_Y] = s;
	rotation_matrix.m_values[J_X] = -s;
	rotation_matrix.m_values[J_Y] = c;
	return rotation_matrix;
}

STATIC const Matrix44 Matrix44::MakeYRotationDegrees(float degrees_about_y)
{
	const float c = CosDegrees(degrees_about_y);
	const float s = SinDegrees(degrees_about_y);

	Matrix44 rotation_matrix;
	rotation_matrix.m_values[I_X] = c;
	rotation_matrix.m_values[I_Z] = -s;
	rotation_matrix.m_values[K_X] = s;
	rotation_matrix.m_values[K_Z] = c;
	return rotation_matrix;
}

STATIC const Matrix44 Matrix44::MakeXRotationDegrees(float degrees_about_x)
{
	const float c = CosDegrees(degrees_about_x);
	const float s = SinDegrees(degrees_about_x);

	Matrix44 rotation_matrix;
	rotation_matrix.m_values[J_Y] = c;
	rotation_matrix.m_values[J_Z] = s;
	rotation_matrix.m_values[K_Y] = -s;
	rotation_matrix.m_values[K_Z] = c;
	return rotation_matrix;
}

STATIC const Matrix44 Matrix44::MakeTranslation2D(const Vec2& translation_xy)
{
	Matrix44 translation_matrix;
	translation_matrix.m_values[T_X] = translation_xy.x;
	translation_matrix.m_values[T_Y] = translation_xy.y;
	return translation_matrix;
}

STATIC const Matrix44 Matrix44::MakeUniformScale2D(const float uniform_scale_xy)
{
	Matrix44 scale_matrix;
	scale_matrix.m_values[I_X] = uniform_scale_xy;
	scale_matrix.m_values[J_Y] = uniform_scale_xy;
	return scale_matrix;
}

const Matrix44 Matrix44::MakeTranslation3D(const Vec3& translation_xyz)
{
	Matrix44 translation_matrix;
	translation_matrix.m_values[T_X] = translation_xyz.x;
	translation_matrix.m_values[T_Y] = translation_xyz.y;
	translation_matrix.m_values[T_Z] = translation_xyz.z;
	return translation_matrix;
}

const Matrix44 Matrix44::MakeUniformScale3D(float uniform_scale_xyz)
{
	Matrix44 scale_matrix;
	scale_matrix.m_values[I_X] = uniform_scale_xyz;
	scale_matrix.m_values[J_Y] = uniform_scale_xyz;
	scale_matrix.m_values[K_Z] = uniform_scale_xyz;
	return scale_matrix;
}

Matrix44 Matrix44::GetInverseMatrix() const
{
	Matrix44 temp_matrix(m_values);
	temp_matrix.Inverse();
	return temp_matrix;
}

Matrix44 Matrix44::GetRotationMatrix() const
{
	Matrix44 rotation_matrix;
	rotation_matrix.m_values[I_X] = m_values[I_X];
	rotation_matrix.m_values[I_Y] = m_values[I_Y];
	rotation_matrix.m_values[I_Z] = m_values[I_Z];
	rotation_matrix.m_values[J_X] = m_values[J_X];
	rotation_matrix.m_values[J_Y] = m_values[J_Y];
	rotation_matrix.m_values[J_Z] = m_values[J_Z];
	rotation_matrix.m_values[K_X] = m_values[K_X];
	rotation_matrix.m_values[K_Y] = m_values[K_Y];
	rotation_matrix.m_values[K_Z] = m_values[K_Z];
	return rotation_matrix;
}

void Matrix44::SetRotationMatrix(const Matrix44& new_rotation_matrix)
{
	m_values[I_X] = new_rotation_matrix.m_values[I_X];
	m_values[I_Y] = new_rotation_matrix.m_values[I_Y];
	m_values[I_Z] = new_rotation_matrix.m_values[I_Z];
	m_values[J_X] = new_rotation_matrix.m_values[J_X];
	m_values[J_Y] = new_rotation_matrix.m_values[J_Y];
	m_values[J_Z] = new_rotation_matrix.m_values[J_Z];
	m_values[K_X] = new_rotation_matrix.m_values[K_X];
	m_values[K_Y] = new_rotation_matrix.m_values[K_Y];
	m_values[K_Z] = new_rotation_matrix.m_values[K_Z];
}

void Matrix44::SetIvec3(const Vec3& vec)
{
	m_values[I_X] = vec.x;
	m_values[I_Y] = vec.y;
	m_values[I_Z] = vec.z;
	m_values[I_W] = 0.0f;
}

void Matrix44::SetJvec3(const Vec3& vec)
{
	m_values[J_X] = vec.x;
	m_values[J_Y] = vec.y;
	m_values[J_Z] = vec.z;
	m_values[J_W] = 0.0f;
}

void Matrix44::SetKvec3(const Vec3& vec)
{
	m_values[K_X] = vec.x;
	m_values[K_Y] = vec.y;
	m_values[K_Z] = vec.z;
	m_values[K_W] = 0.0f;
}

void Matrix44::SetTvec3(const Vec3& vec)
{
	m_values[T_X] = vec.x;
	m_values[T_Y] = vec.y;
	m_values[T_Z] = vec.z;
	m_values[T_W] = 1.0f;
}

void Matrix44::Inverse()
{
	float inv[16];

	inv[0] = m_values[5]  * m_values[10] * m_values[15] - 
		m_values[5]  * m_values[11] * m_values[14] - 
		m_values[9]  * m_values[6]  * m_values[15] + 
		m_values[9]  * m_values[7]  * m_values[14] +
		m_values[13] * m_values[6]  * m_values[11] - 
		m_values[13] * m_values[7]  * m_values[10];

	inv[4] = -m_values[4]  * m_values[10] * m_values[15] + 
		m_values[4]  * m_values[11] * m_values[14] + 
		m_values[8]  * m_values[6]  * m_values[15] - 
		m_values[8]  * m_values[7]  * m_values[14] - 
		m_values[12] * m_values[6]  * m_values[11] + 
		m_values[12] * m_values[7]  * m_values[10];

	inv[8] = m_values[4]  * m_values[9] * m_values[15] - 
		m_values[4]  * m_values[11] * m_values[13] - 
		m_values[8]  * m_values[5] * m_values[15] + 
		m_values[8]  * m_values[7] * m_values[13] + 
		m_values[12] * m_values[5] * m_values[11] - 
		m_values[12] * m_values[7] * m_values[9];

	inv[12] = -m_values[4]  * m_values[9] * m_values[14] + 
		m_values[4]  * m_values[10] * m_values[13] +
		m_values[8]  * m_values[5] * m_values[14] - 
		m_values[8]  * m_values[6] * m_values[13] - 
		m_values[12] * m_values[5] * m_values[10] + 
		m_values[12] * m_values[6] * m_values[9];

	inv[1] = -m_values[1]  * m_values[10] * m_values[15] + 
		m_values[1]  * m_values[11] * m_values[14] + 
		m_values[9]  * m_values[2] * m_values[15] - 
		m_values[9]  * m_values[3] * m_values[14] - 
		m_values[13] * m_values[2] * m_values[11] + 
		m_values[13] * m_values[3] * m_values[10];

	inv[5] = m_values[0]  * m_values[10] * m_values[15] - 
		m_values[0]  * m_values[11] * m_values[14] - 
		m_values[8]  * m_values[2] * m_values[15] + 
		m_values[8]  * m_values[3] * m_values[14] + 
		m_values[12] * m_values[2] * m_values[11] - 
		m_values[12] * m_values[3] * m_values[10];

	inv[9] = -m_values[0]  * m_values[9] * m_values[15] + 
		m_values[0]  * m_values[11] * m_values[13] + 
		m_values[8]  * m_values[1] * m_values[15] - 
		m_values[8]  * m_values[3] * m_values[13] - 
		m_values[12] * m_values[1] * m_values[11] + 
		m_values[12] * m_values[3] * m_values[9];

	inv[13] = m_values[0]  * m_values[9] * m_values[14] - 
		m_values[0]  * m_values[10] * m_values[13] - 
		m_values[8]  * m_values[1] * m_values[14] + 
		m_values[8]  * m_values[2] * m_values[13] + 
		m_values[12] * m_values[1] * m_values[10] - 
		m_values[12] * m_values[2] * m_values[9];

	inv[2] = m_values[1]  * m_values[6] * m_values[15] - 
		m_values[1]  * m_values[7] * m_values[14] - 
		m_values[5]  * m_values[2] * m_values[15] + 
		m_values[5]  * m_values[3] * m_values[14] + 
		m_values[13] * m_values[2] * m_values[7] - 
		m_values[13] * m_values[3] * m_values[6];

	inv[6] = -m_values[0]  * m_values[6] * m_values[15] + 
		m_values[0]  * m_values[7] * m_values[14] + 
		m_values[4]  * m_values[2] * m_values[15] - 
		m_values[4]  * m_values[3] * m_values[14] - 
		m_values[12] * m_values[2] * m_values[7] + 
		m_values[12] * m_values[3] * m_values[6];

	inv[10] = m_values[0]  * m_values[5] * m_values[15] - 
		m_values[0]  * m_values[7] * m_values[13] - 
		m_values[4]  * m_values[1] * m_values[15] + 
		m_values[4]  * m_values[3] * m_values[13] + 
		m_values[12] * m_values[1] * m_values[7] - 
		m_values[12] * m_values[3] * m_values[5];

	inv[14] = -m_values[0]  * m_values[5] * m_values[14] + 
		m_values[0]  * m_values[6] * m_values[13] + 
		m_values[4]  * m_values[1] * m_values[14] - 
		m_values[4]  * m_values[2] * m_values[13] - 
		m_values[12] * m_values[1] * m_values[6] + 
		m_values[12] * m_values[2] * m_values[5];

	inv[3] = -m_values[1] * m_values[6] * m_values[11] + 
		m_values[1] * m_values[7] * m_values[10] + 
		m_values[5] * m_values[2] * m_values[11] - 
		m_values[5] * m_values[3] * m_values[10] - 
		m_values[9] * m_values[2] * m_values[7] + 
		m_values[9] * m_values[3] * m_values[6];

	inv[7] = m_values[0] * m_values[6] * m_values[11] - 
		m_values[0] * m_values[7] * m_values[10] - 
		m_values[4] * m_values[2] * m_values[11] + 
		m_values[4] * m_values[3] * m_values[10] + 
		m_values[8] * m_values[2] * m_values[7] - 
		m_values[8] * m_values[3] * m_values[6];

	inv[11] = -m_values[0] * m_values[5] * m_values[11] + 
		m_values[0] * m_values[7] * m_values[9] + 
		m_values[4] * m_values[1] * m_values[11] - 
		m_values[4] * m_values[3] * m_values[9] - 
		m_values[8] * m_values[1] * m_values[7] + 
		m_values[8] * m_values[3] * m_values[5];

	inv[15] = m_values[0] * m_values[5] * m_values[10] - 
		m_values[0] * m_values[6] * m_values[9] - 
		m_values[4] * m_values[1] * m_values[10] + 
		m_values[4] * m_values[2] * m_values[9] + 
		m_values[8] * m_values[1] * m_values[6] - 
		m_values[8] * m_values[2] * m_values[5];

	float det = m_values[0] * inv[0] + m_values[1] * inv[4] + m_values[2] * inv[8] + m_values[3] * inv[12];

	ASSERT_OR_DIE(det != 0, "Matrix44::Inverse() -- singular matrix")

	det = 1.0f / det;

	for (int i = 0; i < 16; i++)
		m_values[i] = inv[i] * det;
}

float Matrix44::Determinant()
{
	return m_values[-I_Z] * m_values[J_Y] * m_values[K_X] * m_values[T_W] + 
		m_values[I_Y] * m_values[J_Z] * m_values[K_X] * m_values[T_W] + 
		m_values[I_Z] * m_values[J_X] * m_values[K_Y] * m_values[T_W] - 
		m_values[I_X] * m_values[J_Z] * m_values[K_Y] * m_values[T_W] - 
		m_values[I_Y] * m_values[J_X] * m_values[K_Z] * m_values[T_W] + 
		m_values[I_X] * m_values[J_Y] * m_values[K_Z] * m_values[T_W] + 
		m_values[I_Z] * m_values[J_Y] * m_values[K_W] * m_values[T_X] - 
		m_values[I_Y] * m_values[J_Z] * m_values[K_W] * m_values[T_X] - 
		m_values[I_Z] * m_values[J_W] * m_values[K_Y] * m_values[T_X] + 
		m_values[I_W] * m_values[J_Z] * m_values[K_Y] * m_values[T_X] + 
		m_values[I_Y] * m_values[J_W] * m_values[K_Z] * m_values[T_X] - 
		m_values[I_W] * m_values[J_Y] * m_values[K_Z] * m_values[T_X] - 
		m_values[I_Z] * m_values[J_X] * m_values[K_W] * m_values[T_Y] + 
		m_values[I_X] * m_values[J_Z] * m_values[K_W] * m_values[T_Y] + 
		m_values[I_Z] * m_values[J_W] * m_values[K_X] * m_values[T_Y] - 
		m_values[I_W] * m_values[J_Z] * m_values[K_X] * m_values[T_Y] - 
		m_values[I_X] * m_values[J_W] * m_values[K_Z] * m_values[T_Y] + 
		m_values[I_W] * m_values[J_X] * m_values[K_Z] * m_values[T_Y] + 
		m_values[I_Y] * m_values[J_X] * m_values[K_W] * m_values[T_Z] - 
		m_values[I_X] * m_values[J_Y] * m_values[K_W] * m_values[T_Z] - 
		m_values[I_Y] * m_values[J_W] * m_values[K_X] * m_values[T_Z] + 
		m_values[I_W] * m_values[J_Y] * m_values[K_X] * m_values[T_Z] + 
		m_values[I_X] * m_values[J_W] * m_values[K_Y] * m_values[T_Z] - 
		m_values[I_W] * m_values[J_X] * m_values[K_Y] * m_values[T_Z];
}

float& Matrix44::operator()(const unsigned int row, const unsigned int col)
{
	return m_values[row + 4*col];
}

float& Matrix44::operator[](const unsigned index)
{
	return m_values[index];
}

Vec4 Matrix44::operator*(const Vec4& vector) const
{
	Vec4 resulting_vector = GetTransformHomogeneousPoint3D(vector);
	return resulting_vector;
}

//TODO: need to look over this again
Matrix44 Matrix44::operator*(Matrix44& matrix)
{
	Matrix44 new_matrix = IDENTITY;

	new_matrix[0] = matrix[0]*m_values[0]+( matrix[4]*m_values[1]+( matrix[8]*m_values[2]+ matrix[12]*m_values[3]));
	new_matrix[1] =	matrix[1]*m_values[0]+( matrix[5]*m_values[1]+( matrix[9]*m_values[2]+ matrix[13]*m_values[3]));
	new_matrix[2] =	matrix[2]*m_values[0]+( matrix[6]*m_values[1]+( matrix[10]*m_values[2]+ matrix[14]*m_values[3]));
	new_matrix[3] =	matrix[3]*m_values[0]+( matrix[7]*m_values[1]+( matrix[11]*m_values[2]+ matrix[15]*m_values[3]));
	new_matrix[4] =	matrix[0]*m_values[4]+( matrix[4]*m_values[5]+( matrix[8]*m_values[6]+ matrix[12]*m_values[7]));
	new_matrix[5] =	matrix[1]*m_values[4]+( matrix[5]*m_values[5]+( matrix[9]*m_values[6]+ matrix[13]*m_values[7]));
	new_matrix[6] =	matrix[2]*m_values[4]+( matrix[6]*m_values[5]+( matrix[10]*m_values[6]+ matrix[14]*m_values[7]));
	new_matrix[7] =	matrix[3]*m_values[4]+( matrix[7]*m_values[5]+( matrix[11]*m_values[6]+ matrix[15]*m_values[7]));
	new_matrix[8] =	matrix[0]*m_values[8]+( matrix[4]*m_values[9]+( matrix[8]*m_values[10]+ matrix[12]*m_values[11]));
	new_matrix[9] =	matrix[1]*m_values[8]+( matrix[5]*m_values[9]+( matrix[9]*m_values[10]+ matrix[13]*m_values[11]));
	new_matrix[10] =	matrix[2]*m_values[8]+( matrix[6]*m_values[9]+( matrix[10]*m_values[10]+ matrix[14]*m_values[11]));
	new_matrix[11] =	matrix[3]*m_values[8]+( matrix[7]*m_values[9]+( matrix[11]*m_values[10]+ matrix[15]*m_values[11]));
	new_matrix[12] =	matrix[0]*m_values[12]+( matrix[4]*m_values[13]+( matrix[8]*m_values[14]+ matrix[12]*m_values[15]));
	new_matrix[13] =	matrix[1]*m_values[12]+( matrix[5]*m_values[13]+( matrix[9]*m_values[14]+ matrix[13]*m_values[15]));
	new_matrix[14] =	matrix[2]*m_values[12]+( matrix[6]*m_values[13]+( matrix[10]*m_values[14]+ matrix[14]*m_values[15]));
	new_matrix[15] =	matrix[3]*m_values[12]+( matrix[7]*m_values[13]+( matrix[11]*m_values[14]+ matrix[15]*m_values[15]));

	return new_matrix;
}

Matrix44 Matrix44::operator+(const Matrix44& other_matrix)
{
	Matrix44 result;

	for(int i = 0; i < 16; ++i)
	{
		result.m_values[i] = m_values[i] + other_matrix.m_values[i];
	}

	return result;
}

Matrix44 Matrix44::operator*(const float scale_value)
{
	Matrix44 new_matrix = IDENTITY;
	
	new_matrix[0] =		m_values[0]  * scale_value;
	new_matrix[1] =		m_values[1]  * scale_value;
	new_matrix[2] =		m_values[2]  * scale_value;
	new_matrix[3] =		m_values[3]  * scale_value;
	new_matrix[4] =		m_values[4]  * scale_value;
	new_matrix[5] =		m_values[5]  * scale_value;
	new_matrix[6] =		m_values[6]  * scale_value;
	new_matrix[7] =		m_values[7]  * scale_value;
	new_matrix[8] =		m_values[8]  * scale_value;
	new_matrix[9] =		m_values[9]  * scale_value;
	new_matrix[10] =	m_values[10] * scale_value;
	new_matrix[11] =	m_values[11] * scale_value;
	new_matrix[12] =	m_values[12] * scale_value;
	new_matrix[13] =	m_values[13] * scale_value;
	new_matrix[14] =	m_values[14] * scale_value;
	new_matrix[15] =	m_values[15] * scale_value;

	return new_matrix;
}

//------------------------------------------------------------------------
STATIC const Matrix44 Matrix44::ForEuler(const Vec3& euler, const eRotationOrder rot ) 
{
	Matrix44 rot_x = MakeXRotationDegrees(euler.x); 
	Matrix44 rot_y = MakeYRotationDegrees(euler.y); 
	Matrix44 rot_z = MakeZRotationDegrees(euler.z); 

	// for games, we usually go around
	// forward, then right, then up
	Matrix44 ret = Matrix44::IDENTITY; 
	if (rot == ROTATION_ORDER_ZXY) {
		ret = ret * rot_z;
		ret = ret * rot_x; 
		ret = ret * rot_y; 
	}
	else if (rot == ROTATION_ORDER_XYZ){
		ret = ret * rot_x;
		ret = ret * rot_y; 
		ret = ret * rot_z; 
	} 
	else {
		// other cases
		ASSERT_RECOVERABLE(false, "Have yet to create a euler formula for given rotation order"); // implement unknown case
	}

	return ret; 
}

STATIC const Matrix44 Matrix44::ForEulerZXY(const Vec3& euler_degrees, const Vec3& translation)
{
	// at some point scale
	Matrix44 rotation_matrix = ForEuler(euler_degrees, ROTATION_ORDER_ZXY);
	Matrix44 translation_matrix = MakeTranslation3D(translation);

	const Matrix44 result = rotation_matrix * translation_matrix;

	return result;
}

//http://learnwebgl.brown37.net/08_projections/projections_ortho.html
const Matrix44 Matrix44::MakeProjectionOrthographic(const float left, const float right, const float bottom, const float top, const float near, const float far)
{
	const float sx = 1.0f / (right - left);
	const float sy = 1.0f / (top - bottom);
	const float sz = 1.0f / (far - near);

	return Matrix44(
		2.0f * sx,					0.0f,						0.0f,				0.0f,
		0.0f,						2.0f * sy,					0.0f,				0.0f,
		0.0f,						0.0f,						2.0f * sz,			0.0f,
		-(right + left) * sx,		-(top + bottom) * sy,		(far + near) * sz,	1.0f);
}

//http://learnwebgl.brown37.net/08_projections/projections_perspective.html
const Matrix44 Matrix44::MakeProjectionPerspective(float left, float right, float bottom, float top, float near, float far)
{
	const float sx = 1.0f / (right - left);
	const float sy = 1.0f / (top - bottom);
	const float sz = 1.0f / (far - near);

	return Matrix44(
		2.0f * near * sx,					0.0f,								0.0f,								0.0f,
		0.0f,								2.0f * near * sy,					0.0f,								0.0f,
		0.0f,								0.0f,								-(far + near) * sz,					-1.0f,
		-1.0f * near * (right + left) * sx,	-1.0f * near * (top + bottom) * sx,	2.0f * far * near / (near - far) ,	0.0f);
}

const Matrix44 Matrix44::MakeProjectionPerspective(const float fov_degrees, const float aspect, const float near_z, const float far_z)
{
	const float rads = ConvertDegreesToRadians(fov_degrees); 
	const float size = 1.0f / tanf(rads / 2.0f);

	float w = size;
	float h = size;

	// I always grow the side that is large (so fov is for the minimum dimension)
	// This is a personal choice - most engines will just always prefer either width
	// or height (usually height); 
	if (aspect > 1.0f) {
		w /= aspect;
	} else {
		h *= aspect;
	}

	const float q = 1.0f / (far_z - near_z);

	return Matrix44(
		w,          0.0f,         0.0f,								0.0f,
		0.0f,       h,            0.0f,								0.0f,
		0.0f,       0.0f,         (far_z) * q,						1.0f,
		0.0f,       0.0f,         -near_z * far_z * q,				0.0f
	);
}

const Matrix44 Matrix44::MakeTransformMat44(const std::string& transform)
{
	std::vector<std::string> transform_axis = SplitStringOnDelimiter(transform, ' ');

	ASSERT_OR_DIE(static_cast<int>(transform_axis.size()) == 3, "can not make transform matrix with given string");

	std::vector<Vec3> axis;
	axis.resize(3);

	for(int axis_id = 0; axis_id < 3; ++axis_id)
	{
		if(transform_axis[axis_id] == "x")
			axis[axis_id] = Vec3(1.0f, 0.0f, 0.0f);
		else if(transform_axis[axis_id] == "y")
			axis[axis_id] = (Vec3(0.0f, 1.0f, 0.0f));
		else if(transform_axis[axis_id] == "z")
			axis[axis_id] = (Vec3(0.0f, 0.0f, 1.0f));
		else if(transform_axis[axis_id] == "-x")
			axis[axis_id] = (Vec3(-1.0f, 0.0f, 0.0f));
		else if(transform_axis[axis_id] == "-y")
			axis[axis_id] = (Vec3(0.0f, -1.0f, 0.0f));
		else if(transform_axis[axis_id] == "-z")
			axis[axis_id] = (Vec3(0.0f, 0.0f, -1.0f));
	}

	Matrix44 mat44;
	mat44.SetIvec3(axis[0]);
	mat44.SetJvec3(axis[1]);
	mat44.SetKvec3(axis[2]);

	return mat44;
}
