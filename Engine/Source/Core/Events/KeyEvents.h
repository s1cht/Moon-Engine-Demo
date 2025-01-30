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
        inline int32 GetKeyCode() const { return m_KeyCode; }

    protected:
        KeyEvent(int32 keycode) : m_KeyCode(keycode) {}
        int32 m_KeyCode;

    };

    // Input started event. Args: keycode(int32), repeatCount(int32)
    class PAWN_API KeyInputStartedEvent : public KeyEvent
    {
    public:
        KeyInputStartedEvent(int32 keycode, int32 repeatCount)
            : KeyEvent(keycode), m_repeatCount(repeatCount) {
        }

    public:
        EVENT_CLASS_TYPE(KeyInputStarted)
            EVENT_CLASS_GETSTRING(var_to_string_n(int32, m_KeyCode, "keycode") + var_to_string_n(int32, m_repeatCount, "repeatCount"));

    public:
        inline int32 GetRepeatCount() const { return m_repeatCount; }

    private:

        int32 m_repeatCount;
    };

    class PAWN_API KeyInputEndedEvent : public KeyEvent
    {
    public:
        KeyInputEndedEvent(int32 keycode, int32 repeatCount)
            : KeyEvent(keycode) {
        }

    public:
        EVENT_CLASS_TYPE(KeyInputEnded)
            EVENT_CLASS_GETSTRING(var_to_string_n(int32, m_KeyCode, "keycode"));

    };

}
