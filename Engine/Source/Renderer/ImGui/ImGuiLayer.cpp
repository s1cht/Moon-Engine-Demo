#include "ImGuiLayer.h"
#include "ImGuiReferences.h"
#include "Renderer/Renderer.h"
#include "Application/Application.h"

#include <Core/Misc/Time.hpp>

#include "Renderer/API/Vulkan/VulkanRenderAPI.h"

namespace ME::Render::Imgui
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
		RenderAPI::API renderAPI = Renderer::GetRenderAPI();
		if (renderAPI == RenderAPI::API::None)
		{
			ME_INFO(TEXT("Disabling ImGui layer, because RenderAPI is ::None"));
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
			case RenderAPI::API::Vulkan:
			{
				m_ResourceHandler = 


				ImGui_ImplVulkan_InitInfo info = {};
				info.Instance = RenderCommand::Get()->As<VulkanRenderAPI>()->GetInstance();
				info.PhysicalDevice = RenderCommand::Get()->As<VulkanRenderAPI>()->GetPhysicalDevice();
				info.Device = RenderCommand::Get()->As<VulkanRenderAPI>()->GetDevice();
				info.Queue = RenderCommand::Get()->As<VulkanRenderAPI>()->GetGraphicsQueue();
				info.ImageCount = RenderCommand::Get()->GetSwapChain()->GetFrameCount();
				info.DescriptorPool = imguiPool;
				info.Allocator = nullptr;
				info.ApiVersion = VK_API_VERSION_1_4;
				info.MSAASamples = VK_SAMPLE_COUNT_4_BIT;

				ImGui_ImplVulkan_Init();
			}
#ifdef PLATFORM_WINDOWS
			case RenderAPI::API::DirectX12: ME_ASSERT(false, TEXT("RenderAPI::DirectX12 is unsupported")); break;
#elif PLATFORM_MAC
			case RenderAPI::API::Metal: ME_ASSERT(false, TEXT("RenderAPI::Metal is unsupported")); break;
#endif
		}

#ifdef PLATFORM_WINDOWS
		ImGui_ImplWin32_Init(static_cast<Win32Window*>(Application::Get().GetWindow())->GetWindowHandle());
#endif
	}

	void ImGuiLayer::OnDetach()
	{
		if (m_Disabled)
			return;

		Shutdown();
	}

	void ImGuiLayer::OnEvent(ME::Core::Event& event)
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
			ImGui::Text("A FPS: %d", ME::Core::Clock::Time::GetAverageFPS());
			ImGui::Text("I FPS: %d", ME::Core::Clock::Time::GetInstantFPS());
		}
		ImGui::End();
	}

	void ImGuiLayer::BeginRender()
	{
		RenderAPI::API renderAPI;
		if (m_Disabled)
			return;

		ImGui::SetCurrentContext(m_ImGuiContext);
		renderAPI = Renderer::GetRenderAPI();

		switch (renderAPI)
		{
			case RenderAPI::API::Vulkan:
			{
				
				break;
			}
#ifdef PLATFORM_WINDOWS
		case RenderAPI::API::DirectX12: ME_ASSERT(false, TEXT("RenderAPI::DirectX12 is unsupported")); break;
#elif PLATFORM_MAC
		case RenderAPI::API::Metal: ME_ASSERT(false, TEXT("RenderAPI::Metal is unsupported")); break;
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
	}

	void ImGuiLayer::Shutdown()
	{
		RenderAPI::API renderAPI;
		if (m_Disabled)
			return;

		ImGui::SetCurrentContext(m_ImGuiContext);
		renderAPI = Renderer::GetRenderAPI();

		switch (renderAPI)
		{
		case RenderAPI::API::Vulkan: ME_ASSERT(false, TEXT("RenderAPI::Vulkan is unsupported")); break;
#ifdef PLATFORM_WINDOWS
		case RenderAPI::API::DirectX12: ME_ASSERT(false, TEXT("RenderAPI::DirectX12 is unsupported")); break;
#elif PLATFORM_MAC
		case RenderAPI::API::Metal: ME_ASSERT(false, TEXT("RenderAPI::Metal is unsupported")); break;
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
