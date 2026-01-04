#pragma once
#include "ECS/Component.hpp"
#include "ECS/Managers/ComponentManager.hpp"
#include "Audio/AudioEngine.hpp"

namespace ME::Components
{
    struct MEAPI AudioComponent : ECS::Components::Component
    {
        ME_COMPONENT_CONSTRUCTOR(AudioComponent), Audio(nullptr) {}
        ~AudioComponent() override
        {
            
        };

        ME::Core::Memory::Reference<Audio::Audio> Audio;
    };
}