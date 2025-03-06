#pragma once
#include "pch.h"
#include "Core/Core.h"
#include "Keycodes.h"
#include "Core/Events/KeyEvents.h"
#include "Core/Events/MouseEvents.h"
#include "Mouse/Mouse.h"
#include "Keyboard/Keyboard.h"
#include "Core/Events/Event.h"

namespace Pawn {

	class PAWN_API Input
	{
	public:
		using EventCallbackFunc = std::function<void(Event&)>;
	public:
		Input(const Input&) = delete;

		inline static Input& Get() {
			static Input instance;
			return instance;
		};

	public:
		~Input();

	public:
		void SetEventCallback(const EventCallbackFunc& callback);

	public:
		inline static bool IsMouseLeftButtonUp();
		inline static bool IsMouseLeftButtonDown();

		inline static bool IsMouseMiddleButtonUp();
		inline static bool IsMouseMiddleButtonDown();

		inline static bool IsMouseRightButtonUp();
		inline static bool IsMouseRightButtonDown();

	public:
		inline static bool IsKeyUp(uint8 keycode);
		inline static bool IsKeyDown(uint8 keycode);

	public:
		static Keycode ConvertPlatformKeycode(uint16 keycode);
		const uchar* ConvertKeycodeToString(uint8 keycode);

	public:
		inline static Keyboard& GetKeyboard() { return Get().m_Keyboard; };

	private:
		Input();

	private:
		void Init();
		void Shutdown();

	private:
		//Mouse m_Mouse;
		Keyboard m_Keyboard;

		bool m_CallbackExists = false;
		EventCallbackFunc m_Callback;

	private:
		const uchar* m_Chars[PE_MAX_KEYCODE_COUNT];

	};

}

#define BEGIN_KEYCODE_CONVERTATION switch(keycode) {
#define END_KEYCODE_CONVERTATION default: return PE_UNKNOWN; }

#define ADD_KEYCODE(origKey, keycode) case origKey:		\
										{ return keycode; break;}