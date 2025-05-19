#pragma once

#include <Core.hpp>

#include <Core/Events/Event.hpp>
#include <Core/Math/Math.hpp>
#include <Core/Containers/String/String.hpp>

#include "Keycodes.h"
#include "Mouse/Mouse.h"
#include "Keyboard/Keyboard.h"

#include "Events/KeyEvents.h"
#include "Events/MouseEvents.h"

namespace Pawn::Input
{
	class PAWN_API InputController
	{
	public:
		using EventCallbackFunc = std::function<void(Pawn::Core::Event&)>;
	public:
		InputController(const InputController&) = delete;

		inline static InputController& Get() {
			static InputController instance;
			return instance;
		};

	public:
		~InputController();

	public:
		void SetEventCallback(const EventCallbackFunc& callback);

	public:
		inline static bool IsMouseLeftButtonPressed();
		inline static bool IsMouseMiddleButtonPressed();
		inline static bool IsMouseRightButtonPressed();

		inline static Pawn::Core::Math::Vector2D32 GetMousePosition();
		inline static Pawn::Core::Math::Vector2D32 GetMouseDelta();

	public:
		inline static bool IsKeyUp(uint8 keycode);
		inline static bool IsKeyDown(uint8 keycode);

	public:
		static Pawn::Input::Keycode ConvertPlatformKeycode(uint16 keycode);
		Pawn::Core::Containers::String ConvertKeycodeToString(uint8 keycode);

	public:
		inline static Pawn::Input::Devices::Keyboard& GetKeyboard() { return Get().m_Keyboard; };
		inline static Pawn::Input::Devices::Mouse& GetMouse() { return Get().m_Mouse; };

	private:
		InputController();

	private:
		void Init();

	private:
		Pawn::Input::Devices::Mouse m_Mouse;
		Pawn::Input::Devices::Keyboard m_Keyboard;

		bool m_CallbackExists = false;
		EventCallbackFunc m_Callback;

	private:
		Pawn::Core::Containers::String m_Chars[PE_MAX_KEYCODE_COUNT];

	};

}

#define BEGIN_KEYCODE_CONVERTATION switch(keycode) {
#define END_KEYCODE_CONVERTATION default: return PE_UNKNOWN; }

#define ADD_KEYCODE(origKey, keycode) case origKey:		\
										{ return keycode; break;}