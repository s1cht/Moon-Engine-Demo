module;


export module Pawn.Core.Event;

#include "Core.h"
import Pawn.Core;
import Pawn.Core.Container.String;

export enum class EventType
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

export typedef int EVENT_CATEGORY; 

export enum EventCategory
{
    None = 0,
    EventCategory_Application       = BIT(0),
    EventCategory_Window            = BIT(1),
    EventCategory_Input             = BIT(2),
    EventCategory_Keyboard          = BIT(3),
    EventCategory_Mouse             = BIT(4),
};

export namespace Pawn::Core
{
    class CORE_API IEvent
    {
    public:
        virtual EventType GetEventType() const = 0;
        virtual EVENT_CATEGORY GetEventCategory() const = 0;
        virtual const uchar* GetName() const  = 0;
        virtual Pawn::Core::Containers::String GetString() { return GetName(); }
    
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