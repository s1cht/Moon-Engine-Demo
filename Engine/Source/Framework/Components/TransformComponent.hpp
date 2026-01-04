#pragma once
#include "ECS/Component.hpp"
#include "ECS/Managers/ComponentManager.hpp"

#include <Core/Math/Transform.hpp>

namespace ME::Components
{
    struct MEAPI TransformComponent : ECS::Components::Component
    {
        ME_COMPONENT_CONSTRUCTOR(TransformComponent),
            Transform(Core::Math::Transform()) {}
        ~TransformComponent() override = default;

        Core::Math::Transform Transform;
    };
}