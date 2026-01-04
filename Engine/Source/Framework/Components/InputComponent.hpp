#pragma once
#include "ECS/Managers/ComponentManager.hpp"
#include "Input/Input.hpp"

namespace ME::Components
{
    struct MEAPI InputComponent : ECS::Components::Component
    {    
        ME_COMPONENT_CONSTRUCTOR(InputComponent) {}
        ~InputComponent() override = default;

        void OnEvent(Core::Event& event) 
        {
            ME::Core::EventDispatcher dispatcher(event);
            dispatcher.Dispatch<Events::MouseButtonPressedEvent>(OnMouseButtonPressed);
            dispatcher.Dispatch<Events::MouseButtonReleasedEvent>(OnMouseButtonReleased);
            dispatcher.Dispatch<Events::MouseMovedEvent>(OnMouseMoved);
            dispatcher.Dispatch<Events::KeyInputStartedEvent>(OnKeyStarted);
            dispatcher.Dispatch<Events::KeyInputEndedEvent>(OnKeyEnded);
        }

        std::function<bool(ME::Events::MouseButtonPressedEvent&)> OnMouseButtonPressed;
        std::function<bool(ME::Events::MouseButtonReleasedEvent&)> OnMouseButtonReleased;
        std::function<bool(ME::Events::MouseMovedEvent&)> OnMouseMoved;
        std::function<bool(ME::Events::KeyInputStartedEvent&)> OnKeyStarted;
        std::function<bool(ME::Events::KeyInputEndedEvent&)> OnKeyEnded;
    };
}