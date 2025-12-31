#pragma once

#include <Core.hpp>
//#include <Core/Containers/Bitset.hpp>
//#include <Core/Containers/Queue.hpp>

//temporary
#include <queue>
#include <bitset>

#include <Core/Containers/Array.hpp>
#include <Core/Containers/Map.hpp>
#include <Core/TypeID.hpp>
#include "ECS/Entity.hpp"
#include "ECSLimits.hpp"

namespace ME::ECS
{
    using Signature = std::bitset<ME_MAX_COMPONENT_COUNT>;

    namespace Helper
    {
        using EntityTypeIDGenerator = Core::TypeIDGenerator<EntityType, ME_MAX_COMPONENT_COUNT>;
    }

    struct EntityPack
    {
        ME::Core::Memory::Reference<Entity> Entity;
        bool HasVf;
        Entity::EntityVirtualFunctions vfptrs;
    };

    class MEAPI EntityManager
    {
    public:
        EntityManager()
        {
            for (uint64 entity = 0; entity < ME_MAX_ENTITY_COUNT; ++entity)
                m_AvailableEntityIDs.push(entity);
        }

    public:
        void SetSignature(uint64 entityID, Signature signature)
        {
            ME_ASSERT(entityID < ME_MAX_ENTITY_COUNT, "Entity out of range.");
            m_Signatures[entityID] = signature;
        }

        Signature GetSignature(uint64 entityID) const
        {
            ME_ASSERT(entityID < ME_MAX_ENTITY_COUNT, "Entity out of range.");
            return m_Signatures[entityID];
        }

        void DestroyEntity(uint64 entityID)
        {
            ME_ASSERT(entityID < ME_MAX_ENTITY_COUNT, "Entity out of range.");

            m_Signatures[entityID].reset();

            m_AvailableEntityIDs.push(entityID);
            m_Entities.Erase(entityID);
            --m_EntityCount;
        }

        ME_NODISCARD uint64 OnEntityCreated()
        {
            ME_ASSERT(m_EntityCount < ME_MAX_ENTITY_COUNT, "Too many entities in existence.");
            uint64 id = m_AvailableEntityIDs.front();
            m_AvailableEntityIDs.pop();
            ++m_EntityCount;
            return id;
        }

        void AddEntity(uint64 entity, const EntityPack& entityPack)
        {
                m_Entities.Insert(entity, entityPack);
        }

        ME::Core::Memory::Reference<Entity> GetEntity(uint64 entity)
        {
            if (m_Entities.Find(entity) == m_Entities.End())
                return nullptr;
            return m_Entities.Find(entity)->Value2.Entity;
        }

        void OnCreated()
        {
            for (const auto& vfptrs : m_Entities)
                if (vfptrs.Value2.HasVf)
                   vfptrs.Value2.vfptrs.OnCreated();
        }

        void OnUpdate(float32 deltaTime)
        {
            for (const auto& vfptrs : m_Entities)
                if (vfptrs.Value2.HasVf)
                    vfptrs.Value2.vfptrs.OnUpdate(deltaTime);
        }

        void OnDestroy()
        {
            for (const auto& vfptrs : m_Entities)
                if (vfptrs.Value2.HasVf)
                    vfptrs.Value2.vfptrs.OnDestroy();
        }

        ME::Core::Map<uint64, EntityPack>& GetEntities()
        {
            return m_Entities;
        };

    public:
        template <typename T>
        static EntityType GetEntityType()
        {
            static_assert(std::is_base_of_v<ME::ECS::Entity, T>, "Can't use non-entity type in GetEntityType() method!");
            static EntityType typeID = Helper::EntityTypeIDGenerator::ID<T>();
            return typeID;
        }

    private:
        std::queue<uint64> m_AvailableEntityIDs;
        ME::Core::Array<Signature> m_Signatures;
        ME::Core::Map<uint64, EntityPack> m_Entities;
        ME::Core::Atomic_uint64 m_EntityCount;
    };
}
