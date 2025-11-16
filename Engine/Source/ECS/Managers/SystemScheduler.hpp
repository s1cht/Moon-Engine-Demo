#pragma once
#include <Core.hpp>
#include <Core/Containers/String.hpp>
#include <Core/Containers/Set.hpp>
#include <Core/Containers/UnorderedMap.hpp>
#include <Core/TypeID.hpp>

#include "EntityManager.hpp"

namespace ME::ECS
{
    using SystemType = uint64;

    namespace Helper
    {
        using SystemIDGenerator = Core::TypeIDGenerator<SystemType, UINT64_MAX>;
    }

    struct System
    {
        ME::Core::Set<uint64> Entities;
    };

    class MEAPI SystemScheduler
    {
    public:
        template<typename T>
        ME::Core::Memory::Reference<T> RegisterSystem()
        {
            SystemType typeID = GetSystemType<T>();
            if (m_Systems.Find(typeID) != m_Systems.End())
                ME_WARN("Registering system more than once.");

            ME::Core::Memory::Reference<T> system = ME::Core::Memory::MakeReference<T>();
            m_Systems.Insert(typeID, system);
            return system;
        }

        template<typename T>
        void SetSignature(Signature signature)
        {
            SystemType typeID = GetSystemType<T>();
            ME_ASSERT(m_Systems.Contains(typeID), "Can't use non-registered system!");
            m_Signatures.Insert(typeID, signature);
        }

        void OnEntityDestroyed(uint64 entityID)
        {
            for (const auto& pair : m_Systems)
                pair.Value2->Entities.Erase(entityID);
        }

        void EntitySignatureChanged(uint64 entityID, Signature entitySignature)
        {
            for (auto const& pair : m_Systems)
            {
                const uint64& type = pair.Value1;
                const ME::Core::Memory::Reference<System>& system = pair.Value2;
                const Signature& systemSignature = m_Signatures[type];

                if ((entitySignature & systemSignature) == systemSignature)
                    system->Entities.Insert(entityID);
                else
                    system->Entities.Erase(entityID);
            }
        }

    public:
        template <typename T>
        static SystemType GetSystemType()
        {
            static_assert(std::is_base_of_v<System, T>, "Can't use non-component type in GetComponentType() method!");
            static SystemType typeID = Helper::SystemIDGenerator::ID<T>();
            return typeID;
        }

    private:
        ME::Core::UnorderedMap<SystemType, ECS::Signature> m_Signatures;
        ME::Core::UnorderedMap<SystemType, ME::Core::Memory::Reference<System>> m_Systems;
    };
}