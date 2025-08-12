#pragma once

#include "Core.hpp"
#include "Core/Math/Vector3.hpp"
#include "Core/Math/Vector4.hpp"

namespace ME::Core::Math
{
	struct CORE_API PMatrix4x4;

	struct CORE_API PQuaternion
	{
	public:
		union
		{
			struct { float32 w, x, y, z; };
			struct { float32 W, X, Y, Z; };
		};

	public:
		static const PQuaternion Identity;

	public:
		inline PQuaternion();
		inline PQuaternion(float32 scalar, const Vector3<float32>& axis, bool degrees = false);
		inline PQuaternion(const Vector4<float32>& val);
		inline PQuaternion(float32 _w, float32 _x, float32 _y, float32 _z);

		~PQuaternion() = default;
	public:
		bool operator==(const PQuaternion& other) const;

		PQuaternion& operator=(const PQuaternion& other);

		PQuaternion operator*(float32 number) const;
		PQuaternion operator*(const PQuaternion& other) const;

		PQuaternion& operator*=(const PQuaternion& other);
		PQuaternion& operator*=(float32 number);

	public:
		float32 Length() const;
		float32 LengthSquared() const;

		PQuaternion& Normalize();
		PQuaternion Normalized() const;

		PQuaternion Conjugate() const;
		PQuaternion Inverse() const;

		Vector3<float32> RotateVector(const Vector3<float32>& vec) const;

		PMatrix4x4 ToMatrix() const;

	public:
		static PQuaternion FromEulerAngles(float32 yaw, float32 pitch, float32 roll);
		static PQuaternion FromEulerAnglesXYZ(const Vector3<float32>& angles);
		static PQuaternion FromEulerAnglesYXZ(const Vector3<float32>& angles);

		static PQuaternion FromAxisAngle(const Vector3<float32>& axis, float32 angle);

		static PQuaternion FromRadians(const Vector3<float32>& radians);
		static PQuaternion FromRadians(float32 _x, float32 _y, float32 _z);

	};

	PQuaternion operator*(float32 number, const PQuaternion& other);

	inline PQuaternion operator*(float32 number, const PQuaternion& other)
	{
		return other * number;
	}
}