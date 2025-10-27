#pragma once

#include "EventsShared.h"

#include <Core.hpp>
#include <Core/Events/Event.hpp>

namespace ME::Events
{
    class WindowClosedEvent : public Core::Event
    {
    public:
        EVENT_CLASS_CATEGORY(EventCategory_Window | EventCategory_Application);
        EVENT_CLASS_TYPE(WindowClosed);

    };

    class WindowMovedEvent : public Core::Event
    {
    public:
        WindowMovedEvent(int32 x, int32 y) : m_x((float)x), m_y((float)y) {}
        WindowMovedEvent(float x, float y) : m_x(x), m_y(y) {}

    public:
        EVENT_CLASS_CATEGORY(EventCategory_Window | EventCategory_Application);
        EVENT_CLASS_TYPE(WindowMoved);
        EVENT_CLASS_GETSTRING(ME_NAMED_VARIABLE_TOSTRING(float, m_x, "X") + ME_NAMED_VARIABLE_TOSTRING(float, m_y, "Y"));

    public:
        float GetPositionX() { return m_x; };
        float GetPositionY() { return m_y; };

    private:
        float m_x, m_y;

    };

    class WindowResizedEvent : public Core::Event
    {
    public:
        WindowResizedEvent(int32 x, int32 y) : m_x((float)x), m_y((float)y) {}
        WindowResizedEvent(float x, float y) : m_x(x), m_y(y) {}

    public:
        EVENT_CLASS_CATEGORY(EventCategory_Window | EventCategory_Application);
        EVENT_CLASS_TYPE(WindowResized);
        EVENT_CLASS_GETSTRING(ME_NAMED_VARIABLE_TOSTRING(float, m_x, "X") + ME_NAMED_VARIABLE_TOSTRING(float, m_y, "Y"));

    public:
        float GetSizeX() { return m_x; };
        float GetSizeY() { return m_y; };

    private:
        float m_x, m_y;

    };

    class WindowFocusedEvent : public Core::Event
    {
    public:
        EVENT_CLASS_CATEGORY(EventCategory_Window | EventCategory_Application);
        EVENT_CLASS_TYPE(WindowFocused);

    };

    class WindowLostFocusEvent : public Core::Event
    {
    public:
        EVENT_CLASS_CATEGORY(EventCategory_Window | EventCategory_Application);
        EVENT_CLASS_TYPE(WindowLostFocus);

    };

}