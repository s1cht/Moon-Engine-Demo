#pragma once

#include "Platform/Base/Window.h"
#include "Core/Platform/Platform.h"

namespace Pawn {

	class PAWN_API Win32Window : public Window
	{

	public:
		Win32Window(const WindowProperties& properties);
		virtual ~Win32Window();

	public:
		virtual void SetEventCallback(const EventCallbackFunc& callback) override;

	public:
		virtual uint16 GetWidth() override;
		virtual uint16 GetHeight() override;

		inline HWND GetWindowHandle() { return m_Window; };

	public:
		virtual void OnUpdate(float64 deltaTime) override;

	private:
		void Init();
		void Shutdown();

		static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	private:
		HWND m_Window;

	};

}