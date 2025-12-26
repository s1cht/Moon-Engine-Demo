#include "Quaternion.hpp"

#include "Math.hpp"
#include "Matrix4x4.hpp"

namespace ME::Core::Math
{
	const Quaternion Quaternion::Identity = Quaternion(1.0f, 0.0f, 0.0f, 0.0f);

	Quaternion Quaternion::Slerp(const Quaternion & q2, float32 alpha)
	{
		float32 dot = Dot(q2);

		dot = std::clamp(dot, -1.0f, 1.0f);

		float32 angle = acos(dot);

		if (std::abs(angle) < 1e-6f)
			return *this;

		float32 sinAngle = sin(angle);
		return *this * (sin((1.0f - alpha) * angle) / sinAngle) + q2 * (sin(alpha * angle) / sinAngle);
	}

	inline Quaternion Quaternion::FromEulerAnglesXYZ(const Vector3<float32>& angles)
	{
		return FromEulerAnglesXYZ(angles.x, angles.y, angles.z);
	}

	inline Quaternion Quaternion::FromEulerAnglesXYZ(float32 yaw, float32 pitch, float32 roll)
	{
		float32 cx = cosf(pitch * 0.5f);
		float32 sx = sinf(pitch * 0.5f);
		float32 cy = cosf(yaw * 0.5f);
		float32 sy = sinf(yaw * 0.5f);
		float32 cz = cosf(roll * 0.5f);
		float32 sz = sinf(roll * 0.5f);

		Quaternion q;
		q.w = cx * cy * cz - sx * sy * sz;
		q.x = sx * cy * cz + cx * sy * sz;
		q.y = cx * sy * cz - sx * cy * sz;
		q.z = cx * cy * sz + sx * sy * cz;
		return q;
	}

	inline Quaternion Quaternion::FromEulerAnglesYXZ(const Vector3<float32>& angles)
	{
		return FromEulerAnglesYXZ(angles.x, angles.y, angles.z);
	}

	Quaternion Quaternion::FromEulerAnglesYXZ(float32 yaw, float32 pitch, float32 roll)
	{
		float32 cx = cosf(pitch * 0.5f);
		float32 sx = sinf(pitch * 0.5f);
		float32 cy = cosf(yaw * 0.5f);
		float32 sy = sinf(yaw * 0.5f);
		float32 cz = cosf(roll * 0.5f);
		float32 sz = sinf(roll * 0.5f);

		Quaternion q;
		q.w = cy * cx * cz + sy * sx * sz;
		q.x = cy * sx * cz + sy * cx * sz;
		q.y = sy * cx * cz - cy * sx * sz;
		q.z = cy * cx * sz - sy * sx * cz;
		return q;
	}

	Quaternion Quaternion::FromEulerAnglesZYX(const Vector3<float32>& angles)
	{
		return FromEulerAnglesZYX(angles.x, angles.y, angles.z);
	}

	Quaternion Quaternion::FromEulerAnglesZYX(float32 yaw, float32 pitch, float32 roll)
	{
		float32 cx = cosf(pitch * 0.5f);
		float32 sx = sinf(pitch * 0.5f);
		float32 cy = cosf(yaw * 0.5f);
		float32 sy = sinf(yaw * 0.5f);
		float32 cz = cosf(roll * 0.5f);
		float32 sz = sinf(roll * 0.5f);

		Quaternion q;
		q.w = cz * cy * cx + sz * sy * sx;
		q.x = cx * cy * sz - sx * sy * cz;
		q.y = cx * sy * cz + cy * sx * sz;
		q.z = cy * cx * sz - sx * sy * cz;
		return q;
	}

	Quaternion Quaternion::FromAxisAngle(const Vector3<float32>& axis, float32 angle)
	{
		float32 halfAngle = angle * 0.5f;
		float32 s = sinf(halfAngle);
		return Quaternion(
			cosf(halfAngle),
			axis.X * s,
			axis.Y * s,
			axis.Z * s
		);
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
			1.0f - 2.0f * (yy + zz),	2.0f * (xy - wz),			2.0f * (xz + wy),			0.0f,
			2.0f * (xy + wz),			1.0f - 2.0f * (xx + zz),	2.0f * (yz - wx),			0.0f,
			2.0f * (xz - wy),			2.0f * (yz + wx),			1.0f - 2.0f * (xx + yy),	0.0f,
			0.0f,						0.0f,						0.0f,						1.0f
		);
	}

	Vector3<float32> Quaternion::ToEulerAnglesXYZ() const
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

	Vector3<float32> Quaternion::ToEulerAnglesYXZ() const
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

	Quaternion Quaternion::LookAt(const Vector3<float32>& position, const Vector3<float32>& lookAtPosition)
	{
		Vector3<float32> forwardVector = (lookAtPosition - position).Normalized();
		Vector3<float32> referenceForward = Vector3<float32>::ForwardVector;

		float32 dot = referenceForward.Dot(forwardVector);

		dot = ME::Core::Math::Clamp(dot, -1.0f, 1.0f);

		if (std::abs(dot + 1.0f) < 1e-6f)
		{
			Vector3<float32> axis = Vector3<float32>::UpVector;
			if (std::abs(referenceForward.Dot(axis)) > 0.999f)
				axis = Vector3<float32>::RightVector;

			return FromAxisAngle(axis, static_cast<float32>(PI));
		}

		if (std::abs(dot - 1.0f) < 1e-6f)
			return Quaternion::Identity;

		float32 angle = std::acos(dot);
		Vector3<float32> axis = referenceForward.Cross(forwardVector).Normalized();
		return FromAxisAngle(axis, angle);
	}

    Vector3<float32> Quaternion::RotateVector(const Vector3<float32>& vec) const
	{
		Vector3<float32> u(x, y, z);
		float32 uvD = u.Dot(vec);
		Vector3<float32> uvC = u.Cross(vec);
		return 2.f * (uvD * u + w * uvC) + (w * w - u.LengthSquared()) * vec;
	}

	Quaternion Quaternion::Inverse() const
	{
		float32 lenSq = LengthSquared();
		if (std::abs(lenSq - 1) < ME::Core::Math::Epsilon)
		    return Conjugate();
	    float32 invLenSq = 1.0f / lenSq;
	    return Quaternion(w * invLenSq, -x * invLenSq, -y * invLenSq, -z * invLenSq);
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

    float32 Quaternion::Dot(const Quaternion& other) const
    {
        return x * other.x + y * other.y + z * other.z + w * other.w;
    }

    float32 Quaternion::Length() const
	{
		return sqrt(LengthSquared());
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

    Quaternion Quaternion::operator/(float32 number) const
    {
		Quaternion result;
		result.x = x / number;
		result.y = y / number;
		result.z = z / number;
		result.w = w / number;
		return result;
    }

    Quaternion Quaternion::operator/(const Quaternion& other) const
    {
		Quaternion result;
		result.x = x / other.x;
		result.y = y / other.y;
		result.z = z / other.z;
		result.w = w / other.w;
		return result;
    }

    Quaternion& Quaternion::operator/=(float32 number)
    {
		x /= number;
		y /= number;
		z /= number;
		w /= number;
		return *this;
    }

    Quaternion& Quaternion::operator/=(const Quaternion& other)
    {
		x /= other.x;
		y /= other.y;
		z /= other.z;
		w /= other.w;
		return *this;
    }

    Quaternion Quaternion::operator+(const Quaternion& other) const
    {
		Quaternion result;
		result.x = x + other.x;
		result.y = y + other.y;
		result.z = z + other.z;
		result.w = w + other.w;
		return result;
    }

    Quaternion& Quaternion::operator+=(const Quaternion& other)
    {
		x += other.x;
		y += other.y;
		z += other.z;
		w += other.w;
		return *this;
    }

    Quaternion Quaternion::operator-(const Quaternion& other) const
    {
		Quaternion result;
		result.x = x - other.x;
		result.y = y - other.y;
		result.z = z - other.z;
		result.w = w - other.w;
        return result;
    }

    Quaternion& Quaternion::operator-=(const Quaternion& other)
    {
		x -= other.x;
		y -= other.y;
		z -= other.z;
		w -= other.w;
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
		return abs(w - other.w) < epsilon &&
			abs(x - other.x) < epsilon &&
			abs(y - other.y) < epsilon &&
			abs(z - other.z) < epsilon;
	}

	Quaternion::Quaternion(float32 _w, float32 _x, float32 _y, float32 _z) : w(_w), x(_x), y(_y), z(_z) {}

	Quaternion::Quaternion(const Vector4<float32>& val) : w(val.w), x(val.x), y(val.y), z(val.z) {}

	Quaternion::Quaternion(float32 scalar, const Vector3<float32>& axis, bool degrees)
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

		Normalize();
	}

	Quaternion::Quaternion() : w(1.0f), x(0.0f), y(0.0f), z(0.0f) {}
}