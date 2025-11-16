#pragma once

#include <Core.hpp>
#include <Core/Utility/Logging/Logger.hpp>

#include "Component.hpp"
#include "Managers/ECSLimits.hpp"

#define ME_ENT_DEFAULT(entityClassName)                                                             \
public: entityClassName()                                                                           \
    : Entity() {}                                                                                   \
public: entityClassName(ME::Core::Memory::WeakReference<ME::ECS::World> relatedWorld, uint64 id)    \
    : Entity(relatedWorld, id) {}                                                                   \
private:
                                        
#define ME_ENT_CONSTRUCTOR(entityClassName, .../*arguments*/) entityClassName(ME::Core::Memory::WeakReference<World> relatedWorld, uint64 id, __VA_ARGS__) : Entity(relatedWorld, uint64 id)
#define ME_ENTITY_INHERIT(entityClassName)  public ME::ECS::Entity

namespace ME::ECS
{
    class World;

    class MEAPI Entity
    {
    public:
        struct EntityVirtualFunctions
        {
            std::function<void()> OnCreated;
            std::function<void(float32)> OnUpdate;
            std::function<void()> OnDestroy;
        };

    public:
        Entity();
        Entity(ME::Core::Memory::WeakReference<World> relatedWorld, uint64 id);
        Entity(const Entity&) = delete;
        Entity(Entity&&) = delete;

        virtual ~Entity();

    public:
        virtual void OnCreated() {}
        virtual void OnUpdate(float32 deltaTime) {}
        virtual void OnDestroy() {}

    public:
        bool operator<(const Entity& other) const noexcept;

    public:
        bool Exists() const { return m_EntityId != ENT_NIL; }

    public:
        template<typename T>
        void AttachComponent(Components::Component component) const;

        template<typename T>
        void AddComponent() const;

        template<typename T>
        ME_NODISCARD T& GetComponent() const;

        template<typename T>
        ME_NODISCARD bool HasComponent() const;

        template<typename T>
        void RemoveComponent() const;

        void Destroy();

        ME_NODISCARD uint64 GetID() const
        {
            return m_EntityId;
        }

    private:
        uint64 m_EntityId;
        ME::Core::Memory::WeakReference<World> m_RelatedWorld;
    };
}
