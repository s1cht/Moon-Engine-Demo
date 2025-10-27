#pragma once

#include <Core.hpp>
#include <Core/Math/Math.hpp>
#include <Core/Containers/Array.hpp>

#include "Input/Input.h"

// Base class for Window

namespace ME 
{
	struct MOON_API WindowProperties
	{	
	public:
		ME::Core::Containers::String WindowTitle;
		ME::Core::Math::Vector2D32 WindowSize;

		WindowProperties(ME::Core::Containers::String title = TEXT("Moon Engine"),
			ME::Core::Math::Vector2D32 size = ME::Core::Math::Vector2D32(1280.f, 960.f)
		) : WindowTitle(title), WindowSize(size) {};
	};

	class MOON_API Window
	{
	public:
		using EventCallbackFunc = std::function<void(ME::Core::Event&)>;

	protected:
		struct MOON_API WindowData : WindowProperties
		{
			bool Focused;
			bool EventCallbackIsSetUp;

			EventCallbackFunc EventCallback;

			WindowData(const EventCallbackFunc& callback, ME::Input::InputController* inputHandler, const WindowProperties& props)
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
		virtual const ME::Core::Containers::String& GetWindowName() const { return m_Data.WindowTitle; }

	public:
		virtual void OnUpdate(float64 deltaTime) = 0;

	public:
		static Window* Create(WindowProperties props = WindowProperties());

	protected:
		WindowData m_Data;

	};

}
