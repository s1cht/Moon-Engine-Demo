#pragma once

#include <Core.hpp>
#include "Platform/Base/Window.h"

namespace ME 
{
	class MOON_API Win32Window : public Window
	{
	public:
		Win32Window(const WindowProperties& properties);
		virtual ~Win32Window();

	public:
		virtual void SetEventCallback(const EventCallbackFunc& callback) override;

	public:
		virtual float32 GetWidth() const override;
		virtual float32 GetHeight() const override;

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