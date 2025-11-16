#include "Keyboard.hpp"

namespace ME::Input::Devices
{
	Keyboard::Keyboard() : m_KeysPressing()
    {
        for (SIZE_T i = 0; i < ME_MAX_KEYCODE_COUNT; i++)
            m_KeyRepeatCount[i] = 0;
    }

    void Keyboard::SetKeyPressed(uint8 key, bool pressed)
	{
		if (key > ME_MAX_KEYCODE_COUNT)
			return;

		if (m_KeysPressing[key] == true && pressed)
		{
			if (m_KeyRepeatCount[key] < ME_MAX_REPEAT_COUNT)
				m_KeyRepeatCount[key]++;

			Events::KeyInputStartedEvent inputStartedEvent(key, m_KeyRepeatCount[key]);
			m_Callback(inputStartedEvent);

			return;
		}

		m_KeysPressing[key] = pressed;

		if (!pressed)
		{
			m_KeyRepeatCount[key] = 0;

			Events::KeyInputEndedEvent inputEndedEvent(key);
			m_Callback(inputEndedEvent);
		}
		else
		{
			Events::KeyInputStartedEvent inputStartedEvent(key, m_KeyRepeatCount[key]);
			m_Callback(inputStartedEvent);
		}
	}
};