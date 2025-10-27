#include "Input.h"

namespace ME::Input
{
	ME::Core::String InputController::ConvertKeycodeToString(uint8 keycode)
	{
		return m_Chars[keycode];
	}

	InputController::InputController()
	{
		Init();
	}

	void InputController::SetEventCallback(const EventCallbackFunc& callback)
	{
		m_Callback = callback; 
		m_CallbackExists = true;
	
		m_Keyboard.SetEventCallback(callback);
		m_Mouse.SetEventCallback(callback);
	}

	inline bool InputController::IsMouseLeftButtonPressed()
	{
		return Get().GetMouse().IsLeftButtonPressed();
	}

	inline bool InputController::IsMouseMiddleButtonPressed()
	{
		return Get().GetMouse().IsMiddleButtonPressed();
	}

	inline bool InputController::IsMouseRightButtonDown()
	{
		return Get().GetMouse().IsRightButtonPressed();
	}

	void InputController::Init()
	{
		for (SIZE_T i = 0; i < ME_MAX_KEYCODE_COUNT; i++)
			m_Chars[i] = TEXT("Unknown");
	}

	inline ME::Core::Math::Vector2D32 InputController::GetMousePosition()
	{
		return Get().GetMouse().GetPosition();
	}

	inline ME::Core::Math::Vector2D32 InputController::GetMouseDelta()
	{
		return Get().GetMouse().GetMouseDelta();
	}

	bool InputController::IsKeyUp(uint8 keycode)
	{
		return !Get().m_Keyboard.IsKeyHolded(keycode);
	}

	bool InputController::IsKeyDown(uint8 keycode)
	{
		return Get().m_Keyboard.IsKeyHolded(keycode);
	}

}