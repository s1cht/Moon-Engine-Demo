
#include "Keyboard.h"

namespace Pawn {

	Keyboard::Keyboard()
	{
		for (SIZE_T i = 0; i < PE_MAX_KEYCODE_COUNT; i++)
			m_KeyRepeatCount[i] = 0;
	}
	Keyboard::~Keyboard()
	{
	}

	void Keyboard::SetKeyPressed(uint8 key, bool pressed)
	{
		if (key > PE_MAX_KEYCODE_COUNT)
			return;

		if (m_KeysPressing[key] == true && pressed)
		{
			if (m_KeyRepeatCount[key] < PE_MAX_REPEAT_COUNT)
				m_KeyRepeatCount[key]++;

			KeyInputStartedEvent inputStartedEvent(key, m_KeyRepeatCount[key]);
			m_Callback(inputStartedEvent);

			return;
		}

		m_KeysPressing[key] = pressed;

		if (!pressed)
		{
			m_KeyRepeatCount[key] = 0;

			KeyInputEndedEvent inputEndedEvent(key);
			m_Callback(inputEndedEvent);
		}
		else
		{
			KeyInputStartedEvent inputStartedEvent(key, m_KeyRepeatCount[key]);
			m_Callback(inputStartedEvent);
		}
		
		return;
	}

};