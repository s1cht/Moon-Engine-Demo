//
//					Matrix4x4.h
//		Part of Pawn Engines math library
//

#pragma once

#include "Core.h"
#include "Core/Math/Constants.h"
#include "Core/Math/Formulas.h"
#include <cmath>

#define MAT3x3_DET(a11, a12, a13, a21, a22, a23, a31, a32, a33) \
    (a11 * a22 * a33 + a12 * a23 * a31 + a13 * a21 * a32 - a13 * a22 * a31 - a12 * a21 * a33 - a11 * a23 * a32)

namespace Pawn::Core::Math
{
	struct Matrix3x3;

	struct Matrix4x4
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
		Matrix4x4();
		explicit Matrix4x4(float32 scalar) noexcept;
		explicit Matrix4x4(
			const float32& a11, const float32& a12, const float32& a13, const float32& a14,
			const float32& a21, const float32& a22, const float32& a23, const float32& a24,
			const float32& a31, const float32& a32, const float32& a33, const float32& a34,
			const float32& a41, const float32& a42, const float32& a43, const float32& a44
		);

		~Matrix4x4() = default;

	public:
		Matrix4x4& operator*(const float32& scalar);
		Matrix4x4& operator*(const int32& scalar);
		Matrix4x4& operator+(const Matrix4x4& other);
		Matrix4x4& operator-(const Matrix4x4& other);

	public:
		float32 Determinant() const;
		Matrix4x4 Invert() const;
		Matrix4x4 Adjoint() const;
		Matrix4x4 Abs() const;
		Matrix4x4 Transpose() const;

	};

	Matrix4x4::Matrix4x4()
	{
		memset(Matrix, 0, sizeof(Matrix));
	}

	Matrix4x4::Matrix4x4(float32 scalar) noexcept
	{
		Matrix[0][0] = scalar; Matrix[0][1] = 0.f; Matrix[0][2] = 0.f; Matrix[0][3] = 0.f;
		Matrix[1][0] = 0.f;    Matrix[1][1] = scalar; Matrix[1][2] = 0.f; Matrix[1][3] = 0.f;
		Matrix[2][0] = 0.f;    Matrix[2][1] = 0.f;    Matrix[2][2] = scalar; Matrix[2][3] = 0.f;
		Matrix[3][0] = 0.f;    Matrix[3][1] = 0.f;    Matrix[3][2] = 0.f;    Matrix[3][3] = scalar;
	}

	Matrix4x4::Matrix4x4(
		const float32& a11, const float32& a12, const float32& a13, const float32& a14,
		const float32& a21, const float32& a22, const float32& a23, const float32& a24,
		const float32& a31, const float32& a32, const float32& a33, const float32& a34,
		const float32& a41, const float32& a42, const float32& a43, const float32& a44)
	{
		this->a11 = a11; this->a12 = a12; this->a13 = a13; this->a14 = a14;
		this->a21 = a21; this->a22 = a22; this->a23 = a23; this->a24 = a24;
		this->a31 = a31; this->a32 = a32; this->a33 = a33; this->a34 = a34;
		this->a41 = a41; this->a42 = a42; this->a43 = a43; this->a44 = a44;
	}

	Matrix4x4& Matrix4x4::operator*(const float32& scalar)
	{
		Matrix[0][0] *= scalar; Matrix[0][1] *= scalar; Matrix[0][2] *= scalar; Matrix[0][3] *= scalar;
		Matrix[1][0] *= scalar; Matrix[1][1] *= scalar; Matrix[1][2] *= scalar; Matrix[1][3] *= scalar;
		Matrix[2][0] *= scalar; Matrix[2][1] *= scalar; Matrix[2][2] *= scalar; Matrix[2][3] *= scalar;
		Matrix[3][0] *= scalar; Matrix[3][1] *= scalar; Matrix[3][2] *= scalar; Matrix[3][3] *= scalar;
		return *this;
	}

	Matrix4x4& Matrix4x4::operator*(const int32& scalar)
	{
		return this->operator*((float32)scalar);
	}

	Matrix4x4& Matrix4x4::operator+(const Matrix4x4& other)
	{
		Matrix[0][0] += other.Matrix[0][0]; Matrix[0][1] += other.Matrix[0][1]; Matrix[0][2] += other.Matrix[0][2]; Matrix[0][3] += other.Matrix[0][3];
		Matrix[1][0] += other.Matrix[1][0]; Matrix[1][1] += other.Matrix[1][1]; Matrix[1][2] += other.Matrix[1][2]; Matrix[1][3] += other.Matrix[1][3];
		Matrix[2][0] += other.Matrix[2][0]; Matrix[2][1] += other.Matrix[2][1]; Matrix[2][2] += other.Matrix[2][2]; Matrix[2][3] += other.Matrix[2][3];
		Matrix[3][0] += other.Matrix[3][0]; Matrix[3][1] += other.Matrix[3][1]; Matrix[3][2] += other.Matrix[3][2]; Matrix[3][3] += other.Matrix[3][3];
		return *this;
	}

	Matrix4x4& Matrix4x4::operator-(const Matrix4x4& other)
	{
		Matrix[0][0] -= other.Matrix[0][0]; Matrix[0][1] -= other.Matrix[0][1]; Matrix[0][2] -= other.Matrix[0][2]; Matrix[0][3] -= other.Matrix[0][3];
		Matrix[1][0] -= other.Matrix[1][0]; Matrix[1][1] -= other.Matrix[1][1]; Matrix[1][2] -= other.Matrix[1][2]; Matrix[1][3] -= other.Matrix[1][3];
		Matrix[2][0] -= other.Matrix[2][0]; Matrix[2][1] -= other.Matrix[2][1]; Matrix[2][2] -= other.Matrix[2][2]; Matrix[2][3] -= other.Matrix[2][3];
		Matrix[3][0] -= other.Matrix[3][0]; Matrix[3][1] -= other.Matrix[3][1]; Matrix[3][2] -= other.Matrix[3][2]; Matrix[3][3] -= other.Matrix[3][3];
		return *this;
	}

	float32 Matrix4x4::Determinant() const
	{
		return a11 * MAT3x3_DET(a22, a23, a24, a32, a33, a34, a42, a43, a44)
			- a12 * MAT3x3_DET(a21, a23, a24, a31, a33, a34, a41, a43, a44)
			+ a13 * MAT3x3_DET(a21, a22, a24, a31, a32, a34, a41, a42, a44)
			- a14 * MAT3x3_DET(a21, a22, a23, a31, a32, a33, a41, a42, a43);
	}

	Matrix4x4 Matrix4x4::Invert() const
	{
		float32 det = Determinant();
		if (det == 0.f) return Matrix4x4(0.f);
		return Adjoint() * (1.f / det);
	}

	Matrix4x4 Matrix4x4::Adjoint() const
	{
		Matrix4x4 adjoint = Matrix4x4(0.f);

		adjoint.a11 = MAT3x3_DET(a22, a23, a24, a32, a33, a34, a42, a43, a44);
		adjoint.a12 = -MAT3x3_DET(a21, a23, a24, a31, a33, a34, a41, a43, a44);
		adjoint.a13 = MAT3x3_DET(a21, a22, a24, a31, a32, a34, a41, a42, a44);
		adjoint.a14 = -MAT3x3_DET(a21, a22, a23, a31, a32, a33, a41, a42, a43);

		adjoint.a21 = -MAT3x3_DET(a12, a13, a14, a32, a33, a34, a42, a43, a44);
		adjoint.a22 = MAT3x3_DET(a11, a13, a14, a31, a33, a34, a41, a43, a44);
		adjoint.a23 = -MAT3x3_DET(a11, a12, a14, a31, a32, a34, a41, a42, a44);
		adjoint.a24 = MAT3x3_DET(a11, a12, a13, a31, a32, a33, a41, a42, a43);

		adjoint.a31 = MAT3x3_DET(a12, a13, a14, a22, a23, a24, a42, a43, a44);
		adjoint.a32 = -MAT3x3_DET(a11, a13, a14, a21, a23, a24, a41, a43, a44);
		adjoint.a33 = MAT3x3_DET(a11, a12, a14, a21, a22, a24, a41, a42, a44);
		adjoint.a34 = -MAT3x3_DET(a11, a12, a13, a21, a22, a23, a41, a42, a43);

		adjoint.a41 = -MAT3x3_DET(a12, a13, a14, a22, a23, a24, a32, a33, a34);
		adjoint.a42 = MAT3x3_DET(a11, a13, a14, a21, a23, a24, a31, a33, a34);
		adjoint.a43 = -MAT3x3_DET(a11, a12, a14, a21, a22, a24, a31, a32, a34);
		adjoint.a44 = MAT3x3_DET(a11, a12, a13, a21, a22, a23, a31, a32, a33);

		return adjoint.Transpose();
	}

	Matrix4x4 Matrix4x4::Abs() const
	{
		Matrix4x4 result = *this;
		result.a[0][0] = abs(a[0][0]); result.a[0][1] = abs(a[0][1]); result.a[0][2] = abs(a[0][2]); result.a[0][3] = abs(a[0][3]);
		result.a[1][0] = abs(a[1][0]); result.a[1][1] = abs(a[1][1]); result.a[1][2] = abs(a[1][2]); result.a[1][3] = abs(a[1][3]);
		result.a[2][0] = abs(a[2][0]); result.a[2][1] = abs(a[2][1]); result.a[2][2] = abs(a[2][2]); result.a[2][3] = abs(a[2][3]);
		result.a[3][0] = abs(a[3][0]); result.a[3][1] = abs(a[3][1]); result.a[3][2] = abs(a[3][2]); result.a[3][3] = abs(a[3][3]);
		return result;
	}

	Matrix4x4 Matrix4x4::Transpose() const
	{
		Matrix4x4 result = *this;
		result.a[0][1] = a[1][0]; result.a[1][0] = a[0][1];
		result.a[0][2] = a[2][0]; result.a[2][0] = a[0][2];
		result.a[0][3] = a[3][0]; result.a[3][0] = a[0][3];
		result.a[1][2] = a[2][1]; result.a[2][1] = a[1][2];
		result.a[1][3] = a[3][1]; result.a[3][1] = a[1][3];
		result.a[2][3] = a[3][2]; result.a[3][2] = a[2][3];
		return result;
	}
}