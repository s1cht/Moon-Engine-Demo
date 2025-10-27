#pragma once

#include "EventsShared.h"

#include <Core.hpp>
#include <Core/Events/Event.hpp>

namespace ME::Events
{
    class KeyEvent : public ME::Core::Event
    {
    public:
        EVENT_CLASS_CATEGORY(EventCategory_Keyboard | EventCategory_Input)

    public:
        inline int16 GetKeyCode() const { return m_KeyCode; }

    protected:
        KeyEvent(int16 keycode) : m_KeyCode(keycode) {}
        int16 m_KeyCode;

        friend class InputController;
    };

    // Input started event. Args: keycode(int32), repeatCount(int32)
    class KeyInputStartedEvent final : public KeyEvent
    {
    public:
        KeyInputStartedEvent(int16 keycode, int8 repeatCount)
            : KeyEvent(keycode), m_repeatCount(repeatCount)
        {}
        ~KeyInputStartedEvent() override = default;

    public:
        EVENT_CLASS_TYPE(KeyInputStarted);
        EVENT_CLASS_GETSTRING(ME_NAMED_VARIABLE_TOSTRING(int16, m_KeyCode, "keycode") + ME_NAMED_VARIABLE_TOSTRING(int8, m_repeatCount, "repeatCount"));

    public:
        inline int8 GetRepeatCount() const { return m_repeatCount; }

    private:

        int8 m_repeatCount;
    };

    class KeyInputEndedEvent : public KeyEvent
    {
    public:
        KeyInputEndedEvent(int16 keycode)
            : KeyEvent(keycode)
        {}
        ~KeyInputEndedEvent() override = default;

    public:
        EVENT_CLASS_TYPE(KeyInputEnded);
        EVENT_CLASS_GETSTRING(ME_NAMED_VARIABLE_TOSTRING(int16, m_KeyCode, TEXT("keycode")));
    };

}
