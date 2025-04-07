#pragma once

#include "Core.h"
#include "MathMacros.h"
#include <cmath>

#define MAT3x3_DET(a11, a12, a13, a21, a22, a23, a31, a32, a33) \
    (a11 * a22 * a33 + a12 * a23 * a31 + a13 * a21 * a32 - a13 * a22 * a31 - a12 * a21 * a33 - a11 * a23 * a32)

namespace Pawn::Core::Math
{
	struct Matrix3x3;

	template<typename T>
	struct Vector3;

	struct Quaternion;

	struct CORE_API Matrix4x4
	{
	public:
		union
		{
			float32 Matrix[4][4];
			float32 a[4][4];
			struct
			{
				float32 a11, a12, a13, a14,
					a21, a22, a23, a24,
					a31, a32, a33, a34,
					a41, a42, a43, a44;
			};
		};

	public:
		inline Matrix4x4();
		inline Matrix4x4(float32 scalar) noexcept;
		inline Matrix4x4(
			const float32& a11, const float32& a12, const float32& a13, const float32& a14,
			const float32& a21, const float32& a22, const float32& a23, const float32& a24,
			const float32& a31, const float32& a32, const float32& a33, const float32& a34,
			const float32& a41, const float32& a42, const float32& a43, const float32& a44);

		~Matrix4x4() = default;

	public:
		Matrix4x4 operator*(const float32& scalar) const;
		Matrix4x4& operator*=(const float32& scalar);
		Matrix4x4& operator*=(const int32& scalar);
		Matrix4x4 operator*(const Matrix4x4& other) const;
		Matrix4x4 operator+(const Matrix4x4& other) const;
		Matrix4x4& operator+=(const Matrix4x4& other);
		Matrix4x4 operator-(const Matrix4x4& other) const;
		Matrix4x4& operator-=(const Matrix4x4& other);
		bool operator==(const Matrix4x4& other) const;

	public:
		float32 Determinant() const;

		Matrix4x4 Invert() const;

		Matrix4x4 Adjoint() const;

		Matrix4x4 Abs() const;

		Matrix4x4 Transpose() const;

	public:
		static Matrix4x4 FromQuaternion(const Quaternion& q);

		static Matrix4x4 Translation(const Vector3<float32>& translation);

		static Matrix4x4 Scale(const Vector3<float32>& scale);

		static Matrix4x4 Perspective(float32 fov, float32 aspect, float32 _near, float32 _far);

		static Matrix4x4 Orthographic(float32 left, float32 right, float32 bottom, float32 top, float32 _near, float32 _far);

		static Matrix4x4 LookAt(const Vector3<float32>& eye, const Vector3<float32>& target, const Vector3<float32>& up);
	};

	Matrix4x4 operator*(const float32& scalar, const Matrix4x4& mat);
}
