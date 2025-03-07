#pragma once

#include "pch.h"
#include "Core.h"
#include "Core/Bitwise.h"
#include "Core/Containers/String.h"

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

#define FORMAT_EVENT_STRING(eventName, args) String(TEXT("Event ")) + String(eventName) + TEXT(" fired with data: ") + args
#define EVENT_CLASS_GETSTRING(args)\
    virtual String GetString() override { return FORMAT_EVENT_STRING(GetName(), args); }

#define EVENT_CLASS_TYPE(type)\
    static EventType GetStaticType() { return EventType::##type; }\
    virtual EventType GetEventType() const override { return GetStaticType(); }\
    virtual const uchar* GetName() const override { return TEXT(#type); }

#define EVENT_CLASS_CATEGORY(category)\
    virtual EVENT_CATEGORY GetEventCategory() const override { return category; }

#define EVENT_CALLBACK_FUNCTION using EventCallbackFunc = std::function<void(Event&)>

namespace Pawn {

    class CORE_API IEvent
    {
    public:
        virtual EventType GetEventType() const = 0;
        virtual EVENT_CATEGORY GetEventCategory() const = 0;
        virtual const uchar* GetName() const  = 0;
        virtual String GetString() { return GetName(); }
    
        inline bool IsInCategory(EventCategory category)
        {
            return  GetEventCategory() & category;
        }
    
        bool IsHandled = false;
    
    private:
        friend class EventDispatcher;
    
    };
    
    class CORE_API EventDispatcher
    {
        template<typename T>
        using EventFunc = std::function<bool(T&)>;
    public:
        EventDispatcher(IEvent* event) : m_Event(event) {}
    
        template<typename T>
        bool Dispatch(EventFunc<T> func)
        {
            if (m_Event->GetEventType() == T::GetStaticType())
            {
                m_Event->IsHandled = func(*(T)(*m_Event));
                return true;
            }
            return false;
        }
    
    private:
        IEvent* m_Event;
    
    };

    typedef IEvent Event;
}


#define BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)