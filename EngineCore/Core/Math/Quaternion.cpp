#include "Quaternion.hpp"
#include "Matrix4x4.hpp"

namespace ME::Core::Math
{
	const PQuaternion PQuaternion::Identity = PQuaternion(1.0f, 0.0f, 0.0f, 0.0f);

	PQuaternion PQuaternion::FromRadians(float32 _x, float32 _y, float32 _z)
	{
		PQuaternion xRes(_x, Vector3<float32>::RightVector);
		PQuaternion yRes(_y, Vector3<float32>::UpVector);
		PQuaternion zRes(_z, Vector3<float32>::ForwardVector);
		return (zRes * yRes * xRes).Normalized();
	}

    PQuaternion PQuaternion::Slerp(const PQuaternion& q1, const PQuaternion& q2, float32 alpha)
    {
		float32 angle = acos(q1.Dot(q2));
        return q1 * ((sin(1 - alpha) * angle) / sin(angle)) + q2 * (sin(alpha * angle) / sin(angle));
    }

    PQuaternion PQuaternion::FromRadians(const Vector3<float32>& radians)
	{
		PQuaternion xRes(radians.x, Vector3<float32>::RightVector);
		PQuaternion yRes(radians.y, Vector3<float32>::UpVector);
		PQuaternion zRes(radians.z, Vector3<float32>::ForwardVector);
		return (zRes * yRes * xRes).Normalized();
	}

	PQuaternion PQuaternion::FromEulerAngles(float32 yaw, float32 pitch, float32 roll)
	{
		float32 cy = cos(yaw * 0.5f);
		float32 sy = sin(yaw * 0.5f);
		float32 cp = cos(pitch * 0.5f);
		float32 sp = sin(pitch * 0.5f);
		float32 cr = cos(roll * 0.5f);
		float32 sr = sin(roll * 0.5f);

		PQuaternion q;
		q.w = cy * cp * cr + sy * sp * sr;
		q.x = cy * sp * cr + sy * cp * sr;
		q.y = sy * cp * cr - cy * sp * sr;
		q.z = cy * cp * sr - sy * sp * cr;

		return q.Normalized();
	}

	inline PQuaternion PQuaternion::FromEulerAnglesXYZ(const Vector3<float32>& angles)
	{
		return FromEulerAngles(angles.x, angles.y, angles.z);
	}

	inline PQuaternion PQuaternion::FromEulerAnglesXYZ(float32 yaw, float32 pitch, float32 roll)
	{
		return FromEulerAngles(yaw, pitch, roll);
	}

	inline PQuaternion PQuaternion::FromEulerAnglesYXZ(const Vector3<float32>& angles)
	{
		return FromEulerAngles(angles.x, angles.y, angles.z);
	}

	inline PQuaternion PQuaternion::FromEulerAnglesYXZ(float32 yaw, float32 pitch, float32 roll)
	{
		return FromEulerAngles(pitch, yaw, roll);
	}

	PQuaternion PQuaternion::FromAxisAngle(const Vector3<float32>& axis, float32 angle)
	{
		float32 halfAngle = angle * 0.5f;
		float32 s = sinf(halfAngle);
		return PQuaternion(
			cosf(halfAngle),
			axis.X * s,
			axis.Y * s,
			axis.Z * s
		);
	}

	PMatrix4x4 PQuaternion::ToMatrix() const
	{
		float32 xx = (float32)(x * x);
		float32 yy = (float32)(y * y);
		float32 zz = (float32)(z * z);
		float32 xy = (float32)(x * y);
		float32 xz = (float32)(x * z);
		float32 yz = (float32)(y * z);
		float32 wx = (float32)(w * x);
		float32 wy = (float32)(w * y);
		float32 wz = (float32)(w * z);

		return PMatrix4x4(
			1.0f - 2.0f * (yy + zz), 2.0f * (xy - wz), 2.0f * (xz + wy), 0.0f,
			2.0f * (xy + wz), 1.0f - 2.0f * (xx + zz), 2.0f * (yz - wx), 0.0f,
			2.0f * (xz - wy), 2.0f * (yz + wx), 1.0f - 2.0f * (xx + yy), 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		);
	}

	Vector3<float32> PQuaternion::ToEulerAnglesXYZ() const
	{
		constexpr float32 EPSILON = 1e-6f;

		float32 yaw, roll;

		float32 sinPitch = 2.0f * (w * y - z * x);
		sinPitch = std::clamp(sinPitch, -1.0f, 1.0f);
		float32 pitch = asin(sinPitch);

		if (fabsf(pitch - static_cast<float32>(PI) / 2) < EPSILON)
		{
			yaw = atan2(2.0f * (x * z + w * y), 1.0f - 2.0f * (x * x + z * z));
			roll = 0.0f;
			return Vector3(roll, pitch, yaw);
		}
		else if (fabsf(pitch + static_cast<float32>(PI) / 2) < EPSILON)
		{
			yaw = atan2(-2.0f * (x * z + w * y), 1.0f - 2.0f * (x * x + z * z));
			roll = 0.0f;
			return Vector3(roll, pitch, yaw);
		}

		yaw = atan2(2.0f * (w * z + x * y), 1.0f - 2.0f * (y * y + z * z));
		roll = atan2(2.0f * (w * x + y * z), 1.0f - 2.0f * (x * x + y * y));

		return Vector3(roll, pitch, yaw);
	}

	Vector3<float32> PQuaternion::ToEulerAnglesYXZ() const
	{
		constexpr float32 EPSILON = 1e-6f;

		float32 yaw, roll;

		float32 sinPitch = 2.0f * (w * y - z * x);
		sinPitch = std::clamp(sinPitch, -1.0f, 1.0f);
		float32 pitch = asin(sinPitch);

		if (fabsf(pitch - static_cast<float32>(PI) / 2) < EPSILON)
		{
			yaw = atan2(2.0f * (x * z + w * y), 1.0f - 2.0f * (x * x + z * z));
			roll = 0.0f;
			return Vector3(pitch, roll, yaw);
		}
		else if (fabsf(pitch + static_cast<float32>(PI) / 2) < EPSILON)
		{
			yaw = atan2(-2.0f * (x * z + w * y), 1.0f - 2.0f * (x * x + z * z));
			roll = 0.0f;
			return Vector3(pitch, roll, yaw);
		}

		yaw = atan2(2.0f * (w * z + x * y), 1.0f - 2.0f * (y * y + z * z));
		roll = atan2(2.0f * (w * x + y * z), 1.0f - 2.0f * (x * x + y * y));

		return Vector3(pitch, roll, yaw);
    }

    Vector3<float32> PQuaternion::RotateVector(const Vector3<float32>& vec) const
	{
		PQuaternion vecQuat(0.0f, vec.x, vec.y, vec.z);
		PQuaternion result = (*this) * vecQuat * Inverse();
		return Vector3<float32>(result.x, result.y, result.z);
	}

	PQuaternion PQuaternion::Inverse() const
	{
		float32 lenSq = LengthSquared();
		if (lenSq > 1e-6f)
		{
			float32 invLenSq = 1.0f / lenSq;
			return PQuaternion(w * invLenSq, -x * invLenSq, -y * invLenSq, -z * invLenSq);
		}
		return PQuaternion(1.0f, 0.0f, 0.0f, 0.0f);
	}

	PQuaternion PQuaternion::Conjugate() const
	{
		return PQuaternion(w, -x, -y, -z);
	}

	PQuaternion PQuaternion::Normalized() const
	{
		float32 len = Length();
		if (len > 1e-6f)
		{
			float32 invLen = 1.0f / len;
			return PQuaternion(w * invLen, x * invLen, y * invLen, z * invLen);
		}
		return PQuaternion(1.0f, 0.0f, 0.0f, 0.0f);
	}

	PQuaternion& PQuaternion::Normalize()
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

	float32 PQuaternion::LengthSquared() const
	{
		return w * w + x * x + y * y + z * z;
	}

    float32 PQuaternion::Dot(const PQuaternion& other) const
    {
        return x * other.x + y * other.y + z * other.z + w * other.w;
    }

    float32 PQuaternion::Length() const
	{
		return sqrt(LengthSquared());
	}

	PQuaternion& PQuaternion::operator*=(float32 number)
	{
		w *= number;
		x *= number;
		y *= number;
		z *= number;
		return *this;
	}

	PQuaternion& PQuaternion::operator*=(const PQuaternion& other)
	{
		*this = (*this) * other;
		return *this;
	}

    PQuaternion PQuaternion::operator/(float32 number) const
    {
		PQuaternion result;
		result.x = x / number;
		result.y = y / number;
		result.z = z / number;
		result.w = w / number;
		return result;
    }

    PQuaternion PQuaternion::operator/(const PQuaternion& other) const
    {
		PQuaternion result;
		result.x = x / other.x;
		result.y = y / other.y;
		result.z = z / other.z;
		result.w = w / other.w;
		return result;
    }

    PQuaternion& PQuaternion::operator/=(float32 number)
    {
		x /= number;
		y /= number;
		z /= number;
		w /= number;
		return *this;
    }

    PQuaternion& PQuaternion::operator/=(const PQuaternion& other)
    {
		x /= other.x;
		y /= other.y;
		z /= other.z;
		w /= other.w;
		return *this;
    }

    PQuaternion PQuaternion::operator+(const PQuaternion& other) const
    {
		PQuaternion result;
		result.x = x + other.x;
		result.y = y + other.y;
		result.z = z + other.z;
		result.w = w + other.w;
		return result;
    }

    PQuaternion& PQuaternion::operator+=(const PQuaternion& other)
    {
		x += other.x;
		y += other.y;
		z += other.z;
		w += other.w;
		return *this;
    }

    PQuaternion PQuaternion::operator-(const PQuaternion& other) const
    {
		PQuaternion result;
		result.x = x - other.x;
		result.y = y - other.y;
		result.z = z - other.z;
		result.w = w - other.w;
        return result;
    }

    PQuaternion& PQuaternion::operator-=(const PQuaternion& other)
    {
		x -= other.x;
		y -= other.y;
		z -= other.z;
		w -= other.w;
        return *this;
    }

    PQuaternion PQuaternion::operator*(float32 number) const
	{
		return PQuaternion(w * number, x * number, y * number, z * number);
	}

	PQuaternion PQuaternion::operator*(const PQuaternion& other) const
	{
		return PQuaternion(
			w * other.w - x * other.x - y * other.y - z * other.z,
			w * other.x + x * other.w + y * other.z - z * other.y,
			w * other.y - x * other.z + y * other.w + z * other.x,
			w * other.z + x * other.y - y * other.x + z * other.w
		);
	}

	PQuaternion& PQuaternion::operator=(const PQuaternion& other)
	{
		w = other.w;
		x = other.x;
		y = other.y;
		z = other.z;
		return *this;
	}

	bool PQuaternion::operator==(const PQuaternion& other) const
	{
		float32 epsilon = 1e-6f;
		return abs(w - other.w) < epsilon &&
			abs(x - other.x) < epsilon &&
			abs(y - other.y) < epsilon &&
			abs(z - other.z) < epsilon;
	}

	PQuaternion::PQuaternion(float32 _w, float32 _x, float32 _y, float32 _z) : w(_w), x(_x), y(_y), z(_z) {}

	PQuaternion::PQuaternion(const Vector4<float32>& val) : w(val.w), x(val.x), y(val.y), z(val.z) {}

	PQuaternion::PQuaternion(float32 scalar, const Vector3<float32>& axis, bool degrees)
	{
		if (degrees)
			scalar = (float32)RAD(scalar);

		if (!isfinite(scalar) || !axis.LengthSquared())
			return;

		float32 halfAngle = scalar * 0.5f;
		float32 sinHalf = sin(halfAngle);
		Vector3<float32> normAxis = axis.Normalized();

		w = cos(halfAngle);
		x = sinHalf * normAxis.x;
		y = sinHalf * normAxis.y;
		z = sinHalf * normAxis.z;
	}

	PQuaternion::PQuaternion() : w(1.0f), x(0.0f), y(0.0f), z(0.0f) {}


}