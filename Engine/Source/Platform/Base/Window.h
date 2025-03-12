#pragma once
#include "Core.h"
#include "Input/Input.h"
#include "Core/Events/Event.h"
#include "Core/Containers/Array.h"
#include "Core/Math/Vector2D.h"
#include "Core/Containers/String.h"

// Base class for Window

// temporary until string implemented
typedef const uchar* WindowString;

namespace Pawn {

	struct PAWN_API WindowProperties
	{	
	public:
		WindowString WindowTitle;
		Math::Vector2<uint16> WindowSize;

		WindowProperties(WindowString title = TEXT("Pawn Engine"),
			Math::Vector2<uint16> size = Math::Vector2<uint16>(1280, 960)
		) : WindowTitle(title), WindowSize(size) {};
	};

	class PAWN_API Window
	{
	public:
		using EventCallbackFunc = std::function<void(Event&)>;

	protected:
		struct PAWN_API WindowData : WindowProperties
		{
			bool Focused;
			bool EventCallbackIsSetUp;

			EventCallbackFunc EventCallback;

			WindowData(const EventCallbackFunc& callback, Input* inputHandler, const WindowProperties& props)
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
		virtual void OnUpdate() = 0;

	public:
		static Window* Create(WindowProperties props = WindowProperties());

	protected:
		WindowData m_Data;

	};

}
