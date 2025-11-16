#pragma once

#include "ECS/World.hpp"
#include "Renderer/Camera/PerspectiveCamera.hpp"
#include "Framework/Components/CameraComponent.hpp"
#include "Input/Input.hpp"

namespace ME
{
    class PerspectiveCamera : public ECS::Entity
    {
        ME_ENT_DEFAULT(PerspectiveCamera)
    public:
        void OnCreated() override
        {
            AddComponent<Components::CameraComponent>();
        }

        void OnUpdate(float32 deltaTime) override
        {
            float32 speedMultiplier = 1.f * deltaTime;
            Components::CameraComponent& camera = GetComponent<Components::CameraComponent>();

            if (Input::InputController::IsKeyDown(Input::Keycode::ME_KEY_LEFTSHIFT))
                speedMultiplier = 0.5f * deltaTime;
            if (Input::InputController::IsKeyDown(Input::Keycode::ME_KEY_W))
                camera.Camera->SetPosition(
                        camera.Camera->GetTransform().Position() + camera.Camera->GetTransform().LookVector() * (m_CameraSpeed * speedMultiplier));
            if (Input::InputController::IsKeyDown(Input::Keycode::ME_KEY_S))
                camera.Camera->SetPosition(
                        camera.Camera->GetTransform().Position() - camera.Camera->GetTransform().LookVector() * (m_CameraSpeed * speedMultiplier));
            if (Input::InputController::IsKeyDown(Input::Keycode::ME_KEY_D))
                camera.Camera->SetPosition(
                        camera.Camera->GetTransform().Position() + camera.Camera->GetTransform().RightVector() * (m_CameraSpeed * speedMultiplier));
            if (Input::InputController::IsKeyDown(Input::Keycode::ME_KEY_A))
                camera.Camera->SetPosition(
                        camera.Camera->GetTransform().Position() - camera.Camera->GetTransform().RightVector() * (m_CameraSpeed * speedMultiplier));
            if (Input::InputController::IsKeyDown(Input::Keycode::ME_KEY_SPACE))
                camera.Camera->SetPosition(
                        camera.Camera->GetTransform().Position() + camera.Camera->GetTransform().UpVector() * (m_CameraSpeed * speedMultiplier));
            if (Input::InputController::IsKeyDown(Input::Keycode::ME_KEY_LEFTCONTROL))
                camera.Camera->SetPosition(
                        camera.Camera->GetTransform().Position() - camera.Camera->GetTransform().UpVector() * (m_CameraSpeed * speedMultiplier));
            if (Input::InputController::IsMouseRightButtonDown())
            {
                ME::Core::Math::Vector2D mouseDelta = Input::InputController::GetMouseDelta();
                static float32 yaw, pitch = 0.f;
                yaw -= mouseDelta.X * m_CameraSensitivity * deltaTime;
                pitch += mouseDelta.Y * m_CameraSensitivity * deltaTime;

                pitch = std::clamp(pitch, -89.f, 89.f);
                if (yaw > 360.0f) yaw -= 360.f;
                if (yaw < 0.0f) yaw += 360.f;
                camera.Camera->SetRotation(ME::Core::Math::ToRadians(yaw), ME::Core::Math::ToRadians(pitch), 0.f);
            }
            camera.Camera->Update();
        }

    public:
        void SetCameraSpeed(float32 speed) { m_CameraSpeed = speed; }
        float32 GetCameraSpeed() const { return m_CameraSpeed; }
        void SetCameraSensitivity(float32 sens) { m_CameraSensitivity = sens; }
        float32 GetCameraSensitivity() const { return m_CameraSensitivity; }

        Components::CameraComponent& Camera() const { return GetComponent<Components::CameraComponent>(); }

    private:
        float32 m_CameraSpeed = 1.f;
        float32 m_CameraSensitivity = 1.f;
    };
}
