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
		Pawn::Core::Math::Vector2D32 WindowSize;

		WindowProperties(Pawn::Core::Containers::String title = TEXT("Pawn Engine"),
			Pawn::Core::Math::Vector2D32 size = Pawn::Core::Math::Vector2D32(1280.f, 960.f)
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
		virtual float32 GetWidth() const = 0;
		virtual float32 GetHeight() const = 0;
		virtual bool GetFocused() const { return m_Data.Focused; }
		virtual const Pawn::Core::Containers::String& GetWindowName() const { return m_Data.WindowTitle; }

	public:
		virtual void OnUpdate(float64 deltaTime) = 0;

	public:
		static Window* Create(WindowProperties props = WindowProperties());

	protected:
		WindowData m_Data;

	};

}
