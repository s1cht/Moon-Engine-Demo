#pragma once

#include "Core.h"
#include "MathMacros.h"
#include <cmath>

#define MAT2x2_DET(a, b, c, d) (a * d - b * c)

namespace Pawn::Core::Math
{
	struct Quaternion;

	struct CORE_API Matrix3x3
	{
	public:
		union
		{
			float32 Matrix[3][3];
			float32 a[3][3];
			struct
			{
				float32 a11, a12, a13,
					a21, a22, a23,
					a31, a32, a33;
			};
		};

	public:
		inline Matrix3x3();

		inline Matrix3x3(float32 scalar) noexcept;

		inline Matrix3x3(
			const float32& a11, const float32& a12, const float32& a13,
			const float32& a21, const float32& a22, const float32& a23,
			const float32& a31, const float32& a32, const float32& a33);

		~Matrix3x3() = default;

	public:
		Matrix3x3 operator*(const float32& scalar) const;

		Matrix3x3& operator*=(const float32& scalar);

		Matrix3x3& operator*=(const int32& scalar);

		Matrix3x3 operator*(const Matrix3x3& other) const;

		Matrix3x3 operator+(const Matrix3x3& other) const;

		Matrix3x3& operator+=(const Matrix3x3& other);

		Matrix3x3 operator-(const Matrix3x3& other) const;

		Matrix3x3& operator-=(const Matrix3x3& other);

		bool operator==(const Matrix3x3& other) const;

	public:
		float32 Determinant() const;

		Matrix3x3 Invert() const;

		Matrix3x3 Adjoint() const;

		Matrix3x3 Abs() const;

		Matrix3x3 Transpose() const;

		//static Matrix3x3 FromQuaternion(const Quaternion& q);
	};

	inline Matrix3x3 operator*(const float32& scalar, const Matrix3x3& mat);

}