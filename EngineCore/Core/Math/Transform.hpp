#pragma once
#include "Core/Math/Math.hpp"

namespace ME::Core::Math
{
    class COREAPI Transform
    {
    public:
        Transform();

        Transform(const Vector3D& position)
            : m_Position(position),
            m_Rotation(Quaternion::Identity),
            m_Scale(Vector3D(1.f)),
            m_Dirty(true)
        {
        }

        Transform(const Transform&) = default;
        Transform(Transform&&) = default;

        Transform& operator=(const Transform&) = default;

    public:
        const Vector3D& Position() const { return m_Position; }
        const Quaternion& QRotation() const { return m_Rotation; }
        Vector3D Rotation() const { return m_Rotation.ToEulerAnglesXYZ(); }
        const Vector3D& Scale() const { return m_Scale; }

        const Matrix4x4& Matrix() const
        {
            if (m_Dirty)
                RecalculateMatrix();
            return m_CachedMatrix;
        }

        Vector3D LookVector() const { return Matrix().LookVector(); }
        Vector3D UpVector()   const { return Matrix().UpVector(); }
        Vector3D RightVector()const { return Matrix().RightVector(); }

        void SetPosition(const Vector3D& pos)
        {
            m_Position = pos;
            m_Dirty = true;
        }

        void SetRotation(const Quaternion& rot)
        {
            m_Rotation = rot.Normalized();
            m_Dirty = true;
        }

        void SetRotation(const Vector3D& eulerXYZ)
        {
            m_Rotation = Quaternion::FromEulerAnglesXYZ(eulerXYZ);
            m_Dirty = true;
        }

        void SetScale(const Vector3D& scale)
        {
            m_Scale = scale;
            m_Dirty = true;
        }

        Transform operator*(const Transform& o) const
        {
            Transform r;
            r.m_Rotation = (m_Rotation * o.m_Rotation).Normalized();
            r.m_Scale = m_Scale * o.m_Scale;
            r.m_Position = m_Position + (m_Rotation.RotateVector(m_Scale * o.m_Position));
            r.m_Dirty = true;
            return r;
        }

        Vector3D operator*(const Vector3D& v) const
        {
            return m_Rotation.RotateVector(v * m_Scale) + m_Position;
        }

        static Transform FromMatrix(const Matrix4x4& m)
        {
            Transform t;
            t.m_Position = m.GetTranslation();
            t.m_Rotation = m.GetQuaternion().Normalized();
            t.m_Scale = m.GetScale();
            t.m_Dirty = true;
            return t;
        }

        static Transform FromQuaternion(const Quaternion& rot)
        {
            Transform t;
            t.m_Position = Vector3D(0.f);
            t.m_Rotation = rot.Normalized();
            t.m_Scale = Vector3D(1.f);
            t.m_Dirty = true;
            return t;
        }

        static Transform LookAt(const Vector3D& pos, const Vector3D& target, const Vector3D& up)
        {
            Transform t;
            t.m_Position = pos;
            t.m_Rotation = Quaternion::LookAt(pos, target).Normalized();
            t.m_Scale = Vector3D(1.f);
            t.m_Dirty = true;
            return t;
        }

        Transform Inverse() const
        {
            Transform inv;
            inv.m_Scale = Vector3D(1.f) / m_Scale;
            inv.m_Rotation = m_Rotation.Conjugate();
            inv.m_Position = inv.m_Rotation.RotateVector(-m_Position) * inv.m_Scale;
            inv.m_Dirty = true;
            return inv;
        }

    private:
        void RecalculateMatrix() const
        {
            m_CachedMatrix =
                Matrix4x4::FromTranslation(m_Position) *
                Matrix4x4::FromQuaternion(m_Rotation) *
            Matrix4x4::FromScale(m_Scale);

            m_Dirty = false;
        }

    private:
        Vector3D m_Position;
        Quaternion m_Rotation;
        Vector3D m_Scale;

        mutable Matrix4x4 m_CachedMatrix;
        mutable bool m_Dirty;
    };
}
