#pragma once
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
        WindowMovedEvent(int32 x, int32 y) : m_X(static_cast<float32>(x)), m_Y(static_cast<float32>(y)) {}
        WindowMovedEvent(float x, float y) : m_X(x), m_Y(y) {}

    public:
        EVENT_CLASS_CATEGORY(EventCategory_Window | EventCategory_Application);
        EVENT_CLASS_TYPE(WindowMoved);
        EVENT_CLASS_GETSTRING(ME_NAMED_VARIABLE_TOSTRING(float, m_X, "X") + ME_NAMED_VARIABLE_TOSTRING(float, m_Y, "Y"));

    public:
        ME_NODISCARD float32 GetPositionX() const { return m_X; };
        ME_NODISCARD float32 GetPositionY() const { return m_Y; };

    private:
        float32 m_X, m_Y;
    };

    class WindowResizedEvent : public Core::Event
    {
    public:
        WindowResizedEvent(int32 x, int32 y) : m_X(static_cast<float32>(x)), m_Y(static_cast<float32>(y)) {}
        WindowResizedEvent(float32 x, float32 y) : m_X(x), m_Y(y) {}

    public:
        EVENT_CLASS_CATEGORY(EventCategory_Window | EventCategory_Application);
        EVENT_CLASS_TYPE(WindowResized);
        EVENT_CLASS_GETSTRING(ME_NAMED_VARIABLE_TOSTRING(float, m_X, "X") + ME_NAMED_VARIABLE_TOSTRING(float, m_Y, "Y"));

    public:
        ME_NODISCARD float32 GetSizeX() const { return m_X; }
        ME_NODISCARD float32 GetSizeY() const { return m_Y; }

    private:
        float32 m_X, m_Y;
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