#pragma once
#include <Core.h>
#include "Input/Input.h"

import Pawn.Core.Math;
import Pawn.Core.Container.Array;


// Base class for Window

namespace Pawn 
{
	struct PAWN_API WindowProperties
	{	
	public:
		Pawn::Core::Containers::String WindowTitle;
		Pawn::Core::Math::Vector2<uint16> WindowSize;

		WindowProperties(Pawn::Core::Containers::String title = TEXT("Pawn Engine"),
			Pawn::Core::Math::Vector2<uint16> size = Pawn::Core::Math::Vector2<uint16>(1280, 960)
		) : WindowTitle(title), WindowSize(size) {};
	};

	class PAWN_API Window
	{
	public:
		using EventCallbackFunc = std::function<void(Pawn::Core::Event&)>;

	protected:
		struct PAWN_API WindowData : WindowProperties
		{
			bool Focused;
			bool EventCallbackIsSetUp;

			EventCallbackFunc EventCallback;

			WindowData(const EventCallbackFunc& callback, Pawn::Input::InputController* inputHandler, const WindowProperties& props)
				: EventCallback(callback), WindowProperties(props), Focused(false), EventCallbackIsSetUp(true) {
			}
			WindowData(const WindowProperties& props)
				: WindowProperties(props), Focused(false), EventCallbackIsSetUp(false) {
			}
		};

	public:
		Window(const WindowProperties& properties) : m_Data(properties) {}
		virtual ~Window() {};

	public:
		virtual void SetEventCallback(const EventCallbackFunc& callback) = 0;

	public:
		virtual uint16 GetWidth() = 0;
		virtual uint16 GetHeight() = 0;
		virtual bool GetFocused() { return m_Data.Focused; }

	public:
		virtual void OnUpdate(float64 deltaTime) = 0;

	public:
		static Window* Create(WindowProperties props = WindowProperties());

	protected:
		WindowData m_Data;

	};

}
