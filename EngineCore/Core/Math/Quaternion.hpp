#pragma once
#include "MathCore.hpp"

namespace ME::Core::Math
{
	struct COREAPI Quaternion
	{
	public:
		union
		{
			struct { float32 w, x, y, z; };
			struct { float32 W, X, Y, Z; };
		};

	public:
		static const Quaternion Identity;

	public:
		inline Quaternion();
		inline Quaternion(float32 scalar, const Vector3<float32>& axis, bool degrees = false);
		inline Quaternion(const Vector4<float32>& val);
		inline Quaternion(float32 _w, float32 _x, float32 _y, float32 _z);

		~Quaternion() = default;

	public:
		bool operator==(const Quaternion& other) const;

		Quaternion& operator=(const Quaternion& other);

		Quaternion operator*(float32 number) const;
		Quaternion operator*(const Quaternion& other) const;

		Quaternion& operator*=(float32 number);
		Quaternion& operator*=(const Quaternion& other);

	    Quaternion operator/(float32 number) const;
		Quaternion operator/(const Quaternion& other) const;
		Quaternion& operator/=(float32 number);
		Quaternion& operator/=(const Quaternion& other);

		Quaternion operator+(const Quaternion& other) const;
		Quaternion& operator+=(const Quaternion& other);

	    Quaternion operator-(const Quaternion& other) const;
		Quaternion& operator-=(const Quaternion& other);

	public:
		float32 Length() const;
		float32 LengthSquared() const;

		float32 Dot(const Quaternion& other) const;

	    Quaternion Slerp(const Quaternion& q2, float32 alpha);

	    Quaternion& Normalize();
		Quaternion Normalized() const;

		Quaternion Conjugate() const;
		Quaternion Inverse() const;

		Vector3<float32> RotateVector(const Vector3<float32>& vec) const;

		Matrix4x4 ToMatrix() const;
		Vector3<float32> ToEulerAnglesXYZ() const;
		Vector3<float32> ToEulerAnglesYXZ() const;

	public:
		static Quaternion LookAt(const Vector3<float32>& position, const Vector3<float32>& lookAtPosition);

		static Quaternion FromEulerAnglesXYZ(const Vector3<float32>& angles);
		static Quaternion FromEulerAnglesXYZ(float32 yaw, float32 pitch, float32 roll);
		static Quaternion FromEulerAnglesYXZ(const Vector3<float32>& angles);
		static Quaternion FromEulerAnglesYXZ(float32 yaw, float32 pitch, float32 roll);
		static Quaternion FromEulerAnglesZYX(const Vector3<float32>& angles);
		static Quaternion FromEulerAnglesZYX(float32 yaw, float32 pitch, float32 roll);

		static Quaternion FromAxisAngle(const Vector3<float32>& axis, float32 angle);
	};

	Quaternion operator*(float32 number, const Quaternion& other);

	inline Quaternion operator*(float32 number, const Quaternion& other)
	{
		return other * number;
	}
}