#pragma once
#include "ECS/World.hpp"
#include "ECS/Entity.hpp"
#include "Framework/Components/MeshComponent.hpp"
#include "Framework/Components/TransformComponent.hpp"

namespace ME
{
    class MEAPI Block : ME_ENTITY_INHERIT(Block)
    {
        ME_ENT_DEFAULT(Block)

    public:
        ~Block() override = default;
        
    public:
        void OnCreated() override
        {
            AddComponent<Components::TransformComponent>();
            AddComponent<Components::MeshComponent>();
        }

        inline Components::TransformComponent& Transform() const { return GetComponent<Components::TransformComponent>(); }
        inline Components::MeshComponent& Mesh() const { return GetComponent<Components::MeshComponent>(); }

    public:
        inline void SetPosition(Core::Math::Vector3D position);
        inline void SetSize(Core::Math::Vector3D size);
        inline void SetOrientation(Core::Math::Vector3D orientation);
    };
}
