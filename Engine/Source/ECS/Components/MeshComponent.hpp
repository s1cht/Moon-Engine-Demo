#pragma once
#include "ECS/ComponentManager.hpp"
#include "Renderer/Assets/Mesh.hpp"

namespace ME::ECS::Components
{
    struct MEAPI MeshComponent : Component
    {
        MeshComponent() : Component(ComponentManager::GetComponentType<MeshComponent>()), Mesh(nullptr) {}
        ~MeshComponent() override = default;

        bool Visible = false;
        ME::Core::Memory::Reference<ME::Assets::Mesh> Mesh;
    };
}
