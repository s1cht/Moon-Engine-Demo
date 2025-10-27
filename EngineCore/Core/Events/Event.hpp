#pragma once

#include "Core.hpp"
#include "Core/Containers/Array.hpp"
#include "Core/Containers/String/String.hpp"

enum class EventType
{
    None = 0,
    // Window events
    WindowClosed, WindowResized, WindowMoved, WindowFocused, WindowLostFocus,
    // Application events
    AppTick, AppUpdated, AppRendered,
    // Keyboard events
    KeyInputStarted, KeyInputEnded,
    // Mouse events
    MouseMoved, MouseScrolled, MouseButtonPressed, MouseButtonReleased
};

typedef int EVENT_CATEGORY; 

enum EventCategory
{
    None = 0,
    EventCategory_Application       = BIT(0),
    EventCategory_Window            = BIT(1),
    EventCategory_Input             = BIT(2),
    EventCategory_Keyboard          = BIT(3),
    EventCategory_Mouse             = BIT(4),
};

namespace ME::Core
{
    class COREAPI IEvent
    {
    public:
        virtual ~IEvent() = default;
        virtual EventType GetEventType() const = 0;
        virtual EVENT_CATEGORY GetEventCategory() const = 0;
        virtual const char8* GetName() const  = 0;
        virtual ME::Core::String GetString() { return GetName(); }
    
        inline bool IsInCategory(EventCategory category)
        {
            return  GetEventCategory() & category;
        }
    
        bool IsHandled = false;
    
    private:
        friend class EventDispatcher;
    
    };
    
    class COREAPI EventDispatcher
    {
    public:
        EventDispatcher(IEvent& event) : m_Event(event) {}
    
        template<typename T, typename F>
        bool Dispatch(const F& func)
        {
            if (m_Event.GetEventType() == T::GetStaticType())
            {
                m_Event.IsHandled = func(static_cast<T&>(m_Event));
                return true;
            }
            return false;
        }
    
    private:
        IEvent& m_Event;
    
    };

    typedef IEvent Event;
}