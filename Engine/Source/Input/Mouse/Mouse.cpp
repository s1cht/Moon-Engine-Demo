#include "Mouse.h"

namespace ME::Input::Devices
{
	Mouse::Mouse()
	{
		Init();
	}
	
	Mouse::~Mouse()
	{
	}
	
	void Mouse::SetLeftButtonPressed(bool pressed)
	{
		m_LeftButtonDown = pressed;

		if (pressed)
		{
			Events::MouseButtonPressedEvent event(PE_MOUSE_LEFTBUTTON);
			m_Callback(event);
		}
		else
		{
			Events::MouseButtonReleasedEvent event(PE_MOUSE_LEFTBUTTON);
			m_Callback(event);
		}
	}
	
	void Mouse::SetRightButtonPressed(bool pressed)
	{
		m_RightButtonDown = pressed;

		if (pressed)
		{
			Events::MouseButtonPressedEvent event(PE_MOUSE_RIGHTBUTTON);
			m_Callback(event);
		}
		else
		{
			Events::MouseButtonReleasedEvent event(PE_MOUSE_RIGHTBUTTON);
			m_Callback(event);
		}
	}

	void Mouse::SetMiddleButtonPressed(bool pressed)
	{
		m_MiddleButtonDown = pressed;

		if (pressed)
		{
			Events::MouseButtonPressedEvent event(PE_MOUSE_MIDDLEBUTTON);
			m_Callback(event);
		}
		else
		{
			Events::MouseButtonReleasedEvent event(PE_MOUSE_MIDDLEBUTTON);
			m_Callback(event);
		}
	}

	void Mouse::SetMousePosition(float32 x, float32 y)
	{
		SetMouseDelta(x, y);

		m_MousePosition.x = x;
		m_MousePosition.y = y;

		Events::MouseMovedEvent event(x, y, m_MouseDelta.x, m_MouseDelta.y);
		m_Callback(event);
	}

	void Mouse::SetSideButtonPressed(uint8 sideButton, bool pressed)
	{
		if (sideButton >= PE_MAX_MOUSE_SIDEBTN_COUNT)
		{
			ME_ERROR(TEXT("Unknown side button caught!")); 
			return;
		}

		m_SideButtonDown[sideButton] = pressed;

		if (pressed)
		{
			Events::MouseButtonPressedEvent event(sideButton + 3);
			m_Callback(event);
		}
		else
		{
			Events::MouseButtonReleasedEvent event(sideButton + 3);
			m_Callback(event);
		}

	}

	void Mouse::SetCursorPosition(float32 x, float32 y)
	{
#ifdef PLATFORM_WINDOWS
		SetCursorPos(static_cast<int32>(x), static_cast<int32>(y));
#endif

		m_MousePosition.x = x;
		m_MousePosition.y = y;
	}

	void Mouse::Init()
	{
		for (SIZE_T i = 0; i < PE_MAX_MOUSE_SIDEBTN_COUNT; i++)
		{
			m_SideButtonDown[i] = false;
		}
	}

	void Mouse::SetMouseDelta(float32 x, float32 y)
	{
		m_MouseDelta.x = x - m_MousePosition.x;
		m_MouseDelta.y = y - m_MousePosition.y;
	}
};