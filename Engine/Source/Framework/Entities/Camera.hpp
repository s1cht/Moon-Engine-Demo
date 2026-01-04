#pragma once
#include "ECS/World.hpp"
#include "ECS/Entity.hpp"
#include "Input/Input.hpp"
#include "Renderer/Camera.hpp"

namespace ME
{
    class MEAPI EditorCamera : ME_ENTITY_INHERIT(), public ME::Render::Camera
    {
        ME_ENTITY_DEFAULT(EditorCamera)
    public:
        void OnCreated() override 
        {
            m_Yaw = 0.f;
            m_Pitch = 0.f;
        }

        void OnUpdate(float32 deltaTime) override;

    public:
        void Update() override;
        bool BufferUpdateRequired() const override;
        void BufferUpdated() override;

        void SetPosition(const ME::Core::Math::Vector3D& position) override { m_Position = position; m_ViewDirty = true; }
        void SetRotation(const ME::Core::Math::Vector3D& rotation) override { m_Pitch = rotation.x; m_Yaw = rotation.y; m_Roll = rotation.z; m_ViewDirty = true; }
        void SetRotation(float32 pitch, float32 yaw, float32 roll) override { m_Pitch = pitch; m_Yaw = yaw; m_Roll = roll; m_ViewDirty = true; }
        void SetRotation(const ME::Core::Math::Quaternion& rotation) override;

        const ME::Core::Math::Vector3D GetRotation() const override;
        const ME::Core::Math::Quaternion GetQuaternion() const override;
        const ME::Core::Math::Vector3D& GetPosition() const override { return m_Position; }

        const ME::Core::Math::Matrix4x4& GetView() const override { return m_View; }
        const ME::Core::Math::Matrix4x4& GetProjection() const override { return m_Projection; }

    public:
        void SetFieldOfView(uint32 fieldOfView) { m_ProjectionInfo.FieldOfView = static_cast<float32>(fieldOfView); m_ProjectionDirty = true; }
        void SetAspectRatio(float32 aspectRatio) { m_ProjectionInfo.AspectRatio = aspectRatio; m_ProjectionDirty = true; }
        void SetNear(float32 near) { m_ProjectionInfo.Near = near; m_ProjectionDirty = true; }
        void SetFar(float32 far) { m_ProjectionInfo.Far = far; m_ProjectionDirty = true; }
        void SetPerspectiveProjectionInfo(ME::Render::PerspectiveProjectionInfo info) { m_ProjectionInfo = info; m_ProjectionDirty = true; }
        ME::Render::PerspectiveProjectionInfo GetProjectionInfo() const { return m_ProjectionInfo; }

        void SetCameraSpeed(float32 speed) { m_CameraSpeed = speed; }
        float32 GetCameraSpeed() const { return m_CameraSpeed; }
        void SetCameraSensitivity(float32 sens) { m_CameraSensitivity = sens; }
        float32 GetCameraSensitivity() const { return m_CameraSensitivity; }

    private:
        void MoveCamera(float32 deltaTime);
        void RotateCamera(float32 deltaTime);
        void CalculateTransform();

    private:
        bool m_ProjectionDirty;
        bool m_ViewDirty;
        bool m_BufferUpdated;

    private:
        // Camera settings
        Render::PerspectiveProjectionInfo m_ProjectionInfo;

        float32 m_CameraSpeed = 1.f;
        float32 m_CameraSensitivity = 5.f;
    private:
        // Camera data for transformation
        float32 m_Yaw;
        float32 m_Pitch;
        float32 m_Roll;

        ME::Core::Math::Vector3D m_Position;

        ME::Core::Math::Matrix4x4 m_Projection;
        ME::Core::Math::Matrix4x4 m_View;
    };
}