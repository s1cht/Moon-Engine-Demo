#pragma once
#include "MathCore.hpp"

namespace ME::Core::Math
{
	struct COREAPI Matrix4x4
	{
	public:
			union
			{
				float32 Matrix[4][4];
				float32 m[4][4];
				struct
				{
					float32 
						m11, m12, m13, m14,
						m21, m22, m23, m24,
						m31, m32, m33, m34,
						m41, m42, m43, m44;
				};
			};

	public:
		inline Matrix4x4();
		inline explicit Matrix4x4(float32 scalar) noexcept;
		inline explicit Matrix4x4(
			float32 m11, float32 m12, float32 m13, float32 m14,
			float32 m21, float32 m22, float32 m23, float32 m24,
			float32 m31, float32 m32, float32 m33, float32 m34,
			float32 m41, float32 m42, float32 m43, float32 m44);
		Matrix4x4(const Matrix4x4&) = default;
		Matrix4x4(Matrix4x4&&) = default;
		~Matrix4x4() = default;

	public:
		Matrix4x4 operator*(float32 scalar) const;
		Vector4<float32> operator*(const Vector4<float32>& other) const;
		Matrix4x4 operator*(const Matrix4x4& other) const;
		Matrix4x4& operator*=(const int32& scalar);
		Matrix4x4& operator*=(float32 scalar);
		Matrix4x4 operator*=(const Matrix4x4& other);
		Matrix4x4 operator+(const Matrix4x4& other) const;
		Matrix4x4& operator+=(const Matrix4x4& other);
		Matrix4x4 operator-(const Matrix4x4& other) const;
		Matrix4x4& operator-=(const Matrix4x4& other);

		bool operator==(const Matrix4x4& other) const;
        Matrix4x4& operator=(const Matrix4x4& other) = default;

	public:
		float32 Determinant() const;
		Matrix4x4 Invert() const;
		Matrix4x4 InvertAffine() const;
		Matrix4x4 Adjoint() const;
		Matrix4x4 Abs() const;
		Matrix4x4 Transpose() const;
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
		Quaternion GetQuaternion() const;

	public:
		static Matrix4x4 FromQuaternion(const Quaternion& q);
		static Matrix4x4 FromTranslation(const Vector3<float32>& translation);
		static Matrix4x4 FromScale(const Vector3<float32>& scale);
		static Matrix4x4 FromPerspectiveView(float32 fov, float32 aspect, float32 _near, float32 _far);
		static Matrix4x4 FromOrthographicView(float32 left, float32 right, float32 bottom, float32 top, float32 _near, float32 _far);
		static Matrix4x4 LookAt(const Vector3<float32>& eye, const Vector3<float32>& target, const Vector3<float32>& up);
	};

	Matrix4x4 operator*(float32 scalar, const Matrix4x4& matrix);
	Vector4<float32> operator*(const Vector4<float32>& vector, const Matrix4x4& matrix);
}
