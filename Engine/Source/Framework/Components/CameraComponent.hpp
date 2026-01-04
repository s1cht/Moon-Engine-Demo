#pragma once
#include "ECS/Managers/ComponentManager.hpp"
#include "Renderer/Camera.hpp"

namespace ME::Components
{
    struct MEAPI CameraComponent : ECS::Components::Component
    {
        ME_COMPONENT_CONSTRUCTOR(CameraComponent), Camera(nullptr) {}
        ~CameraComponent() override
        {
            
        };

        ME::Core::Memory::Reference<Render::Camera> Camera;
    };
}