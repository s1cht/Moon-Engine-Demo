#include "Transform.hpp"

namespace ME::Core::Math
{
    Transform::Transform()
        : m_Position(Vector3D::ZeroVector),
        m_Rotation(Quaternion::Identity),
        m_Scale(Vector3D(1.f)),
        m_Dirty(true)
    {
    }

}