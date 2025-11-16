#pragma once
#include "ECS/Managers/ComponentManager.hpp"
#include "Renderer/Assets/Mesh.hpp"

namespace ME::Components
{
    struct MEAPI MeshComponent : ECS::Components::Component
    {
        ME_COMPONENT_INIT(MeshComponent) {}
        ~MeshComponent() override = default;

        bool Visible = false;
        bool ShadowsVisible = true;
        float32 Opacity = 1.f;
        ME::Core::Memory::Reference<ME::Assets::Mesh> Mesh = nullptr;
    };
}
