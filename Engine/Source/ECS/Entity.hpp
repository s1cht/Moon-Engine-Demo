#pragma once

#include <Core.hpp>
#include <Core/Utils/Logging/Logger.hpp>
#include "Component.hpp"
#include "ComponentManager.hpp"
#include "Components/MeshComponent.hpp"
#include "Components/TransformComponent.hpp"

namespace ME::ECS
{
    class MEAPI Entity
    {
    public:
        explicit Entity(uint64 id);

        Entity() = delete;
        Entity(const Entity&) = delete;
        Entity(Entity&&) = delete;
        Entity& operator=(const Entity&) = delete;
        Entity& operator=(Entity&&) noexcept = delete;

    public:
        bool operator<(const Entity& other) const noexcept;

    public:
        template<typename T>
        void AddComponent(ME::Core::Memory::Reference<Components::Component> component) const
        {
            if constexpr (std::is_same_v<T, Components::MeshComponent>)
            {
                if (!HasComponent<Components::TransformComponent>())
                    AddComponent<Components::TransformComponent>(ME::Core::Memory::MakeReference<Components::Component>(Components::TransformComponent()));
            }
            ComponentManager::Get().AttachComponent<T>(m_EntityId, component);
        }

        template<typename T>
        ME_NODISCARD T& GetComponent() const
        {
            return ComponentManager::Get().GetComponent<T>(m_EntityId);
        }

        template<typename T>
        ME_NODISCARD bool HasComponent() const
        {
            return ComponentManager::Get().HasComponent<T>(m_EntityId);
        }

        template<typename T>
        inline void RemoveComponent() const
        {
            ComponentManager::Get().RemoveComponent<T>(m_EntityId);
        }

        ME_NODISCARD uint64 GetID() const { return m_EntityId; }

    public:
        ME_NODISCARD static Entity Create();

    private:
        uint64 m_EntityId;
    };
}
