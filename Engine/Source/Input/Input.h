#pragma once
#include "pch.h"
#include "Keycodes.h"
#include "Mouse/Mouse.h"
#include "Keyboard/Keyboard.h"
#include "Core.h"
#include "Core/Math/Vector2D.h"
#include "Core/Events/Event.h"
#include "Events/KeyEvents.h"
#include "Events/MouseEvents.h"

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
		inline static bool IsMouseLeftButtonPressed();
		inline static bool IsMouseMiddleButtonPressed();
		inline static bool IsMouseRightButtonPressed();

		inline static Math::Vector2D32 GetMousePosition();
		inline static Math::Vector2D32 GetMouseDelta();

	public:
		inline static bool IsKeyUp(uint8 keycode);
		inline static bool IsKeyDown(uint8 keycode);

	public:
		static Keycode ConvertPlatformKeycode(uint16 keycode);
		String ConvertKeycodeToString(uint8 keycode);

	public:
		inline static Keyboard& GetKeyboard() { return Get().m_Keyboard; };
		inline static Mouse& GetMouse() { return Get().m_Mouse; };

	private:
		Input();

	private:
		void Init();

	private:
		Mouse m_Mouse;
		Keyboard m_Keyboard;

		bool m_CallbackExists = false;
		EventCallbackFunc m_Callback;

	private:
		String m_Chars[PE_MAX_KEYCODE_COUNT];

	};

}

#define BEGIN_KEYCODE_CONVERTATION switch(keycode) {
#define END_KEYCODE_CONVERTATION default: return PE_UNKNOWN; }

#define ADD_KEYCODE(origKey, keycode) case origKey:		\
										{ return keycode; break;}