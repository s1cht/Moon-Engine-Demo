#include "ImGuiLayer.h"
#include "ImGuiReferences.h"
#include "Renderer/Renderer.h"
#include "Application/Application.h"

#include <Core/Misc/Time.hpp>

namespace Pawn::Render::Imgui
{
	ImGuiLayer::ImGuiLayer()
		: Layer(TEXT("ImGuiLayer"))
	{
	}

	ImGuiLayer::~ImGuiLayer()
	{
	}

	void ImGuiLayer::OnAttach()
	{
		RendererAPI::API renderAPI = Renderer::GetRenderAPI();
		if (renderAPI == RendererAPI::API::None)
		{
			PE_INFO(TEXT("Disabling ImGui layer, because RenderAPI is ::None"));
			m_Disabled = true;
			return;
		}
		m_ImGuiContext = ImGui::CreateContext();
		ImGui::StyleColorsDark();

		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
		io.BackendFlags |= ImGuiBackendFlags_HasMouseHoveredViewport;
		io.BackendFlags |= ImGuiBackendFlags_RendererHasViewports;
	
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		switch (renderAPI)
		{
			case RendererAPI::API::Vulkan: PE_ASSERT(false, TEXT("RenderAPI::Vulkan is unsupported")); break;
#ifdef PLATFORM_WINDOWS
			case RendererAPI::API::DirectX11:
			{
				Win32Window& window = *static_cast<Win32Window*>(Application::Get().GetWindow());
				DirectX11Renderer& renderer = *static_cast<DirectX11Renderer*>(RenderCommand::Get());

				ImGui_ImplWin32_Init(window.GetWindowHandle());
				ImGui_ImplDX11_Init(renderer.GetDevice(), renderer.GetDeviceContext());

				break;
			};
			case RendererAPI::API::DirectX12: PE_ASSERT(false, TEXT("RenderAPI::DirectX12 is unsupported")); break;
#elif PLATFORM_MAC
			case RendererAPI::API::Metal: PE_ASSERT(false, TEXT("RenderAPI::Metal is unsupported")); break;
#endif
		}
	}

	void ImGuiLayer::OnDetach()
	{
		if (m_Disabled)
			return;

		Shutdown();
	}

	void ImGuiLayer::OnEvent(Pawn::Core::Event& event)
	{
		if (m_EnabledEvents)
		{
			ImGuiIO& io = ImGui::GetIO();
			event.IsHandled |= event.IsInCategory(EventCategory_Mouse) & io.WantCaptureMouse;
			event.IsHandled |= event.IsInCategory(EventCategory_Keyboard) & io.WantCaptureKeyboard;
		}
	}

	void ImGuiLayer::OnImGuiRender(float64 deltaTime, ImGuiContext*)
	{
		if (m_Disabled)
			return;

		ImGui::SetCurrentContext(m_ImGuiContext);
		static bool visible = true;

		//ImGui::ShowDemoWindow(&visible);

		if (ImGui::Begin("DeltaTime", &visible))
		{
			ImGui::Text("%.5f", deltaTime);
			ImGui::Text("A FPS: %d", Pawn::Core::Clock::Time::GetAverageFPS());
			ImGui::Text("I FPS: %d", Pawn::Core::Clock::Time::GetInstantFPS());
		}
		ImGui::End();
	}

	void ImGuiLayer::BeginRender()
	{
		RendererAPI::API renderAPI;
		if (m_Disabled)
			return;

		ImGui::SetCurrentContext(m_ImGuiContext);
		renderAPI = Renderer::GetRenderAPI();

		switch (renderAPI)
		{
		case RendererAPI::API::Vulkan: PE_ASSERT(false, TEXT("RenderAPI::Vulkan is unsupported")); break;
#ifdef PLATFORM_WINDOWS
		case RendererAPI::API::DirectX11:
		{
			ImGui_ImplDX11_NewFrame();
			break;
		};
		case RendererAPI::API::DirectX12: PE_ASSERT(false, TEXT("RenderAPI::DirectX12 is unsupported")); break;
#elif PLATFORM_MAC
		case RendererAPI::API::Metal: PE_ASSERT(false, TEXT("RenderAPI::Metal is unsupported")); break;
#endif
		}

#ifdef PLATFORM_WINDOWS
		ImGui_ImplWin32_NewFrame();
#endif
		ImGui::NewFrame();
	}

	void ImGuiLayer::EndRender()
	{
		if (m_Disabled)
			return;

		ImGui::SetCurrentContext(m_ImGuiContext);
		ImGui::Render();
#ifdef PLATFORM_WINDOWS
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#endif 
	}

	void ImGuiLayer::Shutdown()
	{
		RendererAPI::API renderAPI;
		if (m_Disabled)
			return;

		ImGui::SetCurrentContext(m_ImGuiContext);
		renderAPI = Renderer::GetRenderAPI();

		switch (renderAPI)
		{
		case RendererAPI::API::Vulkan: PE_ASSERT(false, TEXT("RenderAPI::Vulkan is unsupported")); break;
#ifdef PLATFORM_WINDOWS
		case RendererAPI::API::DirectX11:
		{
			ImGui_ImplDX11_Shutdown();
			break;
		};
		case RendererAPI::API::DirectX12: PE_ASSERT(false, TEXT("RenderAPI::DirectX12 is unsupported")); break;
#elif PLATFORM_MAC
		case RendererAPI::API::Metal: PE_ASSERT(false, TEXT("RenderAPI::Metal is unsupported")); break;
#endif
		}

#ifdef PLATFORM_WINDOWS
		ImGui_ImplWin32_Shutdown();
#endif

		ImGui::DestroyContext(m_ImGuiContext);
		m_ImGuiContext = nullptr;
	}

	void ImGuiLayer::PostRender()
	{
#ifdef PLATFORM_WINDOWS
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}
#endif
	}

	ImGuiContext* ImGuiLayer::GetContext()
	{
		return m_ImGuiContext;
	}

}