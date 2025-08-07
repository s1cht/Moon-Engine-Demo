#include <imgui.h>

#include "Win32Window.h"
#include "Win32Platform.h"

#include "Input/Input.h"

#include "Events/WindowEvents.h"
#include "Events/KeyEvents.h"
#include "Events/MouseEvents.h"

#include <Core.hpp>
#include <Core/Utils/Logging/Logger.hpp>
#include <Core/Containers/String/String.hpp>
#include <Core/Utils/Benchmark/Benchmark.hpp>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace ME
{
	Window* Window::Create(WindowProperties properties)
	{
		return new Win32Window(properties);
	}

	Win32Window::Win32Window(const WindowProperties& properties)
		: Window(properties)
	{
		Init();
	}

	Win32Window::~Win32Window()
	{
		Shutdown();
	}

	void Win32Window::SetEventCallback(const EventCallbackFunc& callback)
	{
		m_Data.EventCallback = callback;
		m_Data.EventCallbackIsSetUp = true;
	}

	float32 Win32Window::GetWidth() const
	{
		return m_Data.WindowSize.X;
	}

	float32 Win32Window::GetHeight() const
	{
		return m_Data.WindowSize.Y;
	}

	void Win32Window::OnUpdate(float64 deltaTime)
{
		MSG message = {};

		while (PeekMessageW(&message, m_Window, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
	}

	void Win32Window::Init()
	{
		WNDCLASSEXW wc;

		ZeroMemory(&wc, sizeof(wc));

		wc.cbSize = sizeof(wc);
		wc.style = CS_CLASSDC | CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = WindowProc;
		wc.hInstance = GetModuleHandleW(NULL);

		wc.hCursor = LoadCursorW(NULL, IDC_NO);

		wc.lpszClassName = PE_WND_CLASSNAME;

		RegisterClassExW(&wc);

		RECT windowViewport = { 0, 0, (LONG)m_Data.WindowSize.X, (LONG)m_Data.WindowSize.Y };
		AdjustWindowRect(&windowViewport, WS_OVERLAPPEDWINDOW, FALSE);

		m_Window = CreateWindowExW(
			0,													// Optional window styles.
			PE_WND_CLASSNAME,									// Window class
			m_Data.WindowTitle.GetString(),						// Window text
			WS_OVERLAPPEDWINDOW,								// Window style
																//
			// Size and position								//
			CW_USEDEFAULT, CW_USEDEFAULT,						//
			windowViewport.right - windowViewport.left,			//
			windowViewport.bottom - windowViewport.top,			//
			NULL,												// Parent window    
			NULL,												// Menu
			wc.hInstance,										// 
			NULL												// Additional application data
		);

		ME_CORE_ASSERT(m_Window, "Window creation failed! Error: {}", GetLastError());

		bool result = SetPropW(m_Window, L"WndData", &m_Data);
		ME_CORE_ASSERT(result && GetLastError(), "Window user pointer assign failed! Error: {}", GetLastError());
		
		ShowWindow(m_Window, 1);
	}

	void Win32Window::Shutdown()
	{
		CloseWindow(m_Window);
	}

	LRESULT Win32Window::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		/*ImGuiContext* imguiContext = ImGui::GetCurrentContext();
		if (!imguiContext)
		{
			return DefWindowProc(hwnd, uMsg, wParam, lParam);
		}

		ImGuiIO& io = ImGui::GetIO();

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();
			for (int i = 0; i < platform_io.Viewports.Size; i++)
			{
				ImGuiViewport* viewport = platform_io.Viewports[i];
				if (viewport && viewport->PlatformHandle && (HWND)viewport->PlatformHandle == hwnd)
				{
					if (ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam))
					{
						return true;
					}
				}
			}
		}
		else
		{
			if (ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam))
				return true;
		}*/

		WindowData* wndData = static_cast<WindowData*>(GetPropW(hwnd, L"WndData"));

		switch (uMsg)
		{
			case WM_PAINT:
			{
				PAINTSTRUCT ps;
				HDC hdc = BeginPaint(hwnd, &ps);
				FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
				EndPaint(hwnd, &ps);
				break;
			}
		}

		if (!wndData) return DefWindowProc(hwnd, uMsg, wParam, lParam);

		if (wndData->EventCallbackIsSetUp) 
		{
			switch (uMsg)
			{
				case WM_SETFOCUS:
				{
					ME::Events::WindowFocusedEvent event;

					wndData->Focused = true;
					wndData->EventCallback(event);

					break;
				}
				case WM_KILLFOCUS:
				{
					ME::Events::WindowLostFocusEvent event;

					wndData->Focused = false;
					wndData->EventCallback(event);

					break;
				}
				case WM_SIZE:
				{
					wndData->WindowSize.X = LOWORD(lParam);
					wndData->WindowSize.Y = HIWORD(lParam);

					ME::Events::WindowResizedEvent event((float32)wndData->WindowSize.X, (float32)wndData->WindowSize.Y);
					wndData->EventCallback(event);

					break;
				}
				case WM_CLOSE:
				{
					ME::Events::WindowClosedEvent event;
					wndData->EventCallback(event);

					break;
				}
			}
		}
	
		switch (uMsg)
		{
			case WM_SYSKEYDOWN:
			case WM_KEYDOWN:
			{
				static int8 repeatCount = 0;
				WORD key = (int16)LOWORD(wParam);
				WORD keyFlags = HIWORD(lParam);
				WORD scanCode = LOBYTE(keyFlags);
				bool isExtendedKey = (keyFlags & KF_EXTENDED) == KF_EXTENDED;
				bool wasKeyDown = (keyFlags & KF_REPEAT) == KF_REPEAT;

				if (isExtendedKey)
					scanCode = MAKEWORD(scanCode, 0xE0);

				switch (key)
				{
				case VK_SHIFT:
				case VK_CONTROL:
				case VK_MENU:
					key = LOWORD(MapVirtualKeyW(scanCode, MAPVK_VSC_TO_VK_EX));
					break;
				}

				Input::InputController::Get().GetKeyboard().SetKeyPressed((uint8)Input::InputController::ConvertPlatformKeycode(key), true);

				break;
			}
			case WM_SYSKEYUP:
			case WM_KEYUP:
			{
				WORD key = (int16)LOWORD(wParam);
				WORD keyFlags = HIWORD(lParam);
				WORD scanCode = LOBYTE(keyFlags);
				bool isExtendedKey = (keyFlags & KF_EXTENDED) == KF_EXTENDED;

				if (isExtendedKey)
					scanCode = MAKEWORD(scanCode, 0xE0);

				switch (key)
				{
				case VK_SHIFT:
				case VK_CONTROL:
				case VK_MENU:
					key = LOWORD(MapVirtualKeyW(scanCode, MAPVK_VSC_TO_VK_EX));
					break;
				}

				Input::InputController::Get().GetKeyboard().SetKeyPressed((uint8)Input::InputController::ConvertPlatformKeycode(key), false);

				break;
			}
			case WM_MOUSEMOVE:
			{
				Input::InputController::Get().GetMouse().SetMousePosition((float32)GET_X_LPARAM(lParam), (float32)GET_Y_LPARAM(lParam));
				break;
			}

			//case WM_INPUT:
			//{
			//	static RAWINPUT* raw;
			//	static uint32 rawInputSize = 0;
			//
			//	uint32 size = 0;
			//	HRAWINPUT rawInputHeader = (HRAWINPUT)lParam;
			//
			//	GetRawInputData(rawInputHeader, RID_INPUT, NULL, &size, sizeof(RAWINPUTHEADER));
			//
			//	if (size > rawInputSize)
			//	{
			//		::operator delete[](raw);
			//		//raw = memset();
			//	}
			//
			//}
		}

		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}
}
