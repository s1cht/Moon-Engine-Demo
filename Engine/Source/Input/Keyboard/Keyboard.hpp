#pragma once
#include <Core.hpp>

#include "Input/Keycodes.hpp"
#include "KeyEvents.hpp"

#define CHECK_KEY_VALUE(x)	if ((x)> ME_MAX_KEYCODE_COUNT)\
								return false;

constexpr uint32 ME_MAX_REPEAT_COUNT = 50;

namespace ME::Input::Devices
{
	class MEAPI Keyboard
	{
	public:
		EVENT_CALLBACK_FUNCTION;

	public:
		Keyboard();
		~Keyboard() = default;

	public:
		void SetKeyPressed(uint8 key, bool pressed);
		void SetEventCallback(const EventCallbackFunc& callback) { m_Callback = callback; m_CallbackExists = true; }

	public:
		bool IsKeyHolded(uint8 key) const
		{
			CHECK_KEY_VALUE(key);
			return m_KeysPressing[key];
		}

		uint8 GetRepeatCount(uint8 key) const
		{
			CHECK_KEY_VALUE(key); 
			return m_KeyRepeatCount[key];
		}

	private:
		bool m_KeysPressing[ME_MAX_KEYCODE_COUNT];
		uint8 m_KeyRepeatCount[ME_MAX_KEYCODE_COUNT];

	private:
		bool m_CallbackExists = false;
		EventCallbackFunc m_Callback;

	};

};