#pragma once

#include "pch.h"
#include "Core/Core.h"
#include "Core/Events/Event.h"

namespace Pawn {

    class PAWN_API KeyEvent : public IEvent
    {
    public:
        EVENT_CLASS_CATEGORY(EventCategory_Keyboard | EventCategory_Input)

    public:
        inline int16 GetKeyCode() const { return m_KeyCode; }

    protected:
        KeyEvent(int16 keycode) : m_KeyCode(keycode) {}
        int16 m_KeyCode;

        friend class Input;
    };

    // Input started event. Args: keycode(int32), repeatCount(int32)
    class PAWN_API KeyInputStartedEvent : public KeyEvent
    {
    public:
        KeyInputStartedEvent(int16 keycode, int8 repeatCount)
            : KeyEvent(keycode), m_repeatCount(repeatCount) {
        }

    public:
        EVENT_CLASS_TYPE(KeyInputStarted)
            EVENT_CLASS_GETSTRING(PE_NAMED_VARIABLE_TOSTRING(int16, m_KeyCode, "keycode") + PE_NAMED_VARIABLE_TOSTRING(int8, m_repeatCount, "repeatCount"));

    public:
        inline int8 GetRepeatCount() const { return m_repeatCount; }

    private:

        int8 m_repeatCount;
    };

    class PAWN_API KeyInputEndedEvent : public KeyEvent
    {
    public:
        KeyInputEndedEvent(int16 keycode)
            : KeyEvent(keycode) {
        }

    public:
        EVENT_CLASS_TYPE(KeyInputEnded)
            EVENT_CLASS_GETSTRING(PE_NAMED_VARIABLE_TOSTRING(int16, m_KeyCode, TEXT("keycode")));

    };

}
