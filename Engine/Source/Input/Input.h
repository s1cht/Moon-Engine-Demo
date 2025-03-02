#pragma once
#include "pch.h"
#include "Core/Core.h"
#include "Keycodes.h"
#include "Mouse/Mouse.h"
#include "Keyboard/Keyboard.h"
#include "Core/Events/Event.h"

namespace Pawn {


	class PAWN_API Input
	{
		using EventCallbackFunc = std::function<void(Event&)>;

	public:
		constexpr Input();
		~Input();

	public:
		void SetEventCallback(const EventCallbackFunc& callback);

	public:
		bool IsMouseLeftButtonUp();
		bool IsMouseLeftButtonDown();

		bool IsMouseMiddleButtonUp();
		bool IsMouseMiddleButtonDown();

		bool IsMouseRightButtonUp();
		bool IsMouseRightButtonDown();

	public:
		bool IsKeyUp(Keycode keycode);
		bool IsKeyDown(Keycode keycode);

	public:
		static Keycode ConvertPlatformKeycode(uint16 keycode);

	private:
		//Mouse m_Mouse;
		//Keyboard m_Keyboard;

	};

}

#define BEGIN_KEYCODE_CONVERTATION switch(keycode) {
#define END_KEYCODE_CONVERTATION default: return PE_UNKNOWN; }

#define ADD_KEYCODE(origKey, keycode) case origKey:		\
										{ return keycode; break;}