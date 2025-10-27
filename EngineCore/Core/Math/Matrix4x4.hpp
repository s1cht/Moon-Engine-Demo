#pragma once

#include "Core.hpp"
#include "Core/Math/Vector2.hpp"
#include "Core/Math/Vector3.hpp"

namespace ME::Core::Math
{
	struct PMatrix3x3;

	template<typename T>
	struct Vector3;

	struct PQuaternion;

	struct COREAPI PMatrix4x4
	{
	public:
		union
		{
			float32 Matrix[4][4];
			float32 a[4][4];
			struct
			{
				float32 a11, a12, a13, a14,
					a21, a22, a23, a24,
					a31, a32, a33, a34,
					a41, a42, a43, a44;
			};
		};

	public:
		inline PMatrix4x4();
		inline explicit PMatrix4x4(float32 scalar) noexcept;
		inline explicit PMatrix4x4(
			const float32& a11, const float32& a12, const float32& a13, const float32& a14,
			const float32& a21, const float32& a22, const float32& a23, const float32& a24,
			const float32& a31, const float32& a32, const float32& a33, const float32& a34,
			const float32& a41, const float32& a42, const float32& a43, const float32& a44);
		PMatrix4x4(const PMatrix4x4&) = default;
		PMatrix4x4(PMatrix4x4&&) = default;
		~PMatrix4x4() = default;

	public:
		PMatrix4x4 operator*(const float32& scalar) const;
		PMatrix4x4 operator*(const PMatrix4x4& other) const;
		PMatrix4x4& operator*=(const int32& scalar);
		PMatrix4x4& operator*=(const float32& scalar);
		PMatrix4x4 operator+(const PMatrix4x4& other) const;
		PMatrix4x4& operator+=(const PMatrix4x4& other);
		PMatrix4x4 operator-(const PMatrix4x4& other) const;
		PMatrix4x4& operator-=(const PMatrix4x4& other);

		bool operator==(const PMatrix4x4& other) const;
        PMatrix4x4& operator=(const PMatrix4x4& other) = default;

	public:
		float32 Determinant() const;
		PMatrix4x4 Invert() const;
		PMatrix4x4 Adjoint() const;
		PMatrix4x4 Abs() const;
		PMatrix4x4 Transpose() const;
		Vector3<float32> Transform(const Vector3<float32>& axis) const;
		Vector3<float32> TransformNormal(const Vector3<float32>& axis) const;

	public:
		Vector3<float32> LookVector() const;
		Vector3<float32> UpVector() const;
		Vector3<float32> RightVector() const;


		Vector3<float32> GetTranslation() const;
		Vector3<float32> GetScale() const;
	    Vector3<float32> GetEulerAnglesXYZ() const;
		Vector3<float32> GetEulerAnglesYXZ() const;
		Vector3<float32> GetEulerAnglesZYX() const;

	public:
		static PMatrix4x4 FromQuaternion(const PQuaternion& q);
		static PMatrix4x4 FromTranslation(const Vector3<float32>& translation);
		static PMatrix4x4 FromScale(const Vector3<float32>& scale);
		static PMatrix4x4 FromPerspectiveView(float32 fov, float32 aspect, float32 _near, float32 _far);
		static PMatrix4x4 FromOrthographicView(float32 left, float32 right, float32 bottom, float32 top, float32 _near, float32 _far);
		static PMatrix4x4 LookAt(const Vector3<float32>& eye, const Vector3<float32>& target, const Vector3<float32>& up);

	};

	PMatrix4x4 operator*(const float32& scalar, const PMatrix4x4& mat);
}
