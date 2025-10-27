#include "Matrix4x4.hpp"

#include "Math.hpp"
#include "Core/Math/Quaternion.hpp"

namespace ME::Core::Math
{
	inline PMatrix4x4::PMatrix4x4()
		: a11(1.0f), a12(0.0f), a13(0.0f), a14(0.0f),
		a21(0.0f), a22(1.0f), a23(0.0f), a24(0.0f),
		a31(0.0f), a32(0.0f), a33(1.0f), a34(0.0f),
		a41(0.0f), a42(0.0f), a43(0.0f), a44(1.0f)
	{
	}

	inline PMatrix4x4::PMatrix4x4(float32 scalar) noexcept
		: a11(scalar), a12(0.0f), a13(0.0f), a14(0.0f),
		a21(0.0f), a22(scalar), a23(0.0f), a24(0.0f),
		a31(0.0f), a32(0.0f), a33(scalar), a34(0.0f),
		a41(0.0f), a42(0.0f), a43(0.0f), a44(scalar)
	{
	}

	inline PMatrix4x4::PMatrix4x4(
		const float32& a11, const float32& a12, const float32& a13, const float32& a14,
		const float32& a21, const float32& a22, const float32& a23, const float32& a24,
		const float32& a31, const float32& a32, const float32& a33, const float32& a34,
		const float32& a41, const float32& a42, const float32& a43, const float32& a44)
		: a11(a11), a12(a12), a13(a13), a14(a14),
		a21(a21), a22(a22), a23(a23), a24(a24),
		a31(a31), a32(a32), a33(a33), a34(a34),
		a41(a41), a42(a42), a43(a43), a44(a44)
	{
	}

	PMatrix4x4 PMatrix4x4::operator*(const float32& scalar) const
	{
		return PMatrix4x4(
			a11 * scalar, a12 * scalar, a13 * scalar, a14 * scalar,
			a21 * scalar, a22 * scalar, a23 * scalar, a24 * scalar,
			a31 * scalar, a32 * scalar, a33 * scalar, a34 * scalar,
			a41 * scalar, a42 * scalar, a43 * scalar, a44 * scalar
		);
	}

	PMatrix4x4& PMatrix4x4::operator*=(const float32& scalar)
	{
		a11 *= scalar; a12 *= scalar; a13 *= scalar; a14 *= scalar;
		a21 *= scalar; a22 *= scalar; a23 *= scalar; a24 *= scalar;
		a31 *= scalar; a32 *= scalar; a33 *= scalar; a34 *= scalar;
		a41 *= scalar; a42 *= scalar; a43 *= scalar; a44 *= scalar;
		return *this;
	}

	PMatrix4x4& PMatrix4x4::operator*=(const int32& scalar)
	{
		return *this *= static_cast<float32>(scalar);
	}

	PMatrix4x4 PMatrix4x4::operator*(const PMatrix4x4& other) const
	{
		return PMatrix4x4(
			a11 * other.a11 + a12 * other.a21 + a13 * other.a31 + a14 * other.a41,
			a11 * other.a12 + a12 * other.a22 + a13 * other.a32 + a14 * other.a42,
			a11 * other.a13 + a12 * other.a23 + a13 * other.a33 + a14 * other.a43,
			a11 * other.a14 + a12 * other.a24 + a13 * other.a34 + a14 * other.a44,

			a21 * other.a11 + a22 * other.a21 + a23 * other.a31 + a24 * other.a41,
			a21 * other.a12 + a22 * other.a22 + a23 * other.a32 + a24 * other.a42,
			a21 * other.a13 + a22 * other.a23 + a23 * other.a33 + a24 * other.a43,
			a21 * other.a14 + a22 * other.a24 + a23 * other.a34 + a24 * other.a44,

			a31 * other.a11 + a32 * other.a21 + a33 * other.a31 + a34 * other.a41,
			a31 * other.a12 + a32 * other.a22 + a33 * other.a32 + a34 * other.a42,
			a31 * other.a13 + a32 * other.a23 + a33 * other.a33 + a34 * other.a43,
			a31 * other.a14 + a32 * other.a24 + a33 * other.a34 + a34 * other.a44,

			a41 * other.a11 + a42 * other.a21 + a43 * other.a31 + a44 * other.a41,
			a41 * other.a12 + a42 * other.a22 + a43 * other.a32 + a44 * other.a42,
			a41 * other.a13 + a42 * other.a23 + a43 * other.a33 + a44 * other.a43,
			a41 * other.a14 + a42 * other.a24 + a43 * other.a34 + a44 * other.a44
		);
	}

	PMatrix4x4 PMatrix4x4::operator+(const PMatrix4x4& other) const
	{
		return PMatrix4x4(
			a11 + other.a11, a12 + other.a12, a13 + other.a13, a14 + other.a14,
			a21 + other.a21, a22 + other.a22, a23 + other.a23, a24 + other.a24,
			a31 + other.a31, a32 + other.a32, a33 + other.a33, a34 + other.a34,
			a41 + other.a41, a42 + other.a42, a43 + other.a43, a44 + other.a44
		);
	}

	PMatrix4x4& PMatrix4x4::operator+=(const PMatrix4x4& other)
	{
		a11 += other.a11; a12 += other.a12; a13 += other.a13; a14 += other.a14;
		a21 += other.a21; a22 += other.a22; a23 += other.a23; a24 += other.a24;
		a31 += other.a31; a32 += other.a32; a33 += other.a33; a34 += other.a34;
		a41 += other.a41; a42 += other.a42; a43 += other.a43; a44 += other.a44;
		return *this;
	}

	PMatrix4x4 PMatrix4x4::operator-(const PMatrix4x4& other) const
	{
		return PMatrix4x4(
			a11 - other.a11, a12 - other.a12, a13 - other.a13, a14 - other.a14,
			a21 - other.a21, a22 - other.a22, a23 - other.a23, a24 - other.a24,
			a31 - other.a31, a32 - other.a32, a33 - other.a33, a34 - other.a34,
			a41 - other.a41, a42 - other.a42, a43 - other.a43, a44 - other.a44
		);
	}

	PMatrix4x4& PMatrix4x4::operator-=(const PMatrix4x4& other)
	{
		a11 -= other.a11; a12 -= other.a12; a13 -= other.a13; a14 -= other.a14;
		a21 -= other.a21; a22 -= other.a22; a23 -= other.a23; a24 -= other.a24;
		a31 -= other.a31; a32 -= other.a32; a33 -= other.a33; a34 -= other.a34;
		a41 -= other.a41; a42 -= other.a42; a43 -= other.a43; a44 -= other.a44;
		return *this;
	}

	bool PMatrix4x4::operator==(const PMatrix4x4& other) const
	{
		constexpr float32 epsilon = 1e-6f;
		return std::abs(a11 - other.a11) < epsilon &&
			std::abs(a12 - other.a12) < epsilon &&
			std::abs(a13 - other.a13) < epsilon &&
			std::abs(a14 - other.a14) < epsilon &&
			std::abs(a21 - other.a21) < epsilon &&
			std::abs(a22 - other.a22) < epsilon &&
			std::abs(a23 - other.a23) < epsilon &&
			std::abs(a24 - other.a24) < epsilon &&
			std::abs(a31 - other.a31) < epsilon &&
			std::abs(a32 - other.a32) < epsilon &&
			std::abs(a33 - other.a33) < epsilon &&
			std::abs(a34 - other.a34) < epsilon &&
			std::abs(a41 - other.a41) < epsilon &&
			std::abs(a42 - other.a42) < epsilon &&
			std::abs(a43 - other.a43) < epsilon &&
			std::abs(a44 - other.a44) < epsilon;
	}

	float32 PMatrix4x4::Determinant() const
	{
		return a11 * MAT3x3_DET(a22, a23, a24, a32, a33, a34, a42, a43, a44)
			- a12 * MAT3x3_DET(a21, a23, a24, a31, a33, a34, a41, a43, a44)
			+ a13 * MAT3x3_DET(a21, a22, a24, a31, a32, a34, a41, a42, a44)
			- a14 * MAT3x3_DET(a21, a22, a23, a31, a32, a33, a41, a42, a43);
	}

	PMatrix4x4 PMatrix4x4::Invert() const
	{
		float32 det = Determinant();
		if (std::abs(det) < 1e-6f)
			return PMatrix4x4(0.0f);

		float32 invDet = 1.0f / det;

		PMatrix4x4 result;
		result.a11 = invDet * (a22 * (a33 * a44 - a34 * a43) - a23 * (a32 * a44 - a34 * a42) + a24 * (a32 * a43 - a33 * a42));
		result.a12 = invDet * -(a21 * (a33 * a44 - a34 * a43) - a23 * (a31 * a44 - a34 * a41) + a24 * (a31 * a43 - a33 * a41));
		result.a13 = invDet * (a21 * (a32 * a44 - a34 * a42) - a22 * (a31 * a44 - a34 * a41) + a24 * (a31 * a42 - a32 * a41));
		result.a14 = invDet * -(a21 * (a32 * a43 - a33 * a42) - a22 * (a31 * a43 - a33 * a41) + a23 * (a31 * a42 - a32 * a41));

		result.a21 = invDet * -(a12 * (a33 * a44 - a34 * a43) - a13 * (a32 * a44 - a34 * a42) + a14 * (a32 * a43 - a33 * a42));
		result.a22 = invDet * (a11 * (a33 * a44 - a34 * a43) - a13 * (a31 * a44 - a34 * a41) + a14 * (a31 * a43 - a33 * a41));
		result.a23 = invDet * -(a11 * (a32 * a44 - a34 * a42) - a12 * (a31 * a44 - a34 * a41) + a14 * (a31 * a42 - a32 * a41));
		result.a24 = invDet * (a11 * (a32 * a43 - a33 * a42) - a12 * (a31 * a43 - a33 * a41) + a13 * (a31 * a42 - a32 * a41));

		result.a31 = invDet * (a12 * (a23 * a44 - a24 * a43) - a13 * (a22 * a44 - a24 * a42) + a14 * (a22 * a43 - a23 * a42));
		result.a32 = invDet * -(a11 * (a23 * a44 - a24 * a43) - a13 * (a21 * a44 - a24 * a41) + a14 * (a21 * a43 - a23 * a41));
		result.a33 = invDet * (a11 * (a22 * a44 - a24 * a42) - a12 * (a21 * a44 - a24 * a41) + a14 * (a21 * a42 - a22 * a41));
		result.a34 = invDet * -(a11 * (a22 * a43 - a23 * a42) - a12 * (a21 * a43 - a23 * a41) + a13 * (a21 * a42 - a22 * a41));

		result.a41 = invDet * -(a12 * (a23 * a34 - a24 * a33) - a13 * (a22 * a34 - a24 * a32) + a14 * (a22 * a33 - a23 * a32));
		result.a42 = invDet * (a11 * (a23 * a34 - a24 * a33) - a13 * (a21 * a34 - a24 * a31) + a14 * (a21 * a33 - a23 * a31));
		result.a43 = invDet * -(a11 * (a22 * a34 - a24 * a32) - a12 * (a21 * a34 - a24 * a31) + a14 * (a21 * a32 - a22 * a31));
		result.a44 = invDet * (a11 * (a22 * a33 - a23 * a32) - a12 * (a21 * a33 - a23 * a31) + a13 * (a21 * a32 - a22 * a31));

		return result;
	}

	PMatrix4x4 PMatrix4x4::Adjoint() const
	{
		PMatrix4x4 result;

		result.a11 = MAT3x3_DET(a22, a23, a24, a32, a33, a34, a42, a43, a44);
		result.a12 = -MAT3x3_DET(a21, a23, a24, a31, a33, a34, a41, a43, a44);
		result.a13 = MAT3x3_DET(a21, a22, a24, a31, a32, a34, a41, a42, a44);
		result.a14 = -MAT3x3_DET(a21, a22, a23, a31, a32, a33, a41, a42, a43);

		result.a21 = -MAT3x3_DET(a12, a13, a14, a32, a33, a34, a42, a43, a44);
		result.a22 = MAT3x3_DET(a11, a13, a14, a31, a33, a34, a41, a43, a44);
		result.a23 = -MAT3x3_DET(a11, a12, a14, a31, a32, a34, a41, a42, a44);
		result.a24 = MAT3x3_DET(a11, a12, a13, a31, a32, a33, a41, a42, a43);

		result.a31 = MAT3x3_DET(a12, a13, a14, a22, a23, a24, a42, a43, a44);
		result.a32 = -MAT3x3_DET(a11, a13, a14, a21, a23, a24, a41, a43, a44);
		result.a33 = MAT3x3_DET(a11, a12, a14, a21, a22, a24, a41, a42, a44);
		result.a34 = -MAT3x3_DET(a11, a12, a13, a21, a22, a23, a41, a42, a43);

		result.a41 = -MAT3x3_DET(a12, a13, a14, a22, a23, a24, a32, a33, a34);
		result.a42 = MAT3x3_DET(a11, a13, a14, a21, a23, a24, a31, a33, a34);
		result.a43 = -MAT3x3_DET(a11, a12, a14, a21, a22, a24, a31, a32, a34);
		result.a44 = MAT3x3_DET(a11, a12, a13, a21, a22, a23, a31, a32, a33);

		return result;
	}

	PMatrix4x4 PMatrix4x4::Abs() const
	{
		return PMatrix4x4(
			std::abs(a11), std::abs(a12), std::abs(a13), std::abs(a14),
			std::abs(a21), std::abs(a22), std::abs(a23), std::abs(a24),
			std::abs(a31), std::abs(a32), std::abs(a33), std::abs(a34),
			std::abs(a41), std::abs(a42), std::abs(a43), std::abs(a44)
		);
	}

	PMatrix4x4 PMatrix4x4::Transpose() const
	{
		return PMatrix4x4(
			a11, a21, a31, a41,
			a12, a22, a32, a42,
			a13, a23, a33, a43,
			a14, a24, a34, a44
		);
	}

	Vector3<float32> PMatrix4x4::Transform(const Vector3<float32>& axis) const
	{
		float32 w = a14 * axis.x + a24 * axis.y + a34 * axis.z + a44;
		return Vector3(
			(a11 * axis.x + a12 * axis.y + a13 * axis.z + a14) / w,
			(a21 * axis.x + a22 * axis.y + a23 * axis.z + a24) / w,
			(a31 * axis.x + a32 * axis.y + a33 * axis.z + a34) / w
		);
	}

	Vector3<float32> PMatrix4x4::TransformNormal(const Vector3<float32>& axis) const
	{
		return Vector3(
			(a11 * axis.x + a21 * axis.y + a31 * axis.z),
			(a12 * axis.x + a22 * axis.y + a32 * axis.z),
			(a13 * axis.x + a23 * axis.y + a33 * axis.z)
		);
	}

    Vector3<float32> PMatrix4x4::LookVector() const
    {
        return TransformNormal(Vector3<float32>::ForwardVector);
    }

    Vector3<float32> PMatrix4x4::UpVector() const
    {
        return TransformNormal(Vector3<float32>::UpVector);
    }

    Vector3<float32> PMatrix4x4::RightVector() const
    {
        return TransformNormal(Vector3<float32>::RightVector);
    }

    Vector3<float32> PMatrix4x4::GetTranslation() const
    {
        return Vector3(a14, a24, a34);
    }

    Vector3<float32> PMatrix4x4::GetScale() const
    {
		Vector3 xAxis(a11, a21, a31);
		Vector3 yAxis(a12, a22, a32);
		Vector3 zAxis(a13, a23, a33);
        return Vector3(xAxis.Length(), yAxis.Length(), zAxis.Length());
    }

    Vector3<float32> PMatrix4x4::GetEulerAnglesXYZ() const
    {
		float32 pitch, yaw, roll;

		if (fabs(a31) < 0.999999f)
		{
			yaw = std::asin(-a31);
			pitch = std::atan2(a32, a33);
			roll = std::atan2(a21, a11);
		}
		else
		{
			// Gimbal lock
			yaw = (a31 < 0 ? +PI / 2.0f : -PI / 2.0f);
			pitch = std::atan2(-a12, a22);
			roll = 0.0f;
		}

		return Vector3<float32>(
			ME::Core::Math::ToDegrees(pitch),
			ME::Core::Math::ToDegrees(yaw),
			ME::Core::Math::ToDegrees(roll)
		);
    }

    Vector3<float32> PMatrix4x4::GetEulerAnglesYXZ() const
	{
		float32 pitch, yaw, roll;

		if (fabs(a32) < 0.999999f)
		{
			yaw = std::atan2(a13, a33);
			pitch = std::asin(-a23);
			roll = std::atan2(a21, a22);
		}
		else
		{
			yaw = std::atan2(-a31, a11);
			pitch = (a32 < 0 ? +PI / 2.0f : -PI / 2.0f);
			roll = 0.0f;
		}

		return Vector3<float32>(
			ME::Core::Math::ToDegrees(pitch),
			ME::Core::Math::ToDegrees(yaw),
			ME::Core::Math::ToDegrees(roll)
		);
	}

    Vector3<float32> PMatrix4x4::GetEulerAnglesZYX() const
	{
		float32 pitch, yaw, roll;

		if (fabs(a31) < 0.999999f)
		{
			yaw = std::asin(-a31);
			pitch = std::atan2(a32, a33);
			roll = std::atan2(a21, a11);
		}
		else
		{
			yaw = (a31 < 0 ? +PI / 2.0f : -PI / 2.0f);
			pitch = std::atan2(-a12, a22);
			roll = 0.0f;
		}

		return Vector3<float32>(
			ME::Core::Math::ToDegrees(roll),
			ME::Core::Math::ToDegrees(pitch),
			ME::Core::Math::ToDegrees(yaw)
		);
	}

    PMatrix4x4 PMatrix4x4::FromQuaternion(const PQuaternion& q)
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

		return PMatrix4x4(
			1.0f - 2.0f * (yy + zz),	2.0f * (xy - wz),			2.0f * (xz + wy),			0.0f,
			2.0f * (xy + wz),			1.0f - 2.0f * (xx + zz),	2.0f * (yz - wx),			0.0f,
			2.0f * (xz - wy),			2.0f * (yz + wx),			1.0f - 2.0f * (xx + yy),	0.0f,
			0.0f,						0.0f,						0.0f,						1.0f
		);
	}

	PMatrix4x4 PMatrix4x4::FromTranslation(const Vector3<float32>& translation)
	{
		return PMatrix4x4(
			1.0f, 0.0f, 0.0f, translation.X,
			0.0f, 1.0f, 0.0f, translation.Y,
			0.0f, 0.0f, 1.0f, translation.Z,
			0.0f, 0.0f, 0.0f, 1.0f
		);
	}

	PMatrix4x4 PMatrix4x4::FromScale(const Vector3<float32>& scale)
	{
		return PMatrix4x4(
			scale.X, 0.0f, 0.0f, 0.0f,
			0.0f, scale.Y, 0.0f, 0.0f,
			0.0f, 0.0f, scale.Z, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		);
	}

	PMatrix4x4 PMatrix4x4::FromPerspectiveView(float32 fov, float32 aspect, float32 _near, float32 _far)
	{
		float32 tanHalfFov = std::tan(fov * 0.5f);
		float32 zRange = _far - _near;

		return PMatrix4x4(
			1.0f / (aspect * tanHalfFov), 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f / tanHalfFov, 0.0f, 0.0f,
			0.0f, 0.0f, _far / zRange, -_far * _near / zRange,
			0.0f, 0.0f, 1.0f, 0.0f
		);
	}

	PMatrix4x4 PMatrix4x4::FromOrthographicView(float32 left, float32 right, float32 bottom, float32 top, float32 _near, float32 _far)
	{
		return PMatrix4x4(
			2.0f / (right - left), 0.0f, 0.0f, -(right + left) / (right - left),
			0.0f, 2.0f / (top - bottom), 0.0f, -(top + bottom) / (top - bottom),
			0.0f, 0.0f, -2.0f / (_far - _near), -(_far + _near) / (_far - _near),
			0.0f, 0.0f, 0.0f, 1.0f
		);
	}

	PMatrix4x4 PMatrix4x4::LookAt(const Vector3<float32>& eye, const Vector3<float32>& target, const Vector3<float32>& up)
	{
		Vector3<float32> zAxis = (target - eye).Normalized();
		Vector3<float32> xAxis = up.Cross(zAxis).Normalized();
		Vector3<float32> yAxis = zAxis.Cross(xAxis);

		return PMatrix4x4(
			xAxis.X, yAxis.X, zAxis.X, 0,
			xAxis.Y, yAxis.Y, zAxis.Y, 0,
			xAxis.Z, yAxis.Z, zAxis.Z, 0,
			-xAxis.Dot(eye), -yAxis.Dot(eye), -zAxis.Dot(eye), 1
		);
	}

	PMatrix4x4 operator*(const float32& scalar, const PMatrix4x4& mat)
	{
		return mat * scalar;
	}
}