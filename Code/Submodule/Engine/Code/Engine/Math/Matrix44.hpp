#pragma once
#include <string>
#include "Engine/EngineCommon.hpp"

struct Vec2;
struct Vec3;
struct Vec4;

//	[ I_x  J_x  K_x  T_x  ]  [  0  4  8   12  ]
//	[ I_y  J_y  K_y  T_y  ]  [  1  5  9   13  ]
//	[ I_z  J_z  K_z  T_z  ]  [  2  6  10  14  ]
//	[ I_w  J_w  K_w  T_w  ]  [  3  7  11  15  ]
enum eRotationOrder
{
	NO_ROTATION_ORDER = -1,
	ROTATION_ORDER_XYZ,
	ROTATION_ORDER_XZY,
	ROTATION_ORDER_YXZ,
	ROTATION_ORDER_YZX,
	ROTATION_ORDER_ZXY,
	ROTATION_ORDER_ZYX,
	NUM_ROTATION_ORDERS
};


struct Matrix44
{
public:
	float m_values[16]{};

	enum Variables { I_X, I_Y, I_Z, I_W, J_X, J_Y, J_Z, J_W, K_X, K_Y, K_Z, K_W, T_X, T_Y, T_Z, T_W };
	enum Axis {I, J, K, W};
	const static Matrix44 IDENTITY;

public:
	explicit Matrix44();
	explicit Matrix44(const Vec2& i_basis, const Vec2& j_basis, const Vec2& translation_2d);
	explicit Matrix44(const Vec3& i_basis, const Vec3& j_basis, const Vec3& k_basis, const Vec3& translation_3d);
	explicit Matrix44(const Vec4& i_basis, const Vec4& j_basis, const Vec4& k_basis, const Vec4& translation_4d);
	explicit Matrix44(const float param[16]);
	explicit Matrix44(float i_x, float i_y, float i_z, float i_w, 
					  float j_x, float j_y, float j_z, float j_w, 
					  float k_x, float k_y, float k_z, float k_w, 
					  float t_x, float t_y, float t_z, float t_w);


	const Vec2 GetPosition2D() const;
	const Vec2 GetTransformVector2D(const Vec2& vector_quantity) const; // Assumes z = 0, and w = 0
	const Vec2 GetTransformPosition2D(const Vec2& position) const; // Assumes z = 0, and w = 1
	const Vec3 GetPosition3D() const;
	const Vec3 GetTransformVector3D(const Vec3& vector_quantity) const; // Assumes w = 0
	const Vec3 GetTransformPosition3D(const Vec3& position) const; // Assumes w = 1
	const Vec4 GetTransformHomogeneousPoint3D(const Vec4& vector_quantity) const;
	const Vec3 GetIvec3() const;
	const Vec3 GetJvec3() const;
	const Vec3 GetKvec3() const;
	const Vec3 GetTvec3() const;
	Matrix44 GetInverseMatrix() const;
	Matrix44 GetRotationMatrix() const;

	void SetRotationMatrix(const Matrix44& new_rotation_matrix);
	void SetIvec3(const Vec3& vec);
	void SetJvec3(const Vec3& vec);
	void SetKvec3(const Vec3& vec);
	void SetTvec3(const Vec3& vec);
	void Inverse();
	float Determinant();

	static const Matrix44 MakeZRotationDegrees(float degrees_about_z);
	static const Matrix44 MakeYRotationDegrees(float degrees_about_y);
	static const Matrix44 MakeXRotationDegrees(float degrees_about_x);
	static const Matrix44 MakeTranslation2D(const Vec2& translation_xy);
	static const Matrix44 MakeUniformScale2D(float uniform_scale_xy);
	static const Matrix44 MakeTranslation3D(const Vec3& translation_xyz);
	static const Matrix44 MakeUniformScale3D(float uniform_scale_xyz);
	static const Matrix44 ForEuler(const Vec3& euler, eRotationOrder rot );
	static const Matrix44 ForEulerZXY( const Vec3& euler_degrees, const Vec3& translation );
	static const Matrix44 MakeProjectionOrthographic( float left, float right, float bottom, float top, float near = NONZERO_ZERO, float far = 100.0f );
	static const Matrix44 MakeProjectionPerspective( float left, float right, float bottom, float top, float near, float far);
	static const Matrix44 MakeProjectionPerspective( float fov_degrees, float aspect, float near_z, float far_z );
	static const Matrix44 MakeTransformMat44(const std::string& transform );

	float& operator()(unsigned int row, unsigned int col);
	float& operator[](unsigned int index);
	Vec4 operator*(const Vec4& vector) const;
	Matrix44 operator*(Matrix44& matrix);
	Matrix44 operator+(const Matrix44& other_matrix);
	Matrix44 operator*(const float scale_value);

private:
	
};
