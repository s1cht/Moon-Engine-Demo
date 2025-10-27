#pragma once

#include "EventsShared.h"

#include <Core.hpp>
#include <Core/Events/Event.hpp>

namespace ME::Events
{
    class MouseEvent : public Core::Event
    {
    public:
        EVENT_CLASS_CATEGORY(EventCategory_Input | EventCategory_Mouse);
    };

    class MouseButtonEvent : public MouseEvent
    {
    public:
        EVENT_CLASS_GETSTRING(ME_NAMED_VARIABLE_TOSTRING(int32, m_Button, "button"));

    protected:
         MouseButtonEvent(int32 btn) : m_Button(btn) {};
        int32 m_Button;

    };

    class MouseMovedEvent : public MouseEvent
    {
    public:
         MouseMovedEvent() : m_x(0.f), m_y(0.f), m_xDelta(0.f), m_yDelta(0.f) {};
         MouseMovedEvent(float32 x, float32 y, float32 xDelta, float32 yDelta) : m_x(x), m_y(y), m_xDelta(xDelta), m_yDelta(yDelta) {};

    public:
        EVENT_CLASS_TYPE(MouseMoved)

    public:
        inline float32 GetPositionX() const { return m_x; }
        inline float32 GetPositionY() const { return m_y; }

        inline float32 GetDeltaX() const { return m_xDelta; }
        inline float32 GetDeltaY() const { return m_yDelta; }

        EVENT_CLASS_GETSTRING(
            ME_NAMED_VARIABLE_TOSTRING(float32, m_x, "PositionX") + 
            ME_NAMED_VARIABLE_TOSTRING(float32, m_y, "PositionY") + 
            ME_NAMED_VARIABLE_TOSTRING(float32, m_yDelta, "DeltaX") + 
            ME_NAMED_VARIABLE_TOSTRING(float32, m_yDelta, "DeltaY")
        )

    private:
        float32 m_x, m_y;
        float32 m_xDelta, m_yDelta;

    };

    // Mouse scrolled event
    class MouseScrolledEvent : public MouseEvent
    {
    public:
         MouseScrolledEvent() : m_x(0.f), m_y(0.f) {};
         MouseScrolledEvent(float x, float y) : m_x(x), m_y(y) {};

    public:
        EVENT_CLASS_TYPE(MouseMoved);
        EVENT_CLASS_GETSTRING(ME_NAMED_VARIABLE_TOSTRING(float32, m_x, "pos_x") + ME_NAMED_VARIABLE_TOSTRING(float32, m_y, "pos_y"));

    public:
        inline float32 GetOffsetX() const { return m_x; }
        inline float32 GetOffsetY() const { return m_y; }

    private:
        float32 m_x, m_y;

    };

    class MouseButtonPressedEvent : public MouseButtonEvent
    {
    public:
         MouseButtonPressedEvent(uint8 btn) : MouseButtonEvent(btn) {}

    public:
        EVENT_CLASS_TYPE(MouseButtonPressed);

    };

    class MouseButtonReleasedEvent : public MouseButtonEvent
    {
    public:
         MouseButtonReleasedEvent(uint8 btn) : MouseButtonEvent(btn) {}

    public:
        EVENT_CLASS_TYPE(MouseButtonReleased);

    };

}