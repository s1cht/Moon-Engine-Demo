#pragma once
#include <Core.hpp>
#include <Core/Containers/Array.hpp>
#include <Core/Containers/Tables/UnorderedMap.hpp>
#include "ECS/Component.hpp"

namespace ME::ECS
{
    using ComponentType = uint64;

    class MEAPI ComponentManager
    {
    public:
        static ComponentManager& Get()
        {
            static ComponentManager instance;
            return instance;
        }

    public:
        template<typename T>
        void AttachComponent(uint64 entityId, ME::Core::Memory::Reference<Components::Component> component)
        {
            static_assert(std::is_base_of_v<Components::Component, T>, "Can't use non-component type in HasComponent() method!");
            ComponentType type = GetComponentType<T>();
            if (!m_Components.Contains(type))
                m_Components.Insert(type, ME::Core::Containers::UnorderedMap<uint64, ME::Core::Memory::Reference<Components::Component>>());
            m_Components[type].Insert(entityId, component);
        }

        template<typename T>
        bool HasComponent(uint64 entityId)
        {
            static_assert(std::is_base_of_v<Components::Component, T>, "Can't use non-component type in HasComponent() method!");
            ComponentType type = GetComponentType<T>();
            if (!m_Components.Contains(type))
                return false;
            if (!m_Components[type].Contains(entityId))
                return false;
            return true;
        }

        template<typename T>
        T& GetComponent(uint64 entityId)
        {
            static_assert(std::is_base_of_v<Components::Component, T>, "Can't use non-component type in GetComponent() method!");
            ME_ASSERT(HasComponent<T>(entityId), "Can't get a non-existing component!");
            ComponentType type = GetComponentType<T>();
            return *(static_cast<T*>(m_Components[type][entityId].get()));
        }

        template<typename T>
        void RemoveComponent(uint64 entityId)
        {
            static_assert(std::is_base_of_v<Components::Component, T>, "Can't use non-component type in GetComponent() method!");
            if (!HasComponent<T>(entityId))
                return;
            ComponentType type = GetComponentType<T>();
            m_Components[type].Erase(entityId);
        }

    public:
        template <typename T>
        static ComponentType GetComponentType()
        {
            static_assert(std::is_base_of_v<Components::Component, T>, "Can't use non-component type in GetComponentType() method!");
            static ComponentType typeID = GetNextComponentType();
            return typeID;
        }

        uint64 GetEntityID()
        {
            return m_FreeEntityId++;
        }

    private:
        static ComponentType GetNextComponentType()
        {
            static ComponentType compID = 0;
            return compID++;
        }

    private:
        ME::Core::Containers::UnorderedMap<uint64, ME::Core::Containers::UnorderedMap<uint64, ME::Core::Memory::Reference<Components::Component>>> m_Components;
        uint64 m_FreeEntityId = 0;
    };
}
