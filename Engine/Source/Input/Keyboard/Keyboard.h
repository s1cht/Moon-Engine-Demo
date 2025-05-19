#pragma once
#include <Core.hpp>

#include "Input/Keycodes.h"
#include "Events/KeyEvents.h"

#define PE_MAX_REPEAT_COUNT 50

#define CHECK_KEY_VALUE(x)	if (x > PE_MAX_KEYCODE_COUNT)\
								return false;

namespace Pawn::Input::Devices
{
	class PAWN_API Keyboard
	{
	public:
		EVENT_CALLBACK_FUNCTION;

	public:
		Keyboard();
		~Keyboard();

	public:
		void SetKeyPressed(uint8 key, bool pressed);
		void SetEventCallback(const EventCallbackFunc& callback) { m_Callback = callback; m_CallbackExists = true; }

	public:
		bool IsKeyHolded(uint8 key)
		{
			CHECK_KEY_VALUE(key);
			return m_KeysPressing[key];
		}

		uint8 GetRepeatCount(uint8 key)
		{
			CHECK_KEY_VALUE(key);
			return m_KeyRepeatCount[key];
		}

	private:
		bool m_KeysPressing[PE_MAX_KEYCODE_COUNT];
		uint8 m_KeyRepeatCount[PE_MAX_KEYCODE_COUNT];

	private:
		bool m_CallbackExists = false;
		EventCallbackFunc m_Callback;

	};

};