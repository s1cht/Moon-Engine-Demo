#pragma once
#include <Core.hpp>

#include "ECS/Managers/ComponentManager.hpp"
#include "ECS/Managers/EntityManager.hpp"
#include "Managers/SystemScheduler.hpp"

namespace ME::ECS
{
    class MEAPI World : public std::enable_shared_from_this<World>
    {
    public:
        World();

    public:
        void OnUpdate(float32 dT)
        {
            m_DeltaTime = dT;
            m_EntityManager->OnUpdate(m_DeltaTime);
        }

        float32 GetDeltaTime() const
        {
            return m_DeltaTime;
        }

    public:
        template <typename T, typename... argT>
        ME_NODISCARD ME::Core::Memory::Reference<T> Create(argT&&... args)
        {
            ME::Core::Memory::WeakReference<World> worldWeak = weak_from_this();

            ME::Core::Memory::Reference<T> ent = ME::Core::Memory::MakeReference<T>(worldWeak, m_EntityManager->OnEntityCreated(), std::forward<argT>(args)...);

            Entity::EntityVirtualFunctions functions = {};
            functions.OnCreated = std::bind(&T::OnCreated, ent.get());
            functions.OnUpdate = std::bind(&T::OnUpdate, ent.get(), std::placeholders::_1);
            functions.OnDestroy = std::bind(&T::OnDestroy, ent.get());

            EntityPack entPack = {};
            entPack.Entity = ent;
            entPack.HasVf = true;
            entPack.vfptrs = functions;

            m_EntityManager->AddEntity(ent->GetID(), entPack);

            entPack.vfptrs.OnCreated();
            return ent;
        }

        ME_NODISCARD ME::Core::Memory::Reference<Entity> Create()
        {
            ME::Core::Memory::Reference<World> worldRef = shared_from_this();
            ME::Core::Memory::WeakReference<World> worldWeak = worldRef;

            ME::Core::Memory::Reference<Entity> ent = ME::Core::Memory::MakeReference<Entity>(worldWeak, m_EntityManager->OnEntityCreated());

            EntityPack entPack = {};
            entPack.Entity = ent;
            entPack.HasVf = false;
            entPack.vfptrs = {};

            m_EntityManager->AddEntity(ent->GetID(), entPack);
            return ent;
        }

        void DestroyEntity(uint64 entityID) const
        {
            m_EntityManager->DestroyEntity(entityID);
            m_ComponentManager->OnEntityDestroyed(entityID);
            m_SystemScheduler->OnEntityDestroyed(entityID);
        }

        template<typename T>
        void AttachComponent(uint64 entity, T component)
        {
            m_ComponentManager->AttachComponent<T>(entity, component);

            auto signature = m_EntityManager->GetSignature(entity);
            signature.set(m_ComponentManager->GetComponentType<T>(), true);
            m_EntityManager->SetSignature(entity, signature);

            m_SystemScheduler->EntitySignatureChanged(entity, signature);
        }

        template<typename T>
        void AddComponent(uint64 entity)
        {
            m_ComponentManager->AddComponent<T>(entity);
            auto signature = m_EntityManager->GetSignature(entity);
            signature.set(m_ComponentManager->GetComponentType<T>(), true);
            m_EntityManager->SetSignature(entity, signature);

            m_SystemScheduler->EntitySignatureChanged(entity, signature);
        }

        template<typename T>
        void RegisterComponent()
        {
            m_ComponentManager->RegisterComponent<T>();
        }

        template<typename T>
        void RemoveComponent(uint64 entityID)
        {
            m_ComponentManager->RemoveComponent<T>(entityID);
        }

        template<typename T>
        bool HasComponent(uint64 entityID) const
        {
            return m_ComponentManager->HasComponent<T>(entityID);
        }

        template<typename T>
        T& GetComponent(uint64 entityID)
        {
            return *m_ComponentManager->GetComponent<T>(entityID);
        }

        template<typename T>
        ME::Core::Array<uint64> GetComponentEntities()
        {
            return m_ComponentManager->GetComponents<T>()->GetEntities();
        }

        template<typename T>
        ME::Core::Memory::Reference<T> RegisterSystem()
        {
            return m_SystemScheduler->RegisterSystem<T>();
        }

        template<typename T>
        void SetSystemSignature(Signature signature)
        {
            m_SystemScheduler->SetSignature<T>(signature);
        }

        template<typename T, typename... argTs>
        ME::Core::Array<ME::Core::Memory::Reference<Entity>> View()
        {
            ME::Core::Array<ME::Core::Memory::Reference<Entity>> entities = {};
            ME::Core::Array<uint64> entitiesWithT = GetComponentEntities<T>();
            for (const auto& ent : entitiesWithT)
            {
                bool hasAllComponents = true;

                if constexpr (sizeof...(argTs) > 0)
                    hasAllComponents = (m_ComponentManager->HasComponent<argTs>(ent) && ...);

                if (hasAllComponents)
                    entities.EmplaceBack(m_EntityManager->GetEntity(ent));
            }
            return entities;
        }

    private:
        ME::Core::Memory::Scope<ComponentManager> m_ComponentManager;
        ME::Core::Memory::Scope<EntityManager> m_EntityManager;
        ME::Core::Memory::Scope<SystemScheduler> m_SystemScheduler;

        float32 m_DeltaTime;
    };

    // Entity method implementation

    template <typename T>
    void Entity::AttachComponent(Components::Component component) const
    {
        if (auto world = m_RelatedWorld.lock())
        {
            world->AttachComponent<T>(m_EntityId, component);
            return;
        }
        ME_CRITICAL("Attempted to attach component in a destroyed world!");
        throw std::runtime_error("World context unavailable.");
    }

    template <typename T>
    void Entity::AddComponent() const
    {
        if (auto world = m_RelatedWorld.lock())
        {
            world->AddComponent<T>(m_EntityId);
            return;
        }
        ME_CRITICAL("Attempted to attach component in a destroyed world!");
        throw std::runtime_error("World context unavailable.");
    }

    template <typename T>
    T& Entity::GetComponent() const
    {
        if (auto world = m_RelatedWorld.lock())
            return world->GetComponent<T>(m_EntityId);
        ME_CRITICAL("Attempted to get component from a destroyed world!");
        throw std::runtime_error("World context unavailable.");
    }

    template <typename T>
    bool Entity::HasComponent() const
    {
        if (auto world = m_RelatedWorld.lock())
            return world->HasComponent<T>(m_EntityId);
        ME_CRITICAL("Attempted to check for having a component in a destroyed world!");
        throw std::runtime_error("World context unavailable.");
    }

    template <typename T>
    void Entity::RemoveComponent() const
    {
        if (auto world = m_RelatedWorld.lock())
        {
            world->RemoveComponent<T>(m_EntityId);
            return;
        }
        ME_CRITICAL("Attempted to remove component in a destroyed world!");
        throw std::runtime_error("World context unavailable.");
    }

    inline void Entity::Destroy()
    {
        OnDestroy();
        if (auto world = m_RelatedWorld.lock())
            return world->DestroyEntity(m_EntityId);
        ME_CRITICAL("Attempted to destroy entity in a destroyed world!");
        throw std::runtime_error("World context unavailable.");
    }
}
