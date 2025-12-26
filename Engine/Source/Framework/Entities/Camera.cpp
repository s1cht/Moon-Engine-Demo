#include "Camera.hpp"

namespace ME
{
    void EditorCamera::OnUpdate(float32 deltaTime)
    {
        if (Input::InputController::IsMouseRightButtonDown())
            RotateCamera(deltaTime); 
        MoveCamera(deltaTime);
        CalculateTransform();
    }

    inline void EditorCamera::Update() 
    {
    }

    inline bool EditorCamera::BufferUpdateRequired() const 
    { 
        return !m_BufferUpdated; 
    }

    inline void EditorCamera::BufferUpdated() 
    { 
        m_BufferUpdated = true; 
    }
    
    inline void EditorCamera::SetRotation(const ME::Core::Math::Quaternion& rotation) 
    {
        ME::Core::Math::Vector3D eulerRotation = rotation.ToEulerAnglesYXZ();
    
        m_Pitch = ME::Core::Math::ToDegrees(eulerRotation.x);
        m_Yaw = ME::Core::Math::ToDegrees(eulerRotation.y);
        m_Roll = ME::Core::Math::ToDegrees(eulerRotation.z);
        m_ViewDirty = true;
    }

    const ME::Core::Math::Vector3D EditorCamera::GetRotation() const
    {
        return ME::Core::Math::Vector3D(ME::Core::Math::ToRadians(m_Pitch), ME::Core::Math::ToRadians(m_Yaw), ME::Core::Math::ToRadians(m_Roll));
    }

    inline const ME::Core::Math::Quaternion EditorCamera::GetQuaternion() const
    { 
        ME::Core::Math::Quaternion qYaw = ME::Core::Math::Quaternion::FromAxisAngle(ME::Core::Math::Vector3D::YAxis, ME::Core::Math::ToRadians(m_Yaw));
        ME::Core::Math::Quaternion qPitch = ME::Core::Math::Quaternion::FromAxisAngle(qYaw.Conjugate().RotateVector(ME::Core::Math::Vector3D::XAxis), ME::Core::Math::ToRadians(m_Pitch));
        return (qYaw * qPitch).Normalized();
    }

    void EditorCamera::MoveCamera(float32 deltaTime)
    {
        ME::Core::Math::Quaternion rotation = GetQuaternion().Conjugate();
        const ME::Core::Math::Vector3D& lookVector = rotation.RotateVector(ME::Core::Math::Vector3D::ForwardVector);
        const ME::Core::Math::Vector3D& rightVector = rotation.RotateVector(ME::Core::Math::Vector3D::RightVector);
        const ME::Core::Math::Vector3D& upVector = rotation.RotateVector(ME::Core::Math::Vector3D::UpVector);

        float32 speedMultiplier = m_CameraSpeed * deltaTime;

        if (Input::InputController::IsKeyDown(Input::Keycode::ME_KEY_W))
        {
            m_Position += lookVector * speedMultiplier;
            m_ViewDirty = true;
        }
        if (Input::InputController::IsKeyDown(Input::Keycode::ME_KEY_S))
        {
            m_Position -= lookVector * speedMultiplier;
            m_ViewDirty = true;
        }
        if (Input::InputController::IsKeyDown(Input::Keycode::ME_KEY_D))
        {
            m_Position += rightVector * speedMultiplier;
            m_ViewDirty = true;
        }
        if (Input::InputController::IsKeyDown(Input::Keycode::ME_KEY_A))
        {
            m_Position -= rightVector * speedMultiplier;
            m_ViewDirty = true;
        }
        if (Input::InputController::IsKeyDown(Input::Keycode::ME_KEY_SPACE))
        {
            m_Position += upVector * speedMultiplier;
            m_ViewDirty = true;
        }
        if (Input::InputController::IsKeyDown(Input::Keycode::ME_KEY_LEFTCONTROL))
        {
            m_Position -= upVector * speedMultiplier;
            m_ViewDirty = true;
        }
    }
    
    void EditorCamera::RotateCamera(float32 deltaTime)
    {
        Core::Math::Vector2D mouseDelta = Input::InputController::GetMouseDelta();
        m_ViewDirty = true;

        m_Yaw -= mouseDelta.X * m_CameraSensitivity * deltaTime;
        m_Yaw = fmod(m_Yaw, 360.0f);
        if (m_Yaw < 0) m_Yaw += 360.0f;

        m_Pitch += mouseDelta.Y * m_CameraSensitivity * deltaTime;
        m_Pitch = ME::Core::Math::Clamp(-89.f, 89.f, m_Pitch);
    }
    
    void EditorCamera::CalculateTransform()
    {
        if (m_ProjectionDirty)
        {
            m_Projection = ME::Core::Math::Matrix4x4::FromPerspectiveView(m_ProjectionInfo.FieldOfView, 
                m_ProjectionInfo.AspectRatio, 
                m_ProjectionInfo.Near, 
                m_ProjectionInfo.Far);
            m_ProjectionDirty = false;
            m_BufferUpdated = false;
        }
        if (!m_ViewDirty) return;

        m_View = ME::Core::Math::Matrix4x4::FromTranslation(-m_Position) * ME::Core::Math::Matrix4x4::FromQuaternion(GetQuaternion().Conjugate());

        m_ViewDirty = false;
        m_BufferUpdated = false;
    }
}
