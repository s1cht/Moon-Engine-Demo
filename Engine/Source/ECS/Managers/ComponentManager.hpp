#pragma once
#include <Core.hpp>
#include <Core/Containers/Array.hpp>
#include <Core/Containers/UnorderedMap.hpp>
#include <Core/Containers/UnorderedSet.hpp>
#include <Core/Memory/CompileTimeHasher.hpp>
#include <Core/TypeID.hpp>
#include "ECS/Component.hpp"
#include "ECSLimits.hpp"

#define ME_COMPONENT_INIT(className) className() : Component(ME::ECS::ComponentManager::GetComponentType<className>())

namespace ME::ECS
{
    using ComponentType = uint64;

    namespace Helper
    {
        using ComponentIDGenerator = Core::TypeIDGenerator<ComponentType, ME_MAX_COMPONENT_COUNT>;

        class IComponentArray
        {
        public:
            virtual ~IComponentArray() = default;
            virtual void OnEntityDestroyed(uint64 entityId) = 0;
            virtual bool Contains(uint64 entity) = 0;
            virtual ME::Core::Array<uint64> GetEntities() const = 0;
        };

        template<typename T>
        class ComponentArray : public IComponentArray
        {
        public:
            ComponentArray() : m_ComponentArray(ME_MAX_ENTITY_COUNT), m_Size(0)
            {

            }

            void InsertData(uint64 entity, T& component)
            {
                ME_ASSERT(m_EntityToIndex.Find(entity) == m_EntityToIndex.End(), "Component added to same entity more than once.");

                SIZE_T newIndex = m_Size;
                m_EntityToIndex[entity] = newIndex;
                m_IndexToEntity[newIndex] = entity;
                new (&m_ComponentArray[newIndex]) T();
                m_ComponentArray[newIndex] = std::move(component);
                ++m_Size;
            }

            void RemoveData(uint64 entity)
            {
                ME_ASSERT(m_EntityToIndex.Find(entity) != m_EntityToIndex.End(), "Removing non-existent component.");

                uint64 indexOfRemovedEntity = m_EntityToIndex[entity];
                uint64 indexOfLastElement = m_Size - 1;
                if (indexOfRemovedEntity != indexOfLastElement)
                {
                    m_ComponentArray[indexOfRemovedEntity] = std::move(m_ComponentArray[indexOfLastElement]);

                    uint64 entityOfLastElement = m_IndexToEntity[indexOfLastElement];
                    m_EntityToIndex[entityOfLastElement] = indexOfRemovedEntity;
                    m_IndexToEntity[indexOfRemovedEntity] = entityOfLastElement;
                }

                m_ComponentArray[indexOfRemovedEntity].~T();
                new (&m_ComponentArray[indexOfRemovedEntity]) T(std::move(m_ComponentArray[indexOfLastElement]));
                m_ComponentArray[indexOfLastElement].~T();

                m_EntityToIndex.Erase(entity);
                m_IndexToEntity.Erase(indexOfLastElement);

                --m_Size;
            }

            T* GetData(uint64 entity)
            {
                ME_ASSERT(m_EntityToIndex.Find(entity) != m_EntityToIndex.End(), "Retrieving non-existent component.");

                return &m_ComponentArray[m_EntityToIndex[entity]];
            }

            void OnEntityDestroyed(uint64 entity) override
            {
                if (m_EntityToIndex.Find(entity) != m_EntityToIndex.End())
                {
                    RemoveData(entity);
                }
            }

            bool Contains(uint64 entity) override
            {
                return m_EntityToIndex.Contains(entity);
            }

            ME::Core::Array<uint64> GetEntities() const override
            {
                ME::Core::Array<uint64> entities = {};
                for (const auto& pair : m_EntityToIndex)
                    entities.EmplaceBack(pair.Value1);
                return entities;
            }

        private:
            ME::Core::Array<T> m_ComponentArray;
            ME::Core::UnorderedMap<uint64, uint64> m_EntityToIndex;
            ME::Core::UnorderedMap<uint64, uint64> m_IndexToEntity;
            uint64 m_Size;
        };
    }

    class MEAPI ComponentManager
    {
    public:
        template<typename T>
        void RegisterComponent()
        {
            ComponentType type = GetComponentType<T>();
            if (m_RegisteredComponents.Find(type) != m_RegisteredComponents.End())
                ME_WARN("Registering component type more than once.");

            m_RegisteredComponents.Insert(type);

            m_ComponentArrays.Insert(type, ME::Core::Memory::MakeReference<Helper::ComponentArray<T>>());
        }

        template<typename T>
        void AttachComponent(uint64 entityId, T component)
        {
            static_assert(std::is_base_of_v<Components::Component, T>, "Can't use non-component type in HasComponent() method!");
            ComponentType type = GetComponentType<T>();
            GetComponentArray<T>()->InsertData(entityId, component);
        }

        template<typename T>
        void AddComponent(uint64 entityId)
        {
            static_assert(std::is_base_of_v<Components::Component, T>, "Can't use non-component type in HasComponent() method!");
            ComponentType type = GetComponentType<T>();
            T component = T();
            GetComponentArray<T>()->InsertData(entityId, component);
        }

        template<typename T>
        ME_NODISCARD bool HasComponent(uint64 entityId)
        {
            static_assert(std::is_base_of_v<Components::Component, T>, "Can't use non-component type in HasComponent() method!");
            ComponentType type = GetComponentType<T>();
            if (!m_RegisteredComponents.Contains(type))
                return false;
            if (!GetComponentArray<T>()->Contains(entityId))
                return false;
            return true;
        }

        template<typename T>
        ME_NODISCARD T* GetComponent(uint64 entityId)
        {
            static_assert(std::is_base_of_v<Components::Component, T>, "Can't use non-component type in GetComponent() method!");
            ME_ASSERT(HasComponent<T>(entityId), "Can't get a non-existing component!");
            ComponentType type = GetComponentType<T>();
            return GetComponentArray<T>()->GetData(entityId);
        }

        template<typename T>
        void RemoveComponent(uint64 entityId)
        {
            static_assert(std::is_base_of_v<Components::Component, T>, "Can't use non-component type in GetComponent() method!");
            ME_ASSERT(HasComponent<T>(entityId), "Can't remove a non-existing component!");
            ComponentType type = GetComponentType<T>();
            GetComponentArray<T>()->RemoveData(entityId);
        }

        void OnEntityDestroyed(uint64 entityId)
        {
            for (auto& compArr : m_ComponentArrays)
                compArr.Value2->Contains(entityId);
        }

        template<typename T>
        ME::Core::Memory::Reference<Helper::ComponentArray<T>> GetComponents()
        {
            return GetComponentArray<T>();
        }

    public:
        template <typename T>
        static ComponentType GetComponentType()
        {
            static_assert(std::is_base_of_v<Components::Component, T>, "Can't use non-component type in GetComponentType() method!");
            static ComponentType typeID = Helper::ComponentIDGenerator::ID<T>();
            return typeID;
        }

        template <typename T>
        static ME::Core::Memory::Reference<T> Create()
        {
            ME::Core::Memory::Reference<T> component = nullptr;
            component = ME::Core::Memory::MakeReference<T>();
            return component;
        }

        template <typename T, typename... vArg>
        static ME::Core::Memory::Reference<T> Create(vArg&&... args)
        {
            ME::Core::Memory::Reference<T> component = nullptr;
            component = ME::Core::Memory::MakeReference<T>(std::forward<vArg>(args)...);
            return component;
        }

    private:
        template<typename T>
        ME::Core::Memory::Reference<Helper::ComponentArray<T>> GetComponentArray()
        {
            ME_ASSERT(m_RegisteredComponents.Find(GetComponentType<T>()) != m_RegisteredComponents.End(), "Component not registered before use.");
            return std::static_pointer_cast<Helper::ComponentArray<T>>(m_ComponentArrays[GetComponentType<T>()]);
        }

    private:
        ME::Core::UnorderedSet<ComponentType> m_RegisteredComponents;
        ME::Core::UnorderedMap<ComponentType, ME::Core::Memory::Reference<Helper::IComponentArray>> m_ComponentArrays;
    };
}