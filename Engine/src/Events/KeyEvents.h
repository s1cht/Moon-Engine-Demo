#pragma once

#include <sstream>
#include "Core.h"
#include "Events/Event.h"

class PAWN_API KeyEvent : public IEvent
{
public:
    EVENT_CLASS_CATEGORY(EventCategory_Keyboard | EventCategory_Input)

public:
    inline int GetKeyCode() const { return m_KeyCode; }

protected:
    KeyEvent(int keycode) : m_KeyCode(keycode) {}
    int m_KeyCode;

};

class PAWN_API KeyInputStartedEvent : public KeyEvent
{
public:
    KeyInputStartedEvent(int keycode, int repeatCount)
        : KeyEvent(keycode), m_repeatCount(repeatCount) {}

public:
    EVENT_CLASS_TYPE(KeyInputStarted)
    EVENT_CLASS_GETSTRING(var_to_string_n(int, m_KeyCode, "keycode") + var_to_string_n(int, m_repeatCount, "repeatCount"));

public:
    inline int GetRepeatCount() const { return m_repeatCount; }

private:

    int m_repeatCount;
};

class PAWN_API KeyInputEndedEvent : public KeyEvent
{
public:
    KeyInputEndedEvent(int keycode, int repeatCount)
        : KeyEvent(keycode) {
    }

public:
    EVENT_CLASS_TYPE(KeyInputEnded)
    EVENT_CLASS_GETSTRING(var_to_string_n(int, m_KeyCode, "keycode"));

};

