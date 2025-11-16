#pragma once

#include <Core.hpp>
#include <Core/Events/EventMacros.hpp>
#include <Core/Utility/Logging/Logger.hpp>
#include <Core/Events/Event.hpp>
#include <Core/Math/Math.hpp>

#include "MouseEvents.hpp"
#include "Input/Keycodes.hpp"

namespace ME::Input::Devices
{
	class MEAPI Mouse
	{
	public:
		EVENT_CALLBACK_FUNCTION;

	public:
		Mouse();
		~Mouse() = default;

	public:
		void SetEventCallback(const EventCallbackFunc& callback) { m_Callback = callback; m_CallbackExists = true; }

	public:
		inline bool IsLeftButtonPressed() const
		{
			return m_LeftButtonDown;
		}

		inline bool IsMiddleButtonPressed() const
		{
			return m_MiddleButtonDown;
		}

		inline bool IsRightButtonPressed() const
		{
			return m_RightButtonDown;
		}

		bool IsSideButtonDown(uint8 sideButton) const
		{
			if (sideButton >= ME_MAX_MOUSE_SIDEBTN_COUNT)
			{
				ME_ERROR("Unknown side button caught!");
				return false;
			}
			return m_SideButtonDown[sideButton];
		};

		inline ME::Core::Math::Vector2D GetPosition() const
		{
			return m_MousePosition;
		}

		inline ME::Core::Math::Vector2D GetMouseDelta() const
		{
			return m_MouseDelta;
		}

		void GetPositionXY(float32& x, float32& y) const
		{
			x = m_MousePosition.x;
			y = m_MousePosition.y;
		}

		void GetDeltaXY(float32& x, float32& y) const
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
		void SetCursorPosition(float32 x, float32 y);

	private:
		void Init();
		void SetMouseDelta(float32 x, float32 y);

	private:
		bool m_LeftButtonDown;
		bool m_MiddleButtonDown;
		bool m_RightButtonDown;

		bool m_SideButtonDown[ME_MAX_MOUSE_SIDEBTN_COUNT];

		ME::Core::Math::Vector2D m_MousePosition;
		ME::Core::Math::Vector2D m_MouseDelta;

	private:
		bool m_CallbackExists = false;
		EventCallbackFunc m_Callback;
	};

}