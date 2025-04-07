#pragma once

#include "Core.h"
#include "MathMacros.h"
#include <cmath>

namespace Pawn::Core::Math
{
	struct CORE_API Matrix4x4;

	template<typename T>
	struct Vector3;

	template<typename T>
	struct Vector4;

	struct CORE_API Quaternion
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

		Quaternion operator*(const Quaternion& other) const;

		Quaternion& operator*=(const Quaternion& other);

		Quaternion operator*(float32 number) const;

		Quaternion& operator*=(float32 number);

	public:
		float32 Length() const;

		float32 LengthSquared() const;

		Quaternion& Normalize();

		Quaternion Normalized() const;

		Quaternion Conjugate() const;

		Quaternion Inverse() const;

		Vector3<float32> RotateVector(const Vector3<float32>& vec) const;

		Matrix4x4 ToMatrix() const;

	public:
		static Quaternion FromEulerAngles(const Vector3<float32>& angles);

		static Quaternion FromEulerAngles(float32 yaw, float32 pitch, float32 roll);

		static Quaternion FromRadians(const Vector3<float32>& radians);

		static Quaternion FromRadians(float32 _x, float32 _y, float32 _z);

	};

	Quaternion operator*(float32 number, const Quaternion& other);

	inline Quaternion operator*(float32 number, const Quaternion& other)
	{
		return other * number;
	}
}