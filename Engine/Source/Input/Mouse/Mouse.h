#pragma once

#include <Core.h>
#include <Core/Events/EventMacros.h>
#include <Core/Utils/Logging/Logger.h>

#include "Input/Keycodes.h"
#include "Events/MouseEvents.h"

import Pawn.Core.Event;
import Pawn.Core.Math;

namespace Pawn::Input::Devices
{
	class PAWN_API Mouse
	{
	public:
		EVENT_CALLBACK_FUNCTION;

	public:
		Mouse();
		~Mouse();

	public:
		void SetEventCallback(const EventCallbackFunc& callback) { m_Callback = callback; m_CallbackExists = true; }

	public:
		inline bool IsLeftButtonPressed()
		{
			return m_LeftButtonDown;
		}

		inline bool IsMiddleButtonPressed()
		{
			return m_MiddleButtonDown;
		}

		inline bool IsRightButtonPressed()
		{
			return m_RightButtonDown;
		}

		bool IsSideButtonDown(uint8 sideButton)
		{
			if (sideButton >= PE_MAX_MOUSE_SIDEBTN_COUNT)
			{
				PE_ERROR(TEXT("Unknown side button caught!"));
				return false;
			}
			return m_SideButtonDown[sideButton];
		};

		inline Pawn::Core::Math::Vector2D32 GetPosition()
		{
			return m_MousePosition;
		}

		inline Pawn::Core::Math::Vector2D32 GetMouseDelta()
		{
			return m_MouseDelta;
		}

		void GetPositionXY(float32& x, float32& y)
		{
			x = m_MousePosition.x;
			y = m_MousePosition.y;
		}

		void GetDeltaXY(float32& x, float32& y)
		{
			x = m_MouseDelta.x;
			y = m_MouseDelta.y;
		}

	public:
		void SetLeftButtonPressed(bool pressed);
		void SetRightButtonPressed(bool pressed);
		void SetMiddleButtonPressed(bool pressed);
		void SetMousePosition(float32 x, float32 y);
		void SetSideButtonPressed(uint8 sideButton, bool pressed);

	private:
		void Init();

		void SetMouseDelta(float32 x, float32 y);

	private:
		bool m_LeftButtonDown;
		bool m_MiddleButtonDown;
		bool m_RightButtonDown;

		bool m_SideButtonDown[PE_MAX_MOUSE_SIDEBTN_COUNT];

		Pawn::Core::Math::Vector2D32 m_MousePosition;
		Pawn::Core::Math::Vector2D32 m_MouseDelta;

	private:
		bool m_CallbackExists = false;
		EventCallbackFunc m_Callback;

	};

}