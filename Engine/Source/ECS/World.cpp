#include "World.hpp"

namespace ME::ECS
{
    World::World()
    {
        m_ComponentManager = ME::Core::Memory::MakeScope<ME::ECS::ComponentManager>();
        m_EntityManager = ME::Core::Memory::MakeScope<ME::ECS::EntityManager>();
        m_SystemScheduler = ME::Core::Memory::MakeScope<ME::ECS::SystemScheduler>();
        m_WorldDestroyed = false;
    }

    void World::Shutdown()
    {
        m_WorldDestroyed = true;
        m_SystemScheduler.reset();
        m_EntityManager.reset();
        m_ComponentManager.reset();
    }
}
