#pragma once

#include "Events/Event.h"

class WindowClosedEvent : public IEvent
{
public:
    EVENT_CLASS_CATEGORY(EventCategory_Window | EventCategory_Application);
    EVENT_CLASS_TYPE(WindowClosed);
    
};

class WindowMovedEvent : public IEvent
{
public:
    WindowMovedEvent(int x, int y) : m_x((float)x), m_y((float)y) {}
    WindowMovedEvent(float x, float y) : m_x(x), m_y(y) {}

public:
    EVENT_CLASS_CATEGORY(EventCategory_Window | EventCategory_Application);
    EVENT_CLASS_TYPE(WindowMoved);
    EVENT_CLASS_GETSTRING(var_to_string_n(float, m_x, "X") + var_to_string_n(float, m_y, "Y"));

public:
    float GetPositionX() { return m_x; };
    float GetPositionY() { return m_y; };

private:
    float m_x, m_y;

};

class WindowResizedEvent : public IEvent
{
public:
    WindowResizedEvent(int x, int y) : m_x((float)x), m_y((float)y) {}
    WindowResizedEvent(float x, float y) : m_x(x), m_y(y) {}

public:
    EVENT_CLASS_CATEGORY(EventCategory_Window | EventCategory_Application);
    EVENT_CLASS_TYPE(WindowResized);
    EVENT_CLASS_GETSTRING(var_to_string_n(float, m_x, "X") + var_to_string_n(float, m_y, "Y"));

public:
    float GetSizeX() { return m_x; };
    float GetSizeY() { return m_y; };

private:
    float m_x, m_y;

};

class WindowFocusedEvent : public IEvent
{
public:
    EVENT_CLASS_CATEGORY(EventCategory_Window | EventCategory_Application);
    EVENT_CLASS_TYPE(WindowFocused);

};

class WindowLostFocusEvent : public IEvent
{
public:
    EVENT_CLASS_CATEGORY(EventCategory_Window | EventCategory_Application);
    EVENT_CLASS_TYPE(WindowLostFocus);

};