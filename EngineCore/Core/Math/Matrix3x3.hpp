#pragma once

#include "Core.hpp"

namespace Pawn::Core::Math
{
	struct PQuaternion;

	struct CORE_API PMatrix3x3
	{
	public:
		union
		{
			float32 Matrix[3][3];
			float32 a[3][3];
			struct
			{
				float32 a11, a12, a13,
					a21, a22, a23,
					a31, a32, a33;
			};
		};

	public:
		inline PMatrix3x3();

		inline PMatrix3x3(float32 scalar) noexcept;

		inline PMatrix3x3(
			const float32& a11, const float32& a12, const float32& a13,
			const float32& a21, const float32& a22, const float32& a23,
			const float32& a31, const float32& a32, const float32& a33);

		~PMatrix3x3() = default;

	public:
		PMatrix3x3 operator*(const float32& scalar) const;

		PMatrix3x3& operator*=(const float32& scalar);

		PMatrix3x3& operator*=(const int32& scalar);

		PMatrix3x3 operator*(const PMatrix3x3& other) const;

		PMatrix3x3 operator+(const PMatrix3x3& other) const;

		PMatrix3x3& operator+=(const PMatrix3x3& other);

		PMatrix3x3 operator-(const PMatrix3x3& other) const;

		PMatrix3x3& operator-=(const PMatrix3x3& other);

		bool operator==(const PMatrix3x3& other) const;

	public:
		float32 Determinant() const;

		PMatrix3x3 Invert() const;

		PMatrix3x3 Adjoint() const;

		PMatrix3x3 Abs() const;

		PMatrix3x3 Transpose() const;

		//static Matrix3x3 FromQuaternion(const Quaternion& q);
	};

	inline PMatrix3x3 operator*(const float32& scalar, const PMatrix3x3& mat);

}