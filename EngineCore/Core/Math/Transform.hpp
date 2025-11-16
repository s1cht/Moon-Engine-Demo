#pragma once

#include <Core/Math/Math.hpp>

namespace ME::Core::Math
{
    class Transform
    {
    public:
        Transform() : m_Position(Vector3D::ZeroVector), m_Rotation(Vector3D::ZeroVector), m_Scale(Vector3D(1)), m_Matrix(Matrix4x4()) {}
        Transform(const Vector3D& position) : m_Position(position), m_Rotation(Vector3D::ZeroVector), m_Scale(Vector3D(1)), m_Matrix(Matrix4x4())
        {
            m_Matrix.a14 = position.X;
            m_Matrix.a24 = position.Y;
            m_Matrix.a34 = position.Z;
        }
        Transform(const Transform&) = default;
        Transform(Transform&&) = default;
        ~Transform() = default;

    public:
        Transform& operator=(const Transform&) = default;

    public:
        const Vector3D& Position() const { return m_Position; }
        const Vector3D& Rotation() const { return m_Rotation; }
        const Vector3D& Scale() const { return m_Scale; }
        const Matrix4x4& Matrix() const { return m_Matrix; }

        Vector3D LookVector() const { return m_Matrix.LookVector(); }
        Vector3D UpVector() const { return m_Matrix.UpVector(); }
        Vector3D RightVector() const { return m_Matrix.RightVector(); }

    public:
        Transform Inverse() const
        {
            Transform result;
            result.m_Matrix = m_Matrix.Invert();
            result.m_Position = result.m_Matrix.GetTranslation();
            result.m_Scale = result.m_Matrix.GetScale();
            result.m_Rotation = result.m_Matrix.GetEulerAnglesXYZ();
            return result;
        }
        Transform Lerp(const Transform& goal, float32 alpha) const
        {
            Transform result = *this;
            result.m_Position = m_Position.Lerp(goal.m_Position, alpha);
            result.m_Rotation = Quaternion::Slerp(
                Quaternion::FromEulerAnglesXYZ(m_Rotation),
                Quaternion::FromEulerAnglesXYZ(goal.m_Rotation),
                alpha
            ).ToEulerAnglesXYZ();
            result.m_Scale = m_Scale.Lerp(goal.m_Scale, alpha);
            result.m_Matrix = Matrix4x4::FromTranslation(m_Position) * Matrix4x4::FromScale(m_Scale) * Matrix4x4::FromQuaternion(Quaternion::FromEulerAnglesXYZ(m_Rotation));
            return result;
        }

    public:
        inline Transform operator*(const Transform& other) const
        {
            return FromMatrix(m_Matrix * other.m_Matrix);
        }

        inline Transform& operator*=(const Transform& other)
        {
            *this = *this * other;
            return *this;
        }

        inline Vector3D operator*(const Vector3D& other) const
        {
            return m_Position * other;
        }

        inline Transform operator+(const Vector3D& other) const
        {
            Transform result = *this;
            result.m_Position += other;
            result.m_Matrix.a14 = result.m_Position.X;
            result.m_Matrix.a24 = result.m_Position.Y;
            result.m_Matrix.a34 = result.m_Position.Z;
            return result;
        }

        inline Transform& operator+=(const Vector3D& other)
        {
            *this = *this + other;
            return *this;
        }

        inline Transform operator-(const Vector3D& other) const
        {
            Transform result = *this;
            result.m_Position += other;
            result.m_Matrix.a14 = result.m_Position.X;
            result.m_Matrix.a24 = result.m_Position.Y;
            result.m_Matrix.a34 = result.m_Position.Z;
            return result;
        }

        inline Transform& operator-(const Vector3D& other)
        {
            *this = *this + other;
            return *this;
        }

    public:
        static Transform FromMatrix(const Matrix4x4& matrix)
        {
            Transform result;

            result.m_Position = matrix.GetTranslation();
            result.m_Scale = matrix.GetScale();
            result.m_Rotation = matrix.GetEulerAnglesXYZ();
            result.m_Matrix = matrix;

            return result;
        }

        static Transform FromScale(const Vector3D& scale)
        {
            Transform result;

            result.m_Position = Vector3D::ZeroVector;
            result.m_Rotation = Vector3D::ZeroVector;
            result.m_Scale = scale;
            result.m_Matrix = Matrix4x4::FromScale(scale);

            return result;
        }

        static Transform FromAngles(const Vector3D& angles)
        {
            Transform result;

            result.m_Position = Vector3D::ZeroVector;
            result.m_Rotation = angles;
            result.m_Scale = Vector3D(1);
            result.m_Matrix = Matrix4x4::FromQuaternion(Quaternion::FromEulerAnglesXYZ(angles));

            return result;
        }

        static Transform FromEulerAnglesXYZ(const Vector3D& angles)
        {
            Transform result;

            result.m_Position = Vector3D::ZeroVector;
            result.m_Rotation = angles;
            result.m_Scale = Vector3D(1);
            result.m_Matrix = Matrix4x4::FromQuaternion(Quaternion::FromEulerAnglesXYZ(angles));

            return result;
        }

        static Transform FromEulerAnglesYXZ(const Vector3D& angles)
        {
            Transform result;

            result.m_Matrix = Matrix4x4::FromQuaternion(Quaternion::FromEulerAnglesYXZ(angles));
            result.m_Position = Vector3D::ZeroVector;
            result.m_Rotation = result.m_Matrix.GetEulerAnglesXYZ();
            result.m_Scale = Vector3D(1);

            return result;
        }

        static Transform FromQuaternion(const Quaternion& angles)
        {
            Transform result;

            result.m_Position = Vector3D::ZeroVector;
            result.m_Rotation = Vector3D(angles.y, angles.x, angles.z);
            result.m_Scale = Vector3D(1);
            result.m_Matrix = Matrix4x4::FromQuaternion(angles);

            return result;
        }

    private:
        Vector3D m_Position;
        Vector3D m_Rotation;
        Vector3D m_Scale;

        Matrix4x4 m_Matrix;

    };
}
