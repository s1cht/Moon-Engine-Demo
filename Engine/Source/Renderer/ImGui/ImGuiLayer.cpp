#include "ImGuiLayer.hpp"
#include <Core/Time.hpp>

#include "ImGuiReferences.hpp"
#include "Application/Application.hpp"
#include "Renderer/Renderer.hpp"
#include "Renderer/API/Vulkan/VulkanCommandBuffer.hpp"
#include "Renderer/API/Vulkan/VulkanRenderAPI.hpp"
#include "Renderer/API/Vulkan/VulkanRenderPass.hpp"
#include "Renderer/API/Vulkan/VulkanResourceHandler.hpp"

namespace ME::Render::ImGui
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
			ME_INFO("Disabling ImGui layer, because RenderAPI is ::None");
			m_Disabled = true;
			return;
		}
		m_ImGuiContext = ::ImGui::CreateContext();
		::ImGui::StyleColorsDark();

		ImGuiIO& io = ::ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
		io.BackendFlags |= ImGuiBackendFlags_HasMouseHoveredViewport;
		io.BackendFlags |= ImGuiBackendFlags_RendererHasViewports;
	
		ImGuiStyle& style = ::ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		m_ResourceHandler = Render::ResourceHandler::Create(RenderCommand::Get()->GetSwapChain()->GetFrameCount());
		CreateRenderResources();

		switch (renderAPI)
		{
			case RenderAPI::API::Vulkan:
			{
				ImGui_ImplVulkan_InitInfo info = {};
				info.Instance = RenderCommand::Get()->As<VulkanRenderAPI>()->GetInstance();
				info.PhysicalDevice = RenderCommand::Get()->As<VulkanRenderAPI>()->GetPhysicalDevice();
				info.Device = RenderCommand::Get()->As<VulkanRenderAPI>()->GetDevice();
				info.Queue = RenderCommand::Get()->As<VulkanRenderAPI>()->GetGraphicsQueue();
				info.DescriptorPool = m_ResourceHandler->As<VulkanResourceHandler>()->GetDescriptorPool();
				info.Allocator = nullptr;
				info.RenderPass = m_Pass->As<VulkanRenderPass>()->GetRenderPass();
				info.Subpass = 0;
				info.ImageCount = 2;
				info.MinImageCount = 2;
				info.ApiVersion = VK_API_VERSION_1_4;
				info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

				ImGui_ImplVulkan_Init(&info);

				break;
			}
#ifdef PLATFORM_WINDOWS
			case RenderAPI::API::DirectX12: ME_ASSERT(false, "RenderAPI::DirectX12 is unsupported"); break;
        default: break;
#elif PLATFORM_MAC
			case RenderAPI::API::Metal: ME_ASSERT(false, "RenderAPI::Metal is unsupported"); break;
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
			ImGuiIO& io = ::ImGui::GetIO();
			event.IsHandled |= event.IsInCategory(EventCategory_Mouse) & io.WantCaptureMouse;
			event.IsHandled |= event.IsInCategory(EventCategory_Keyboard) & io.WantCaptureKeyboard;
		}
	}

	void ImGuiLayer::OnImGuiRender(float64 deltaTime, ImGuiContext*)
	{
		if (m_Disabled)
			return;

		::ImGui::SetCurrentContext(m_ImGuiContext);
		static bool visible = true;

		//::ImGui::ShowDemoWindow(&visible);

		if (::ImGui::Begin("DeltaTime", &visible))
		{
			::ImGui::Text("%.7f", deltaTime);
			::ImGui::Text("A FPS: %d", ME::Core::Clock::Time::GetAverageFPS());
			::ImGui::Text("I FPS: %d", ME::Core::Clock::Time::GetInstantFPS());
			::ImGui::Text("67");
		}
		::ImGui::End();
	}

	void ImGuiLayer::BeginRender()
	{
		RenderAPI::API renderAPI;
		if (m_Disabled)
			return;

		::ImGui::SetCurrentContext(m_ImGuiContext);
		renderAPI = Renderer::GetRenderAPI();

		switch (renderAPI)
		{
			case RenderAPI::API::Vulkan:
			{
				ImGui_ImplVulkan_NewFrame();
				break;
			}
#ifdef PLATFORM_WINDOWS
		case RenderAPI::API::DirectX12: ME_ASSERT(false, "RenderAPI::DirectX12 is unsupported"); break;
#elif PLATFORM_MAC
		case RenderAPI::API::Metal: ME_ASSERT(false, "RenderAPI::Metal is unsupported"); break;
#endif
		}

#ifdef PLATFORM_WINDOWS
		ImGui_ImplWin32_NewFrame();
#endif
		::ImGui::NewFrame();
	}

	void ImGuiLayer::EndRender(const ME::Core::Memory::Reference<ME::Render::CommandBuffer>& commandBuffer)
	{
		if (m_Disabled)
			return;

		::ImGui::SetCurrentContext(m_ImGuiContext);
		::ImGui::Render();
		Render(commandBuffer);
	}

	void ImGuiLayer::Shutdown()
	{
		RenderAPI::API renderAPI;
		if (m_Disabled)
			return;

		::ImGui::SetCurrentContext(m_ImGuiContext);
		renderAPI = Renderer::GetRenderAPI();

		switch (renderAPI)
		{
		case RenderAPI::API::Vulkan:
		{
			ImGui_ImplVulkan_Shutdown();
			m_Pass->Shutdown();
			m_ResourceHandler->Shutdown();
			break;
		}
#ifdef PLATFORM_WINDOWS
		case RenderAPI::API::DirectX12: ME_ASSERT(false, "RenderAPI::DirectX12 is unsupported"); break;
#elif PLATFORM_MAC
		case RenderAPI::API::Metal: ME_ASSERT(false, "RenderAPI::Metal is unsupported"); break;
#endif
		}

#ifdef PLATFORM_WINDOWS
		ImGui_ImplWin32_Shutdown();
#endif

		::ImGui::DestroyContext(m_ImGuiContext);
		m_ImGuiContext = nullptr;
	}

	void ImGuiLayer::Render(const ME::Core::Memory::Reference<ME::Render::CommandBuffer>& commandBuffer)
	{
		if (Renderer::GetRenderAPI() != Render::RenderAPI::API::Vulkan) return;
		ImDrawData* drawData = ::ImGui::GetDrawData();

		Render::ClearValue clrVal = {};
		clrVal.ColorClearValue = Core::Math::Vector4D32(0.f, 0.f, 0.f, 1.f);

		Render::RenderPassBeginInfo beginInfo = {};
		beginInfo.Framebuffer = RenderCommand::GetAvailableFramebuffer();
		beginInfo.ClearValues = { clrVal };
		beginInfo.RenderArea = {};
		beginInfo.RenderArea.Offset = { 0,0 };
		beginInfo.RenderArea.Extent = Render::RenderCommand::Get()->GetSwapChain()->GetExtent();

		//m_Pass->Begin(commandBuffer, beginInfo);
		//
		//ImGui_ImplVulkan_RenderDrawData(drawData, commandBuffer->As<VulkanCommandBuffer>()->GetCommandBuffer());
		//
		//m_Pass->End(commandBuffer);
	}

	void ImGuiLayer::CreateRenderResources()
	{
	    AttachmentSpecification attachmentSpecs = {};
		attachmentSpecs.IsStencil = false;
		attachmentSpecs.IsDepth = false;
		attachmentSpecs.AttachmentFormat = RenderCommand::GetAvailableFramebuffer()->GetSpecification().Attachments[0]->GetSpecification().Format;
		attachmentSpecs.LoadOp = LoadOperation::Load;
		attachmentSpecs.StoreOp = StoreOperation::Store;
		attachmentSpecs.InitialLayout = ImageLayout::ColorAttachment;
		attachmentSpecs.FinalLayout = ImageLayout::Present;
		attachmentSpecs.SampleCount = SampleCount::Count1;

		SubpassDependency dependency1 = {};
		dependency1.SubpassSrc = ~0u;
		dependency1.SubpassDst = 0;
		dependency1.AccessFlagsSrc = AccessFlags::MemoryRead;
		dependency1.AccessFlagsDst = AccessFlags::ColorAttachmentWrite |
			AccessFlags::ColorAttachmentRead |
			AccessFlags::DepthStencilRead |
			AccessFlags::DepthStencilWrite;
		dependency1.PipelineStageFlagsSrc = PipelineStageFlags::BottomOfPipe;
		dependency1.PipelineStageFlagsDst = PipelineStageFlags::ColorAttachmentOutput |
			PipelineStageFlags::EarlyFragmentTests |
			PipelineStageFlags::LateFragmentTests;

		SubpassDependency dependency2 = {};
		dependency2.SubpassSrc = dependency1.SubpassDst;
		dependency2.SubpassDst = dependency1.SubpassSrc;
		dependency2.AccessFlagsSrc = dependency1.AccessFlagsDst;
		dependency2.AccessFlagsDst = dependency1.AccessFlagsSrc;
		dependency2.PipelineStageFlagsSrc = dependency1.PipelineStageFlagsDst;
		dependency2.PipelineStageFlagsDst = dependency1.PipelineStageFlagsSrc;

		SubpassSpecification subpass = {};
		subpass.ColorAttachmentRefs = { 0 };
		subpass.DepthStencilAttachmentRef = ~0u;
		subpass.InputAttachmentRefs = {};
		subpass.PipelineBindPoint = PipelineBindPoint::Graphics;

		Render::RenderPassSpecification rpSpecs = Renderer::GetRenderPass()->GetSpecification();
		rpSpecs.AttachmentSpecs = { attachmentSpecs };
		rpSpecs.SubpassSpecs = { subpass };
		rpSpecs.SubpassDependencies = { dependency1, dependency2 };
		rpSpecs.DebugName = TEXT("ImGui render pass");
		m_Pass = Render::RenderPass::Create(rpSpecs);
	}

	void ImGuiLayer::PostRender()
	{
		ImGuiIO& io = ::ImGui::GetIO();
#ifdef PLATFORM_WINDOWS
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			::ImGui::UpdatePlatformWindows();
			::ImGui::RenderPlatformWindowsDefault();
		}
#endif
	}
}