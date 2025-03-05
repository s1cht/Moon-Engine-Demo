#pragma once

#include "pch.h"
#include "Core/Events/Event.h"

namespace Pawn {

    class PAWN_API MouseEvent : public IEvent
    {
    public:
        EVENT_CLASS_CATEGORY(EventCategory_Input | EventCategory_Mouse);
    };

    class PAWN_API MouseButtonEvent : public MouseEvent
    {
    public:
        EVENT_CLASS_GETSTRING(PE_NAMED_VARIABLE_TOSTRING(int32, m_Button, "button"));

    protected:
        constexpr MouseButtonEvent(int32 btn) : m_Button(btn) {};
        int32 m_Button;
    };

    class PAWN_API MouseMovedEvent : public MouseEvent
    {
    public:
        constexpr MouseMovedEvent() : m_x(0.f), m_y(0.f) {};
        constexpr MouseMovedEvent(float x, float y) : m_x(x), m_y(y) {};

    public:
        EVENT_CLASS_TYPE(MouseMoved)

    public:
        float GetPositionX() const { return m_x; }
        float GetPositionY() const { return m_y; }

        EVENT_CLASS_GETSTRING(PE_NAMED_VARIABLE_TOSTRING(float, m_x, "offset_x") + PE_NAMED_VARIABLE_TOSTRING(float, m_y, "offset_y"))

    private:
        float m_x, m_y;
    };

    // Mouse scrolled event
    class PAWN_API MouseScrolledEvent : public MouseEvent
    {
    public:
        constexpr MouseScrolledEvent() : m_x(0.f), m_y(0.f) {};
        constexpr MouseScrolledEvent(float x, float y) : m_x(x), m_y(y) {};

    public:
        EVENT_CLASS_TYPE(MouseMoved);
        EVENT_CLASS_GETSTRING(PE_NAMED_VARIABLE_TOSTRING(float, m_x, "pos_x") + PE_NAMED_VARIABLE_TOSTRING(float, m_y, "pos_y"));

    public:
        float GetOffsetX() const { return m_x; }
        float GetOffsetY() const { return m_y; }

    private:
        float m_x, m_y;
    };

    class PAWN_API MouseButtonPressedEvent : public MouseButtonEvent
    {
    public:
        constexpr MouseButtonPressedEvent(int32 btn) : MouseButtonEvent(btn) {}

    public:
        EVENT_CLASS_TYPE(MouseButtonPressed);

    };

    class PAWN_API MouseButtonReleasedEvent : public MouseButtonEvent
    {
    public:
        constexpr MouseButtonReleasedEvent(int32 btn) : MouseButtonEvent(btn) {}

    public:
        EVENT_CLASS_TYPE(MouseButtonReleased);

    };

}