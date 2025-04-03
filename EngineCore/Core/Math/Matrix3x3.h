//
//					Matrix3x3.h
//		Part of Pawn Engines math library
//


#pragma once

#include "Core.h"
#include "Core/Math/Constants.h"
#include "Core/Math/Formulas.h"
#include <cmath>

#define MAT2x2_DET(a, b, c, d) (a * d - b * c)

namespace Pawn::Core::Math
{
	struct Matrix3x3
	{
	public:
		union
		{
			float32 Matrix[3][3];
			float32 a[3][3];
			struct
			{
				float32	a11, a12, a13, 
						a21, a22, a23, 
						a31, a32, a33;
			};
		};

	public:
		Matrix3x3();
		explicit Matrix3x3(float32 scalar) noexcept;
		explicit Matrix3x3(
			const float32& a11, const float32& a12, const float32& a13,
			const float32& a21, const float32& a22, const float32& a23,
			const float32& a31, const float32& a32, const float32& a33
		);

		~Matrix3x3() = default;

	public:
		Matrix3x3& operator*(const float32& scalar);
		Matrix3x3& operator*(const int32& scalar);
		Matrix3x3& operator+(const Matrix3x3& other);
		Matrix3x3& operator-(const Matrix3x3& other);

	public:
		float32 Determinant() const;
		Matrix3x3 Invert() const;
		Matrix3x3 Adjoint() const;
		Matrix3x3 Abs() const;
		Matrix3x3 Transpose() const;

	};


	Matrix3x3::Matrix3x3()
	{
		memset(Matrix, 0, sizeof(Matrix));
	}

	Matrix3x3::Matrix3x3(float32 scalar) noexcept
	{
		Matrix[0][0] = scalar; Matrix[0][1] = 0.f; Matrix[0][2] = 0.f;
		Matrix[1][0] = 0.f;    Matrix[1][1] = scalar; Matrix[1][2] = 0.f;
		Matrix[2][0] = 0.f;    Matrix[2][1] = 0.f;    Matrix[2][2] = scalar;
	}

	Matrix3x3::Matrix3x3(
		const float32& a11, const float32& a12, const float32& a13, 
		const float32& a21, const float32& a22, const float32& a23, 
		const float32& a31, const float32& a32, const float32& a33)
	{
		this->a11 = a11; this->a12 = a12; this->a13 = a13;
		this->a21 = a21; this->a22 = a22; this->a23 = a23;
		this->a31 = a31; this->a32 = a32; this->a33 = a33;
	}

	Matrix3x3& Matrix3x3::operator*(const float32& scalar)
	{
		Matrix[0][0] *= scalar;
		Matrix[0][1] *= scalar;
		Matrix[0][2] *= scalar;

		Matrix[1][0] *= scalar;
		Matrix[1][1] *= scalar;
		Matrix[1][2] *= scalar;

		Matrix[2][0] *= scalar;
		Matrix[2][1] *= scalar;
		Matrix[2][2] *= scalar;

		return *this;
	}

	Matrix3x3& Matrix3x3::operator*(const int32& scalar)
	{
		return this->operator*((float32)scalar);
	}

	Matrix3x3& Matrix3x3::operator+(const Matrix3x3& other)
	{
		Matrix[0][0] += other.Matrix[0][0];
		Matrix[0][1] += other.Matrix[0][1];
		Matrix[0][2] += other.Matrix[0][2];
		Matrix[1][0] += other.Matrix[1][0];
		Matrix[1][1] += other.Matrix[1][1];
		Matrix[1][2] += other.Matrix[1][2];
		Matrix[2][0] += other.Matrix[2][0];
		Matrix[2][1] += other.Matrix[2][1];
		Matrix[2][2] += other.Matrix[2][2];

		return *this;
	}

	Matrix3x3& Matrix3x3::operator-(const Matrix3x3& other)
	{
		Matrix[0][0] -= other.Matrix[0][0];
		Matrix[0][1] -= other.Matrix[0][1];
		Matrix[0][2] -= other.Matrix[0][2];
		Matrix[1][0] -= other.Matrix[1][0];
		Matrix[1][1] -= other.Matrix[1][1];
		Matrix[1][2] -= other.Matrix[1][2];
		Matrix[2][0] -= other.Matrix[2][0];
		Matrix[2][1] -= other.Matrix[2][1];
		Matrix[2][2] -= other.Matrix[2][2];

		return *this;
	}

	float32 Matrix3x3::Determinant() const
	{
		return (a[0][0] * a[1][1] * a[2][2] + a[1][0] * a[2][1] * a[0][2] + a[2][0] * a[0][1] * a[1][2]) 
			- (a[0][2] * a[1][1] * a[2][0] + a[1][2] * a[2][1] * a[0][0] + a[2][2] * a[0][1] * a[1][0]);
	}

	Matrix3x3 Matrix3x3::Invert() const
	{
		float32 det = Determinant();
		if (det == 0.f) return Matrix3x3(0.f);
		return Adjoint().Transpose() * (1.f / det);
	}

	Matrix3x3 Matrix3x3::Adjoint() const
	{
		Matrix3x3 adjoint = Matrix3x3(0.f);

		adjoint.a11 = MAT2x2_DET(a22, a23, a32, a33);
		adjoint.a12 = -MAT2x2_DET(a21, a23, a31, a33);
		adjoint.a13 = MAT2x2_DET(a21, a22, a31, a32);
		
		adjoint.a21 = -MAT2x2_DET(a12, a13, a32, a33);
		adjoint.a22 = MAT2x2_DET(a11, a13, a31, a33);
		adjoint.a23 = -MAT2x2_DET(a11, a12, a31, a32);

		adjoint.a31 = MAT2x2_DET(a12, a13, a22, a23);
		adjoint.a32 = -MAT2x2_DET(a11, a13, a21, a23);
		adjoint.a33 = MAT2x2_DET(a11, a12, a21, a22);

		return adjoint;
	}

	Matrix3x3 Matrix3x3::Abs() const
	{
		Matrix3x3 result = *this;

		result.a[0][0] = abs(a[0][0]);
		result.a[0][1] = abs(a[0][1]);
		result.a[0][2] = abs(a[0][2]);

		result.a[1][0] = abs(a[1][0]);
		result.a[1][1] = abs(a[1][1]);
		result.a[1][2] = abs(a[1][2]);

		result.a[2][0] = abs(a[2][0]);
		result.a[2][1] = abs(a[2][1]);
		result.a[2][2] = abs(a[2][2]);

		return result;
	}

	Matrix3x3 Matrix3x3::Transpose() const
	{
		Matrix3x3 result = *this;
		result.a[0][1] = a[1][0];
		result.a[1][0] = a[0][1];

		result.a[0][1] = a[1][0];
		result.a[1][0] = a[0][1];

		result.a[0][2] = a[2][0];
		result.a[2][0] = a[0][2];

		result.a[1][2] = a[2][1];
		result.a[2][1] = a[1][2];

		return result;
	}
}