#include "Input.h"

namespace Pawn {

	String Input::ConvertKeycodeToString(uint8 keycode)
	{
		return m_Chars[keycode];
	}

	Input::Input()
	{
		Init();
	}

	Input::~Input()
	{
	}

	void Input::SetEventCallback(const EventCallbackFunc& callback)
	{
		m_Callback = callback; 
		m_CallbackExists = true;
	
		m_Keyboard.SetEventCallback(callback);
		m_Mouse.SetEventCallback(callback);
	}

	inline bool Input::IsMouseLeftButtonPressed()
	{
		return Get().GetMouse().IsLeftButtonPressed();
	}

	inline bool Input::IsMouseMiddleButtonPressed()
	{
		return Get().GetMouse().IsMiddleButtonPressed();
	}

	inline bool Input::IsMouseRightButtonPressed()
	{
		return Get().GetMouse().IsRightButtonPressed();
	}

	void Input::Init()
	{
		for (SIZE_T i = 0; i < PE_MAX_KEYCODE_COUNT; i++)
		{
			m_Chars[i] = TEXT("Unknown");
		}
	}

	inline Math::Vector2D32 Input::GetMousePosition()
	{
		return Get().GetMouse().GetPosition();
	}

	inline Math::Vector2D32 Input::GetMouseDelta()
	{
		return Get().GetMouse().GetMouseDelta();
	}

	bool Input::IsKeyUp(uint8 keycode)
	{
		return !Get().m_Keyboard.IsKeyHolded(keycode);
	}

	bool Input::IsKeyDown(uint8 keycode)
	{
		return Get().m_Keyboard.IsKeyHolded(keycode);
	}

}