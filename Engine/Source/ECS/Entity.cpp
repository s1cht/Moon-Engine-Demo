#include "Entity.hpp"

namespace ME::ECS
{
    Entity::Entity(uint64 id)
        : m_EntityId(id)
    {
    }

    bool Entity::operator<(const Entity& other) const noexcept
    {
        return m_EntityId < other.GetID();
    }

    Entity Entity::Create()
    {
        return Entity(ComponentManager::Get().GetEntityID());
    }
}
