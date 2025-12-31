#include "Entity.hpp"
#include "ECS/Managers/EntityManager.hpp"
#include "Managers/ComponentManager.hpp"

namespace ME::ECS
{
    Entity::Entity()
        : m_EntityId(ENT_NIL), m_RelatedWorld() {}

    Entity::Entity(ME::Core::Memory::WeakReference<World> relatedWorld, uint64 id)
        : m_EntityId(id), m_RelatedWorld(relatedWorld), m_EntityType(~0ull) {}

    Entity::~Entity()
    {
        Destroy();
    }

    bool Entity::operator<(const Entity& other) const noexcept
    {
        return m_EntityId < other.GetID();
    }
}
