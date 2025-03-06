#include "pch.h"
#include "Input.h"

namespace Pawn {

	const uchar* Input::ConvertKeycodeToString(uint8 keycode)
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
		//m_Mouse.SetEventCallback(callback);
	}

	void Input::Init()
	{
		for (SIZE_T i = 0; i < PE_MAX_KEYCODE_COUNT; i++)
		{
			m_Chars[i] = TEXT("Unknown");
		}
	}

	void Input::Shutdown()
	{
		
	}

	bool Input::IsMouseLeftButtonUp()
	{
		return false;
	}

	bool Input::IsMouseLeftButtonDown()
	{
		return false;
	}

	bool Input::IsMouseMiddleButtonUp()
	{
		return false;
	}

	bool Input::IsMouseMiddleButtonDown()
	{
		return false;
	}

	bool Input::IsMouseRightButtonUp()
	{
		return false;
	}

	bool Input::IsMouseRightButtonDown()
	{
		return false;
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