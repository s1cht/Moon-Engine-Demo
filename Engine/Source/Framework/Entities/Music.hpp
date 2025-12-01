#pragma once
#include "ECS/World.hpp"
#include "ECS/Entity.hpp"
#include "Framework/Components/AudioComponent.hpp"

namespace ME
{
    class Music : ME_ENTITY_INHERIT(Music)
    {
        ME_ENT_DEFAULT(Music)

    public:
        ~Music() override = default;

    public:
        void OnCreated() override
        {
            AddComponent<Components::AudioComponent>();
        }

        inline Components::AudioComponent& Audio() const { return GetComponent<Components::AudioComponent>(); }
    };
}