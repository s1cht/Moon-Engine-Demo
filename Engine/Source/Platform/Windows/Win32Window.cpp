#include "pch.h"
#include "Win32Window.h"
#include "Win32Platform.h"
#include "Core/String/String.h"
#include "Core/Utils/Logging/Logger.h"
#include "Input/Input.h"
#include "Core/Events/WindowEvents.h"
#include "Core/Events/KeyEvents.h"
#include "Core/Events/MouseEvents.h"


namespace Pawn
{

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

	uint16 Win32Window::GetWidth()
	{
		return m_Data.WindowSize.X;
	}

	uint16 Win32Window::GetHeigth()
	{
		return m_Data.WindowSize.Y;
	}

	void Win32Window::OnUpdate()
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
		PE_INFO("Window creation begin");
		PE_INFO("Registering window class");
		WNDCLASSEXW wc;

		ZeroMemory(&wc, sizeof(wc));

		wc.cbSize = sizeof(wc);
		wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		wc.lpfnWndProc = WindowProc;
		wc.hInstance = GetModuleHandleW(NULL);

		wc.hCursor = LoadCursorW(NULL, IDC_NO);

		wc.lpszClassName = PE_WND_CLASSNAME;

		RegisterClassExW(&wc);

		PE_INFO("Creating window");
		m_Window = CreateWindowExW(
			0,                              // Optional window styles.
			PE_WND_CLASSNAME,                // Window class
			m_Data.WindowTitle,					// Window text
			WS_OVERLAPPEDWINDOW,            // Window style

			// Size and position
			CW_USEDEFAULT, CW_USEDEFAULT, 
			m_Data.WindowSize.X,
			m_Data.WindowSize.Y,
			NULL,       // Parent window    
			NULL,       // Menu
			NULL,
			NULL        // Additional application data
		);

		PE_CORE_ASSERT(m_Window, "Window creation failed! Error: {}", GetLastError());

		PE_INFO("Setting up window data");
		bool result = SetPropW(m_Window, L"WndData", &m_Data);
		PE_CORE_ASSERT(result && GetLastError(), "Window user pointer assign failed! Error: {}", GetLastError());
		
		ShowWindow(m_Window, 1);
		PE_INFO("Window creation end");
	}

	void Win32Window::Shutdown()
	{
		CloseWindow(m_Window);
	}

	Window* Window::Create(WindowProperties properties)
	{
		return new Win32Window(properties);
	}

	LRESULT Win32Window::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
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
				WindowFocusedEvent event;

				wndData->Focused = true;
				wndData->EventCallback(event);

				break;
			}
			case WM_KILLFOCUS:
			{
				WindowLostFocusEvent event;

				wndData->Focused = false;
				wndData->EventCallback(event);

				break;
			}
			case WM_SYSKEYDOWN:
			case WM_KEYDOWN:
			{
				int16 key = (int16)LOWORD(wParam);

				uint8 keyFlags = HIWORD(lParam);

				uint8 scanCode = LOBYTE(keyFlags);
				bool isExtendedKey = (keyFlags & KF_EXTENDED) == KF_EXTENDED;

				if (isExtendedKey)
					scanCode = MAKEWORD(scanCode, 0xE0);

				bool wasKeyDown = (keyFlags & KF_REPEAT) == KF_REPEAT;
				uint8 repeatCount = LOWORD(lParam);

				BOOL isKeyReleased = (keyFlags & KF_UP) == KF_UP;

				switch (key)
				{
				case VK_SHIFT:
				case VK_CONTROL:
				case VK_MENU:
					key = LOWORD(MapVirtualKeyW(scanCode, MAPVK_VSC_TO_VK_EX));
					break;
				}

				PE_INFO("Windows virtual key: {0}; Pawn virtual key {1}, Repeat count: {2}", key, (int16)Input::ConvertPlatformKeycode(key), repeatCount);

				break;
			}
			default:
				return DefWindowProc(hwnd, uMsg, wParam, lParam);
			}
		}
	}
}
