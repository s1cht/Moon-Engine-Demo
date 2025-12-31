#include <imgui.h>
#include <Core.hpp>
#include <Core/Utility/Logging/Logger.hpp>
#include <Core/Containers/String.hpp>

#include "Win32Window.hpp"
#include "Win32Platform.hpp"
#include "Input/Input.hpp"
#include "Input/Keyboard/KeyEvents.hpp"
#include "Input/Mouse/MouseEvents.hpp"
#include "Platform/Base/WindowEvents.hpp"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace ME
{
#if defined(PLATFORM_WINDOWS)
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
		WNDCLASSEXA wc;

		ZeroMemory(&wc, sizeof(wc));

		wc.cbSize = sizeof(wc);
		wc.style = CS_CLASSDC | CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = WindowProc;
		wc.hInstance = GetModuleHandleA(nullptr);

		wc.hCursor = LoadCursorW(nullptr, IDC_NO);

		wc.lpszClassName = ME_WND_CLASSNAME;

		RegisterClassExA(&wc);

		RECT windowViewport = { 0, 0, static_cast<LONG>(m_Data.WindowSize.X), static_cast<LONG>(m_Data.WindowSize.Y) };
		AdjustWindowRect(&windowViewport, WS_OVERLAPPEDWINDOW, FALSE);

		m_Window = CreateWindowExA(
			0,													// Optional window styles.
			ME_WND_CLASSNAME,									// Window class
			CONVERT_TEXT(m_Data.WindowTitle.String()),			// Window text
			WS_OVERLAPPEDWINDOW,								// Window style
																//
			// Size and position								//
			CW_USEDEFAULT, CW_USEDEFAULT,						//
			windowViewport.right - windowViewport.left,			//
			windowViewport.bottom - windowViewport.top,			//
			nullptr,											// Parent window    
			nullptr,											// Menu
			wc.hInstance,										// 
			nullptr												// Additional application data
		);

		ME_CORE_ASSERT(m_Window, "Window creation failed! Error: {}", GetLastError());

		bool result = SetPropA(m_Window, "WndData", &m_Data);
		ME_CORE_ASSERT(result, "Window user pointer assign failed! Error: {}", GetLastError());
		
		ShowWindow(m_Window, 1);
	}

	void Win32Window::Shutdown()
	{
		CloseWindow(m_Window);
	}

	LRESULT Win32Window::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		ImGuiContext* imguiContext = ImGui::GetCurrentContext();
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
				if (viewport && viewport->PlatformHandle && static_cast<HWND>(viewport->PlatformHandle) == hwnd)
					if (ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam))
						return true;
			}
		}
		else
		{
			if (ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam))
				return true;
		}

		WindowData* wndData = static_cast<WindowData*>(GetPropW(hwnd, L"WndData"));

		switch (uMsg)
		{
			case WM_PAINT:
			{
				PAINTSTRUCT ps;
				HDC hdc = BeginPaint(hwnd, &ps);
				FillRect(hdc, &ps.rcPaint, reinterpret_cast<HBRUSH>((COLOR_WINDOW + 1)));
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
				WORD key = static_cast<int16>(LOWORD(wParam));
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

				Input::InputController::Get().GetKeyboard().SetKeyPressed(static_cast<uint8>(Input::InputController::ConvertPlatformKeycode(key)), true);

				break;
			}
			case WM_SYSKEYUP:
			case WM_KEYUP:
			{
				WORD key = static_cast<int16>(LOWORD(wParam));
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

				Input::InputController::Get().GetKeyboard().SetKeyPressed(static_cast<uint8>(Input::InputController::ConvertPlatformKeycode(key)), false);

				break;
			}
			case WM_MOUSEMOVE:
			{
				Input::InputController::Get().GetMouse().SetMousePosition(static_cast<float32>(GET_X_LPARAM(lParam)), static_cast<float32>(GET_Y_LPARAM(lParam)));
				break;
			}
			case WM_LBUTTONUP:
			{
				Input::InputController::Get().GetMouse().SetLeftButtonPressed(false);
				break;
			}
			case WM_LBUTTONDOWN:
			{
				Input::InputController::Get().GetMouse().SetLeftButtonPressed(true);
				break;
			}
			case WM_RBUTTONUP:
			{
				Input::InputController::Get().GetMouse().SetRightButtonPressed(false);
				break;
			}
			case WM_RBUTTONDOWN:
			{
				Input::InputController::Get().GetMouse().SetRightButtonPressed(true);
				break;
			}
			case WM_MBUTTONUP:
			{
				Input::InputController::Get().GetMouse().SetMiddleButtonPressed(false);
				break;
			}
			case WM_MBUTTONDOWN:
			{
				Input::InputController::Get().GetMouse().SetMiddleButtonPressed(true);
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
#endif
}
