#pragma once
#include "ECS/Component.hpp"
#include "ECS/ComponentManager.hpp"

#include <Core/Math/Transform.hpp>

namespace ME::ECS::Components
{
    struct MEAPI TransformComponent : Component
    {
        TransformComponent() : Component(ME::ECS::ComponentManager::GetComponentType<TransformComponent>()),
            Transform(Core::Math::Transform()) {}
        ~TransformComponent() override = default;

        Core::Math::Transform Transform;
    };
}