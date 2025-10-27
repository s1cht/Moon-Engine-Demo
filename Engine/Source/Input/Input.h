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

namespace ME::Input
{
	class MEAPI InputController
	{
	public:
		using EventCallbackFunc = std::function<void(ME::Core::Event&)>;
	public:
		InputController(const InputController&) = delete;

		inline static InputController& Get() {
			static InputController instance;
			return instance;
		};

	public:
		~InputController() = default;

	public:
		void SetEventCallback(const EventCallbackFunc& callback);

	public:
		inline static bool IsMouseLeftButtonPressed();
		inline static bool IsMouseMiddleButtonPressed();
		inline static bool IsMouseRightButtonDown();

		inline static ME::Core::Math::Vector2D32 GetMousePosition();
		inline static ME::Core::Math::Vector2D32 GetMouseDelta();

	public:
		inline static bool IsKeyUp(uint8 keycode);
		inline static bool IsKeyDown(uint8 keycode);

	public:
		static ME::Input::Keycode ConvertPlatformKeycode(uint16 keycode);
		ME::Core::String ConvertKeycodeToString(uint8 keycode);

	public:
		inline static ME::Input::Devices::Keyboard& GetKeyboard() { return Get().m_Keyboard; };
		inline static ME::Input::Devices::Mouse& GetMouse() { return Get().m_Mouse; };

	private:
		InputController();

	private:
		void Init();

	private:
		ME::Input::Devices::Mouse m_Mouse;
		ME::Input::Devices::Keyboard m_Keyboard;

		bool m_CallbackExists = false;
		EventCallbackFunc m_Callback;

	private:
		ME::Core::String m_Chars[ME_MAX_KEYCODE_COUNT];

	};

}

#define BEGIN_KEYCODE_CONVERTATION switch(keycode) {
#define END_KEYCODE_CONVERTATION default: return ME_UNKNOWN; }

#define ADD_KEYCODE(origKey, keycode) case origKey:		\
										{ return keycode; break;}