#include "Quaternion.h"
#include "Matrix4x4.h"
#include "Vector3D.h"
#include "Vector4D.h"

namespace Pawn::Core::Math
{
	inline const Quaternion Quaternion::Identity(1.0f, 0.0f, 0.0f, 0.0f);

	Quaternion Quaternion::FromRadians(float32 _x, float32 _y, float32 _z)
	{
		Quaternion xRes(_x, Vector3<float32>::RightVector);
		Quaternion yRes(_y, Vector3<float32>::UpVector);
		Quaternion zRes(_z, Vector3<float32>::ForwardVector);
		return (zRes * yRes * xRes).Normalized();
	}

	Quaternion Quaternion::FromRadians(const Vector3<float32>& radians)
	{
		Quaternion xRes(radians.x, Vector3<float32>::RightVector);
		Quaternion yRes(radians.y, Vector3<float32>::UpVector);
		Quaternion zRes(radians.z, Vector3<float32>::ForwardVector);
		return (zRes * yRes * xRes).Normalized();
	}

	Quaternion Quaternion::FromEulerAngles(float32 yaw, float32 pitch, float32 roll)
	{
		float32 cy = std::cos(yaw * 0.5f);
		float32 sy = std::sin(yaw * 0.5f);
		float32 cp = std::cos(pitch * 0.5f);
		float32 sp = std::sin(pitch * 0.5f);
		float32 cr = std::cos(roll * 0.5f);
		float32 sr = std::sin(roll * 0.5f);

		Quaternion result(
			cr * cp * cy + sr * sp * sy,
			sr * cp * cy - cr * sp * sy,
			cr * sp * cy + sr * cp * sy,
			cr * cp * sy - sr * sp * cy
		);
		return result.Normalized();
	}

	Quaternion Quaternion::FromEulerAngles(const Vector3<float32>& angles)
	{
		return FromEulerAngles(angles.z, angles.y, angles.x);
	}

	Matrix4x4 Quaternion::ToMatrix() const
	{
		float32 xx = x * x;
		float32 yy = y * y;
		float32 zz = z * z;
		float32 xy = x * y;
		float32 xz = x * z;
		float32 yz = y * z;
		float32 wx = w * x;
		float32 wy = w * y;
		float32 wz = w * z;

		return Matrix4x4(
			1.0f - 2.0f * (yy + zz), 2.0f * (xy - wz), 2.0f * (xz + wy), 0.0f,
			2.0f * (xy + wz), 1.0f - 2.0f * (xx + zz), 2.0f * (yz - wx), 0.0f,
			2.0f * (xz - wy), 2.0f * (yz + wx), 1.0f - 2.0f * (xx + yy), 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		);
	}

	Vector3<float32> Quaternion::RotateVector(const Vector3<float32>& vec) const
	{
		Quaternion vecQuat(0.0f, vec.x, vec.y, vec.z);
		Quaternion result = (*this) * vecQuat * Inverse();
		return Vector3<float32>(result.x, result.y, result.z);
	}

	Quaternion Quaternion::Inverse() const
	{
		float32 lenSq = LengthSquared();
		if (lenSq > 1e-6f)
		{
			float32 invLenSq = 1.0f / lenSq;
			return Quaternion(w * invLenSq, -x * invLenSq, -y * invLenSq, -z * invLenSq);
		}
		return Quaternion(1.0f, 0.0f, 0.0f, 0.0f);
	}

	Quaternion Quaternion::Conjugate() const
	{
		return Quaternion(w, -x, -y, -z);
	}

	Quaternion Quaternion::Normalized() const
	{
		float32 len = Length();
		if (len > 1e-6f)
		{
			float32 invLen = 1.0f / len;
			return Quaternion(w * invLen, x * invLen, y * invLen, z * invLen);
		}
		return Quaternion(1.0f, 0.0f, 0.0f, 0.0f);
	}

	Quaternion& Quaternion::Normalize()
	{
		float32 len = Length();
		if (len > 1e-6f)
		{
			float32 invLen = 1.0f / len;
			w *= invLen;
			x *= invLen;
			y *= invLen;
			z *= invLen;
		}
		return *this;
	}

	float32 Quaternion::LengthSquared() const
	{
		return w * w + x * x + y * y + z * z;
	}

	float32 Quaternion::Length() const
	{
		return std::sqrt(LengthSquared());
	}

	Quaternion& Quaternion::operator*=(float32 number)
	{
		w *= number;
		x *= number;
		y *= number;
		z *= number;
		return *this;
	}

	Quaternion& Quaternion::operator*=(const Quaternion& other)
	{
		*this = (*this) * other;
		return *this;
	}

	Quaternion Quaternion::operator*(float32 number) const
	{
		return Quaternion(w * number, x * number, y * number, z * number);
	}

	Quaternion Quaternion::operator*(const Quaternion& other) const
	{
		return Quaternion(
			w * other.w - x * other.x - y * other.y - z * other.z,
			w * other.x + x * other.w + y * other.z - z * other.y,
			w * other.y - x * other.z + y * other.w + z * other.x,
			w * other.z + x * other.y - y * other.x + z * other.w
		);
	}

	Quaternion& Quaternion::operator=(const Quaternion& other)
	{
		w = other.w;
		x = other.x;
		y = other.y;
		z = other.z;
		return *this;
	}

	bool Quaternion::operator==(const Quaternion& other) const
	{
		float32 epsilon = 1e-6f;
		return std::abs(w - other.w) < epsilon &&
			std::abs(x - other.x) < epsilon &&
			std::abs(y - other.y) < epsilon &&
			std::abs(z - other.z) < epsilon;
	}

	Quaternion::Quaternion(float32 _w, float32 _x, float32 _y, float32 _z) : w(_w), x(_x), y(_y), z(_z) {}

	Quaternion::Quaternion(const Vector4<float32>& val) : w(val.w), x(val.x), y(val.y), z(val.z) {}

	Quaternion::Quaternion(float32 scalar, const Vector3<float32>& axis, bool degrees)
	{
		if (degrees)
			scalar = (float32)RAD(scalar);

		if (!std::isfinite(scalar) || !axis.LengthSquared())
			return;

		float32 halfAngle = scalar * 0.5f;
		float32 sinHalf = std::sin(halfAngle);
		Vector3<float32> normAxis = axis.Normalized();

		w = std::cos(halfAngle);
		x = sinHalf * normAxis.x;
		y = sinHalf * normAxis.y;
		z = sinHalf * normAxis.z;
	}

	Quaternion::Quaternion() : w(1.0f), x(0.0f), y(0.0f), z(0.0f) {}


}