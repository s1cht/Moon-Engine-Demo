#include "Matrix4x4.hpp"

#include "Math.hpp"
#include "Vector4.hpp"
#include "Core/Math/Quaternion.hpp"

namespace ME::Core::Math
{
	inline Matrix4x4::Matrix4x4()
		: m11(1.0f), m12(0.0f), m13(0.0f), m14(0.0f),
		m21(0.0f), m22(1.0f), m23(0.0f), m24(0.0f),
		m31(0.0f), m32(0.0f), m33(1.0f), m34(0.0f),
		m41(0.0f), m42(0.0f), m43(0.0f), m44(1.0f)
	{
	}

	inline Matrix4x4::Matrix4x4(float32 scalar) noexcept
		: m11(scalar), m12(0.0f), m13(0.0f), m14(0.0f),
		m21(0.0f), m22(scalar), m23(0.0f), m24(0.0f),
		m31(0.0f), m32(0.0f), m33(scalar), m34(0.0f),
		m41(0.0f), m42(0.0f), m43(0.0f), m44(scalar)
	{
	}

	inline Matrix4x4::Matrix4x4(
		float32 a11, float32 a12, float32 m13, float32 a14,
		float32 a21, float32 a22, float32 a23, float32 a24,
		float32 a31, float32 m32, float32 a33, float32 a34,
		float32 a41, float32 a42, float32 a43, float32 a44)
		: m11(a11), m12(a12), m13(m13), m14(a14),
		m21(a21), m22(a22), m23(a23), m24(a24),
		m31(a31), m32(m32), m33(a33), m34(a34),
		m41(a41), m42(a42), m43(a43), m44(a44)
	{
	}

	// Matrix-scalar multiplication M * C
	Matrix4x4 Matrix4x4::operator*(float32 scalar) const
	{
		return Matrix4x4(
			m11 * scalar, m12 * scalar, m13 * scalar, m14 * scalar,
			m21 * scalar, m22 * scalar, m23 * scalar, m24 * scalar,
			m31 * scalar, m32 * scalar, m33 * scalar, m34 * scalar,
			m41 * scalar, m42 * scalar, m43 * scalar, m44 * scalar
		);
	}

	// Column-Major matrix-vector multiplication M * v
	Vector4<float32> Matrix4x4::operator*(const Vector4<float32>& other) const
	{
		return Vector4(
			other.x * m11 + other.y * m21 + other.z * m31 + other.w * m41,
			other.x * m12 + other.y * m22 + other.z * m32 + other.w * m42,
			other.x * m13 + other.y * m23 + other.z * m33 + other.w * m43,
			other.x * m14 + other.y * m24 + other.z * m34 + other.w * m44
		);
	}

	// Matrix-scalar multiplication M * C
    Matrix4x4& Matrix4x4::operator*=(float32 scalar)
	{
		m11 *= scalar; m12 *= scalar; m13 *= scalar; m14 *= scalar;
		m21 *= scalar; m22 *= scalar; m23 *= scalar; m24 *= scalar;
		m31 *= scalar; m32 *= scalar; m33 *= scalar; m34 *= scalar;
		m41 *= scalar; m42 *= scalar; m43 *= scalar; m44 *= scalar;
		return *this;
	}

	// Matrix-matrix multiplication A * B
    Matrix4x4 Matrix4x4::operator*=(const Matrix4x4& other)
    {
		return *this = *this * other;
    }

	// Matrix-scalar multiplication M * C
    Matrix4x4& Matrix4x4::operator*=(const int32& scalar)
	{
		return *this *= static_cast<float32>(scalar);
	}


	// Matrix-matrix multiplication A * B
	Matrix4x4 Matrix4x4::operator*(const Matrix4x4& other) const
	{
		return Matrix4x4(
			m11 * other.m11 + m12 * other.m21 + m13 * other.m31 + m14 * other.m41,
			m11 * other.m12 + m12 * other.m22 + m13 * other.m32 + m14 * other.m42,
			m11 * other.m13 + m12 * other.m23 + m13 * other.m33 + m14 * other.m43,
			m11 * other.m14 + m12 * other.m24 + m13 * other.m34 + m14 * other.m44,

			m21 * other.m11 + m22 * other.m21 + m23 * other.m31 + m24 * other.m41,
			m21 * other.m12 + m22 * other.m22 + m23 * other.m32 + m24 * other.m42,
			m21 * other.m13 + m22 * other.m23 + m23 * other.m33 + m24 * other.m43,
			m21 * other.m14 + m22 * other.m24 + m23 * other.m34 + m24 * other.m44,

			m31 * other.m11 + m32 * other.m21 + m33 * other.m31 + m34 * other.m41,
			m31 * other.m12 + m32 * other.m22 + m33 * other.m32 + m34 * other.m42,
			m31 * other.m13 + m32 * other.m23 + m33 * other.m33 + m34 * other.m43,
			m31 * other.m14 + m32 * other.m24 + m33 * other.m34 + m34 * other.m44,

			m41 * other.m11 + m42 * other.m21 + m43 * other.m31 + m44 * other.m41,
			m41 * other.m12 + m42 * other.m22 + m43 * other.m32 + m44 * other.m42,
			m41 * other.m13 + m42 * other.m23 + m43 * other.m33 + m44 * other.m43,
			m41 * other.m14 + m42 * other.m24 + m43 * other.m34 + m44 * other.m44
		);
	}

	// Matrix-matrix addition A + B
	Matrix4x4 Matrix4x4::operator+(const Matrix4x4& other) const
	{
		return Matrix4x4(
			m11 + other.m11, m12 + other.m12, m13 + other.m13, m14 + other.m14,
			m21 + other.m21, m22 + other.m22, m23 + other.m23, m24 + other.m24,
			m31 + other.m31, m32 + other.m32, m33 + other.m33, m34 + other.m34,
			m41 + other.m41, m42 + other.m42, m43 + other.m43, m44 + other.m44
		);
	}

	// Matrix-matrix addition A + B
	Matrix4x4& Matrix4x4::operator+=(const Matrix4x4& other)
	{
		m11 += other.m11; m12 += other.m12; m13 += other.m13; m14 += other.m14;
		m21 += other.m21; m22 += other.m22; m23 += other.m23; m24 += other.m24;
		m31 += other.m31; m32 += other.m32; m33 += other.m33; m34 += other.m34;
		m41 += other.m41; m42 += other.m42; m43 += other.m43; m44 += other.m44;
		return *this;
	}

	// Matrix-matrix subraction A - B
	Matrix4x4 Matrix4x4::operator-(const Matrix4x4& other) const
	{
		return Matrix4x4(
			m11 - other.m11, m12 - other.m12, m13 - other.m13, m14 - other.m14,
			m21 - other.m21, m22 - other.m22, m23 - other.m23, m24 - other.m24,
			m31 - other.m31, m32 - other.m32, m33 - other.m33, m34 - other.m34,
			m41 - other.m41, m42 - other.m42, m43 - other.m43, m44 - other.m44
		);
	}

	// Matrix-matrix subraction A - B
	Matrix4x4& Matrix4x4::operator-=(const Matrix4x4& other)
	{
		m11 -= other.m11; m12 -= other.m12; m13 -= other.m13; m14 -= other.m14;
		m21 -= other.m21; m22 -= other.m22; m23 -= other.m23; m24 -= other.m24;
		m31 -= other.m31; m32 -= other.m32; m33 -= other.m33; m34 -= other.m34;
		m41 -= other.m41; m42 -= other.m42; m43 -= other.m43; m44 -= other.m44;
		return *this;
	}

	// Comparing operator
	bool Matrix4x4::operator==(const Matrix4x4& other) const
	{
		return std::abs(m11 - other.m11) < ME::Core::Math::Epsilon &&
			std::abs(m12 - other.m12) < ME::Core::Math::Epsilon &&
			std::abs(m13 - other.m13) < ME::Core::Math::Epsilon &&
			std::abs(m14 - other.m14) < ME::Core::Math::Epsilon &&
			std::abs(m21 - other.m21) < ME::Core::Math::Epsilon &&
			std::abs(m22 - other.m22) < ME::Core::Math::Epsilon &&
			std::abs(m23 - other.m23) < ME::Core::Math::Epsilon &&
			std::abs(m24 - other.m24) < ME::Core::Math::Epsilon &&
			std::abs(m31 - other.m31) < ME::Core::Math::Epsilon &&
			std::abs(m32 - other.m32) < ME::Core::Math::Epsilon &&
			std::abs(m33 - other.m33) < ME::Core::Math::Epsilon &&
			std::abs(m34 - other.m34) < ME::Core::Math::Epsilon &&
			std::abs(m41 - other.m41) < ME::Core::Math::Epsilon &&
			std::abs(m42 - other.m42) < ME::Core::Math::Epsilon &&
			std::abs(m43 - other.m43) < ME::Core::Math::Epsilon &&
			std::abs(m44 - other.m44) < ME::Core::Math::Epsilon;
	}

	// Matrix determinant operation det(M)
	float32 Matrix4x4::Determinant() const
	{
		return m11 * MAT3x3_DET(m22, m23, m24, m32, m33, m34, m42, m43, m44)
			- m12 * MAT3x3_DET(m21, m23, m24, m31, m33, m34, m41, m43, m44)
			+ m13 * MAT3x3_DET(m21, m22, m24, m31, m32, m34, m41, m42, m44)
			- m14 * MAT3x3_DET(m21, m22, m23, m31, m32, m33, m41, m42, m43);
	}

	// Matrix invert operation M^-1
    Matrix4x4 Matrix4x4::Invert() const
    {
		float32 det = Determinant();
		if (std::abs(det) < 1e-6f)
			return Matrix4x4(0.0f);

		float32 invDet = 1.0f / det;

		Matrix4x4 result;
		result.m11 = invDet * (m22 * (m33 * m44 - m34 * m43) - m23 * (m32 * m44 - m34 * m42) + m24 * (m32 * m43 - m33 * m42));
		result.m12 = invDet * -(m21 * (m33 * m44 - m34 * m43) - m23 * (m31 * m44 - m34 * m41) + m24 * (m31 * m43 - m33 * m41));
		result.m13 = invDet * (m21 * (m32 * m44 - m34 * m42) - m22 * (m31 * m44 - m34 * m41) + m24 * (m31 * m42 - m32 * m41));
		result.m14 = invDet * -(m21 * (m32 * m43 - m33 * m42) - m22 * (m31 * m43 - m33 * m41) + m23 * (m31 * m42 - m32 * m41));

		result.m21 = invDet * -(m12 * (m33 * m44 - m34 * m43) - m13 * (m32 * m44 - m34 * m42) + m14 * (m32 * m43 - m33 * m42));
		result.m22 = invDet * (m11 * (m33 * m44 - m34 * m43) - m13 * (m31 * m44 - m34 * m41) + m14 * (m31 * m43 - m33 * m41));
		result.m23 = invDet * -(m11 * (m32 * m44 - m34 * m42) - m12 * (m31 * m44 - m34 * m41) + m14 * (m31 * m42 - m32 * m41));
		result.m24 = invDet * (m11 * (m32 * m43 - m33 * m42) - m12 * (m31 * m43 - m33 * m41) + m13 * (m31 * m42 - m32 * m41));

		result.m31 = invDet * (m12 * (m23 * m44 - m24 * m43) - m13 * (m22 * m44 - m24 * m42) + m14 * (m22 * m43 - m23 * m42));
		result.m32 = invDet * -(m11 * (m23 * m44 - m24 * m43) - m13 * (m21 * m44 - m24 * m41) + m14 * (m21 * m43 - m23 * m41));
		result.m33 = invDet * (m11 * (m22 * m44 - m24 * m42) - m12 * (m21 * m44 - m24 * m41) + m14 * (m21 * m42 - m22 * m41));
		result.m34 = invDet * -(m11 * (m22 * m43 - m23 * m42) - m12 * (m21 * m43 - m23 * m41) + m13 * (m21 * m42 - m22 * m41));

		result.m41 = invDet * -(m12 * (m23 * m34 - m24 * m33) - m13 * (m22 * m34 - m24 * m32) + m14 * (m22 * m33 - m23 * m32));
		result.m42 = invDet * (m11 * (m23 * m34 - m24 * m33) - m13 * (m21 * m34 - m24 * m31) + m14 * (m21 * m33 - m23 * m31));
		result.m43 = invDet * -(m11 * (m22 * m34 - m24 * m32) - m12 * (m21 * m34 - m24 * m31) + m14 * (m21 * m32 - m22 * m31));
		result.m44 = invDet * (m11 * (m22 * m33 - m23 * m32) - m12 * (m21 * m33 - m23 * m31) + m13 * (m21 * m32 - m22 * m31));
		return result;
    }

	// Affine matrix invert operation M^-1
    Matrix4x4 Matrix4x4::InvertAffine() const
	{
		Matrix4x4 result;
		result.m11 = m11; result.m12 = m21; result.m13 = m31; result.m14 = 0.0f;
		result.m21 = m12; result.m22 = m22; result.m23 = m32; result.m24 = 0.0f;
		result.m31 = m13; result.m32 = m23; result.m33 = m33; result.m34 = 0.0f;

		float tx = m41, ty = m42, tz = m43;
		result.m41 = -(tx * result.m11 + ty * result.m21 + tz * result.m31);
		result.m42 = -(tx * result.m12 + ty * result.m22 + tz * result.m32);
		result.m43 = -(tx * result.m13 + ty * result.m23 + tz * result.m33);
		result.m44 = 1.0f;

		return result;
	}

	// Matrix adjoint operation adj(M)
	Matrix4x4 Matrix4x4::Adjoint() const
	{
		Matrix4x4 result;

		result.m11 = MAT3x3_DET(m22, m23, m24, m32, m33, m34, m42, m43, m44);
		result.m12 = -MAT3x3_DET(m21, m23, m24, m31, m33, m34, m41, m43, m44);
		result.m13 = MAT3x3_DET(m21, m22, m24, m31, m32, m34, m41, m42, m44);
		result.m14 = -MAT3x3_DET(m21, m22, m23, m31, m32, m33, m41, m42, m43);

		result.m21 = -MAT3x3_DET(m12, m13, m14, m32, m33, m34, m42, m43, m44);
		result.m22 = MAT3x3_DET(m11, m13, m14, m31, m33, m34, m41, m43, m44);
		result.m23 = -MAT3x3_DET(m11, m12, m14, m31, m32, m34, m41, m42, m44);
		result.m24 = MAT3x3_DET(m11, m12, m13, m31, m32, m33, m41, m42, m43);

		result.m31 = MAT3x3_DET(m12, m13, m14, m22, m23, m24, m42, m43, m44);
		result.m32 = -MAT3x3_DET(m11, m13, m14, m21, m23, m24, m41, m43, m44);
		result.m33 = MAT3x3_DET(m11, m12, m14, m21, m22, m24, m41, m42, m44);
		result.m34 = -MAT3x3_DET(m11, m12, m13, m21, m22, m23, m41, m42, m43);

		result.m41 = -MAT3x3_DET(m12, m13, m14, m22, m23, m24, m32, m33, m34);
		result.m42 = MAT3x3_DET(m11, m13, m14, m21, m23, m24, m31, m33, m34);
		result.m43 = -MAT3x3_DET(m11, m12, m14, m21, m22, m24, m31, m32, m34);
		result.m44 = MAT3x3_DET(m11, m12, m13, m21, m22, m23, m31, m32, m33);

		return result;
	}

	// Matrix abs operation |M|
	Matrix4x4 Matrix4x4::Abs() const
	{
		return Matrix4x4(
			std::abs(m11), std::abs(m12), std::abs(m13), std::abs(m14),
			std::abs(m21), std::abs(m22), std::abs(m23), std::abs(m24),
			std::abs(m31), std::abs(m32), std::abs(m33), std::abs(m34),
			std::abs(m41), std::abs(m42), std::abs(m43), std::abs(m44)
		);
	}

	// Matrix transpose operation M^T
	Matrix4x4 Matrix4x4::Transpose() const
	{
		Matrix4x4 mat = {};
		mat.m[0][0] = this->m[0][0];
		mat.m[1][1] = this->m[1][1];
		mat.m[2][2] = this->m[2][2];
		mat.m[3][3] = this->m[3][3];

		mat.m[0][1] = this->m[1][0];
		mat.m[0][2] = this->m[2][0];
		mat.m[0][3] = this->m[3][0];

		mat.m[1][0] = this->m[0][1];
		mat.m[1][2] = this->m[2][1];
		mat.m[1][3] = this->m[3][1];

		mat.m[2][0] = this->m[0][2];
		mat.m[2][1] = this->m[1][2];
		mat.m[2][3] = this->m[3][2];

		mat.m[3][0] = this->m[0][3];
		mat.m[3][1] = this->m[1][3];
		mat.m[3][2] = this->m[2][3];

		return mat;
	}

	// Vector transformation using Matrix
	Vector3<float32> Matrix4x4::Transform(const Vector3<float32>& axis) const
	{
		float32 w = m41 * axis.x + m42 * axis.y + m43 * axis.z + m44;
		return Vector3(
			(m11 * axis.x + m12 * axis.y + m13 * axis.z + m14) / w,
			(m21 * axis.x + m22 * axis.y + m23 * axis.z + m24) / w,
			(m31 * axis.x + m32 * axis.y + m33 * axis.z + m34) / w
		);
	}

	// Unit vector transformation using Matrix
	Vector3<float32> Matrix4x4::TransformNormal(const Vector3<float32>& axis) const
	{
		return Vector3(
			(axis.x * m11 + axis.y * m12 + axis.z * m13),
			(axis.x * m21 + axis.y * m22 + axis.z * m23),
			(axis.x * m31 + axis.y * m32 + axis.z * m33)
		);
	}

	// Look vector calculation
    Vector3<float32> Matrix4x4::LookVector() const
    {
		return Vector3D(m31, m32, m33);
    }

	// Up vector calculation
    Vector3<float32> Matrix4x4::UpVector() const
    {
		return Vector3D(m21, m22, m23);
    }

	// Right vector calculation
    Vector3<float32> Matrix4x4::RightVector() const
    {
		return Vector3D(m11, m12, m13);
    }

	// Translation extraction
    Vector3<float32> Matrix4x4::GetTranslation() const
    {
        return Vector3(m41, m42, m43);
    }

	// Scale extraction
    Vector3<float32> Matrix4x4::GetScale() const
    {
		Vector3 xAxis(m11, m21, m31);
		Vector3 yAxis(m12, m22, m32);
		Vector3 zAxis(m13, m23, m33);
        return Vector3(xAxis.Length(), yAxis.Length(), zAxis.Length());
    }

	// XYZ Angle extraction
    Vector3<float32> Matrix4x4::GetEulerAnglesXYZ() const
    {
		float32 pitch, yaw, roll;

		if (fabs(m31) < 0.999999f)
		{
			yaw = std::asin(-m31);
			pitch = std::atan2(m32, m33);
			roll = std::atan2(m21, m11);
		}
		else
		{
			yaw = (m31 < 0 ? + static_cast<float32>(ME::Core::Math::Pi) / 2.0f : -static_cast<float32>(ME::Core::Math::Pi) / 2.0f);
			pitch = std::atan2(-m12, m22);
			roll = 0.0f;
		}

		return Vector3<float32>(
			ME::Core::Math::ToDegrees(pitch),
			ME::Core::Math::ToDegrees(yaw),
			ME::Core::Math::ToDegrees(roll)
		);
    }

	// YXZ Angle extraction
    Vector3<float32> Matrix4x4::GetEulerAnglesYXZ() const
	{
		float32 pitch, yaw, roll;

		if (fabs(m32) < 0.999999f)
		{
			yaw = std::atan2(m13, m33);
			pitch = std::asin(-m23);
			roll = std::atan2(m21, m22);
		}
		else
		{
			yaw = std::atan2(-m31, m11);
			pitch = (m32 < 0 ? +PI / 2.0f : -PI / 2.0f);
			roll = 0.0f;
		}

		return Vector3<float32>(
			ME::Core::Math::ToDegrees(pitch),
			ME::Core::Math::ToDegrees(yaw),
			ME::Core::Math::ToDegrees(roll)
		);
	}

	// ZYX Angle extraction
    Vector3<float32> Matrix4x4::GetEulerAnglesZYX() const
	{
		float32 pitch, yaw, roll;

		if (fabs(m31) < 0.999999f)
		{
			yaw = std::asin(-m31);
			pitch = std::atan2(m32, m33);
			roll = std::atan2(m21, m11);
		}
		else
		{
			yaw = (m31 < 0 ? +PI / 2.0f : -PI / 2.0f);
			pitch = std::atan2(-m12, m22);
			roll = 0.0f;
		}

		return Vector3<float32>(
			ME::Core::Math::ToDegrees(roll),
			ME::Core::Math::ToDegrees(pitch),
			ME::Core::Math::ToDegrees(yaw)
		);
	}

	// Quaternion extraction
	Quaternion Matrix4x4::GetQuaternion() const
	{
		float32 trace = m11 + m22 + m33;
		Quaternion q;

		if (trace > 0.0f)
		{
			float32 s = 0.5f / sqrtf(trace + 1.0f);
			q.w = 0.25f / s;
			q.x = (m23 - m32) * s;
			q.y = (m31 - m13) * s;
			q.z = (m12 - m21) * s;
		}
		else
		{
			if (m11 > m22 && m11 > m33)
			{
				float32 s = 2.0f * sqrtf(1.0f + m11 - m22 - m33);
				q.w = (m23 - m32) / s;
				q.x = 0.25f * s;
				q.y = (m12 + m21) / s;
				q.z = (m13 + m31) / s;
			}
			else if (m22 > m33)
			{
				float32 s = 2.0f * sqrtf(1.0f + m22 - m11 - m33);
				q.w = (m31 - m13) / s;
				q.x = (m12 + m21) / s;
				q.y = 0.25f * s;
				q.z = (m23 + m32) / s;
			}
			else
			{
				float32 s = 2.0f * sqrtf(1.0f + m33 - m11 - m22);
				q.w = (m12 - m21) / s;
				q.x = (m13 + m31) / s;
				q.y = (m23 + m32) / s;
				q.z = 0.25f * s;
			}
		}
		return q.Normalize();
	}

	// Matrix creation from quaternion
    Matrix4x4 Matrix4x4::FromQuaternion(const Quaternion& q)
	{
		float xx = q.x * q.x;
		float yy = q.y * q.y;
		float zz = q.z * q.z;
		float xy = q.x * q.y;
		float xz = q.x * q.z;
		float yz = q.y * q.z;
		float wx = q.w * q.x;
		float wy = q.w * q.y;
		float wz = q.w * q.z;

		return Matrix4x4(
			1.0f - 2.0f * (yy + zz),	2.0f * (xy - wz),			2.0f * (xz + wy),			0.0f,
			2.0f * (xy + wz),			1.0f - 2.0f * (xx + zz),	2.0f * (yz - wx),			0.0f,
			2.0f * (xz - wy),			2.0f * (yz + wx),			1.0f - 2.0f * (xx + yy),	0.0f,
			0.0f,						0.0f,						0.0f,						1.0f
		);
	}

	// Matrix creation from translation
	Matrix4x4 Matrix4x4::FromTranslation(const Vector3<float32>& translation)
	{
		return Matrix4x4(
			1.0f,			0.0f,			0.0f,			0.f,
			0.0f,			1.0f,			0.0f,			0.f,
			0.0f,			0.0f,			1.0f,			0.f,		
			translation.X,	translation.Y,	translation.Z,	1.0f
		);
	}

	// Matrix creation from scale
	Matrix4x4 Matrix4x4::FromScale(const Vector3<float32>& scale)
	{
		return Matrix4x4(
			scale.X,	0.0f,		0.0f,		0.0f,
			0.0f,		scale.Y,	0.0f,		0.0f,
			0.0f,		0.0f,		scale.Z,	0.0f,
			0.0f,		0.0f,		0.0f,		1.0f
		);
	}

	// Perspective view matrix creation 
	Matrix4x4 Matrix4x4::FromPerspectiveView(float32 fov, float32 aspect, float32 _near, float32 _far)
	{
		float32 scale = std::tanf(fov * 0.5f) * _near;
		float32 top = scale;
		float32 bottom = -top;
		float32 right = aspect * scale;
		float32 left = -right;
		float32 zSubtraction = _far - _near;
		float32 zSum = _far + _near;

		return Matrix4x4(
			(2 * _near) / (right - left),		0.0f,								0.0f,													0.0f, 
			0.0f,								(2 * _near) / (top - bottom),		0.0f,													0.0f,
			(right + left) / (right - left),	(top + bottom) / (top - bottom),	-zSum / zSubtraction,									-1.0f,
			0.0f,								0.0f,								-(2.f * _far * _near) / zSubtraction,					0.0f
		);
	}

	// Orthographic view matrix creation 
	Matrix4x4 Matrix4x4::FromOrthographicView(float32 left, float32 right, float32 bottom, float32 top, float32 _near, float32 _far)
	{
		float32 width = right - left;
		float32 height = top - bottom;
		float32 depth = _far - _near;

		return Matrix4x4(
			2.0f / width, 0.0f, 0.0f, 0.0f,
			0.0f, 2.0f / height, 0.0f, 0.0f,
			0.0f, 0.0f, -2.0f / depth, 0.0f,
			-(right + left) / width, -(top + bottom) / height, -(_far + _near) / depth, 1.0f
		);
	}

	// Look at matrix creation
	Matrix4x4 Matrix4x4::LookAt(const Vector3<float32>& eye, const Vector3<float32>& target, const Vector3<float32>& up)
	{
		Vector3<float32> zAxis = (eye - target).Normalized();
		Vector3<float32> xAxis = up.Cross(zAxis).Normalized();
		Vector3<float32> yAxis = zAxis.Cross(xAxis);

		return Matrix4x4(
			xAxis.X,			yAxis.X,				zAxis.X,			0,
			xAxis.Y,			yAxis.Y,				zAxis.Y,			0,
			xAxis.Z,			yAxis.Z,				zAxis.Z,			0,
			-xAxis.Dot(eye),	-yAxis.Dot(eye),		-zAxis.Dot(eye),	1
		);
	}

	// Matrix-scalar multiplication C * M
	Matrix4x4 operator*(float32 scalar, const Matrix4x4& matrix)
	{
		return matrix * scalar;
	}

	// Row-Major matrix-vector multiplication v * M
	Vector4<float32> operator*(const Vector4<float32>& vector, const Matrix4x4& matrix)
	{
		return ME::Core::Math::Vector4D(
			vector.x * matrix.m11 + vector.y * matrix.m21 + vector.z * matrix.m31 + vector.w * matrix.m41,
			vector.x * matrix.m12 + vector.y * matrix.m22 + vector.z * matrix.m32 + vector.w * matrix.m42,
			vector.x * matrix.m13 + vector.y * matrix.m23 + vector.z * matrix.m33 + vector.w * matrix.m43,
			vector.x * matrix.m14 + vector.y * matrix.m24 + vector.z * matrix.m34 + vector.w * matrix.m44
		);
	}
}