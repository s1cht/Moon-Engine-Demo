#pragma once

#include <Core.hpp>

namespace ME::ECS::Components
{
    struct MEAPI Component
    {
        Component(uint64 componentType) : m_ComponentType(componentType) {}
        virtual ~Component() {};

        ME_NODISCARD uint64 GetComponentType() const { return m_ComponentType; }

    protected:
        uint64 m_ComponentType;
    };
}
