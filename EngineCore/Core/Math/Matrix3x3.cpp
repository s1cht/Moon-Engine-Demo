#include "Matrix3x3.h"
#include "Quaternion.h"

#define MAT2x2_DET(a, b, c, d) (a * d - b * c)

namespace Pawn::Core::Math
{
	Matrix3x3 operator*(const float32& scalar, const Matrix3x3& mat)
	{
		return mat * scalar;
	}

	Matrix3x3::Matrix3x3(
		const float32& a11, const float32& a12, const float32& a13,
		const float32& a21, const float32& a22, const float32& a23,
		const float32& a31, const float32& a32, const float32& a33) :
		a11(a11), a12(a12), a13(a13),
		a21(a21), a22(a22), a23(a23),
		a31(a31), a32(a32), a33(a33) {}

	Matrix3x3::Matrix3x3(float32 scalar) noexcept :
		a11(scalar), a12(0.0f), a13(0.0f),
		a21(0.0f), a22(scalar), a23(0.0f),
		a31(0.0f), a32(0.0f), a33(scalar) {}

	Matrix3x3::Matrix3x3() :
		a11(1.0f), a12(0.0f), a13(0.0f),
		a21(0.0f), a22(1.0f), a23(0.0f),
		a31(0.0f), a32(0.0f), a33(1.0f) {}

	/*Matrix3x3 Matrix3x3::FromQuaternion(const Quaternion& q)
	{
		float32 xx = q.x * q.x;
		float32 yy = q.y * q.y;
		float32 zz = q.z * q.z;
		float32 xy = q.x * q.y;
		float32 xz = q.x * q.z;
		float32 yz = q.y * q.z;
		float32 wx = q.w * q.x;
		float32 wy = q.w * q.y;
		float32 wz = q.w * q.z;

		return Matrix3x3(
			1.0f - 2.0f * (yy + zz), 2.0f * (xy - wz), 2.0f * (xz + wy),
			2.0f * (xy + wz), 1.0f - 2.0f * (xx + zz), 2.0f * (yz - wx),
			2.0f * (xz - wy), 2.0f * (yz + wx), 1.0f - 2.0f * (xx + yy)
		);
	}*/

	Matrix3x3 Matrix3x3::Transpose() const
	{
		return Matrix3x3(
			a11, a21, a31,
			a12, a22, a32,
			a13, a23, a33
		);
	}

	Matrix3x3 Matrix3x3::Abs() const
	{
		return Matrix3x3(
			abs(a11), abs(a12), abs(a13),
			abs(a21), abs(a22), abs(a23),
			abs(a31), abs(a32), abs(a33)
		);
	}

	Matrix3x3 Matrix3x3::Adjoint() const
	{
		return Matrix3x3(
			MAT2x2_DET(a22, a23, a32, a33),
			-MAT2x2_DET(a21, a23, a31, a33),
			MAT2x2_DET(a21, a22, a31, a32),
			-MAT2x2_DET(a12, a13, a32, a33),
			MAT2x2_DET(a11, a13, a31, a33),
			-MAT2x2_DET(a11, a12, a31, a32),
			MAT2x2_DET(a12, a13, a22, a23),
			-MAT2x2_DET(a11, a13, a21, a23),
			MAT2x2_DET(a11, a12, a21, a22)
		);
	}

	Matrix3x3 Matrix3x3::Invert() const
	{
		float32 det = Determinant();
		if (abs(det) < 1e-6f)
			return Matrix3x3(0.0f);

		float32 invDet = 1.0f / det;

		return Matrix3x3(
			invDet * (a22 * a33 - a23 * a32),
			invDet * (a13 * a32 - a12 * a33),
			invDet * (a12 * a23 - a13 * a22),
			invDet * (a23 * a31 - a21 * a33),
			invDet * (a11 * a33 - a13 * a31),
			invDet * (a13 * a21 - a11 * a23),
			invDet * (a21 * a32 - a22 * a31),
			invDet * (a12 * a31 - a11 * a32),
			invDet * (a11 * a22 - a12 * a21)
		);
	}

	float32 Matrix3x3::Determinant() const
	{
		return (a11 * a22 * a33 + a21 * a32 * a13 + a31 * a12 * a23)
			- (a13 * a22 * a31 + a23 * a32 * a11 + a33 * a12 * a21);
	}

	bool Matrix3x3::operator==(const Matrix3x3& other) const
	{
		float32 epsilon = 1e-6f;
		return abs(a11 - other.a11) < epsilon &&
			abs(a12 - other.a12) < epsilon &&
			abs(a13 - other.a13) < epsilon &&
			abs(a21 - other.a21) < epsilon &&
			abs(a22 - other.a22) < epsilon &&
			abs(a23 - other.a23) < epsilon &&
			abs(a31 - other.a31) < epsilon &&
			abs(a32 - other.a32) < epsilon &&
			abs(a33 - other.a33) < epsilon;
	}

	Matrix3x3& Matrix3x3::operator-=(const Matrix3x3& other)
	{
		a11 -= other.a11; a12 -= other.a12; a13 -= other.a13;
		a21 -= other.a21; a22 -= other.a22; a23 -= other.a23;
		a31 -= other.a31; a32 -= other.a32; a33 -= other.a33;
		return *this;
	}

	Matrix3x3 Matrix3x3::operator-(const Matrix3x3& other) const
	{
		return Matrix3x3(
			a11 - other.a11, a12 - other.a12, a13 - other.a13,
			a21 - other.a21, a22 - other.a22, a23 - other.a23,
			a31 - other.a31, a32 - other.a32, a33 - other.a33
		);
	}

	Matrix3x3& Matrix3x3::operator+=(const Matrix3x3& other)
	{
		a11 += other.a11; a12 += other.a12; a13 += other.a13;
		a21 += other.a21; a22 += other.a22; a23 += other.a23;
		a31 += other.a31; a32 += other.a32; a33 += other.a33;
		return *this;
	}

	Matrix3x3 Matrix3x3::operator+(const Matrix3x3& other) const
	{
		return Matrix3x3(
			a11 + other.a11, a12 + other.a12, a13 + other.a13,
			a21 + other.a21, a22 + other.a22, a23 + other.a23,
			a31 + other.a31, a32 + other.a32, a33 + other.a33
		);
	}

	Matrix3x3 Matrix3x3::operator*(const Matrix3x3& other) const
	{
		return Matrix3x3(
			a11 * other.a11 + a12 * other.a21 + a13 * other.a31,
			a11 * other.a12 + a12 * other.a22 + a13 * other.a32,
			a11 * other.a13 + a12 * other.a23 + a13 * other.a33,

			a21 * other.a11 + a22 * other.a21 + a23 * other.a31,
			a21 * other.a12 + a22 * other.a22 + a23 * other.a32,
			a21 * other.a13 + a22 * other.a23 + a23 * other.a33,

			a31 * other.a11 + a32 * other.a21 + a33 * other.a31,
			a31 * other.a12 + a32 * other.a22 + a33 * other.a32,
			a31 * other.a13 + a32 * other.a23 + a33 * other.a33
		);
	}

	Matrix3x3 Matrix3x3::operator*(const float32& scalar) const
	{
		return Matrix3x3(
			a11 * scalar, a12 * scalar, a13 * scalar,
			a21 * scalar, a22 * scalar, a23 * scalar,
			a31 * scalar, a32 * scalar, a33 * scalar
		);
	}

	Matrix3x3& Matrix3x3::operator*=(const int32& scalar)
	{
		return *this *= static_cast<float32>(scalar);
	}

	Matrix3x3& Matrix3x3::operator*=(const float32& scalar)
	{
		a11 *= scalar; a12 *= scalar; a13 *= scalar;
		a21 *= scalar; a22 *= scalar; a23 *= scalar;
		a31 *= scalar; a32 *= scalar; a33 *= scalar;
		return *this;
	}
}

#undef MAT2x2_DET