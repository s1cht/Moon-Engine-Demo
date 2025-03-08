#include "pch.h"
#include "ImGuiLayer.h"
#include "Renderer/Renderer.h"

#include "imgui.h"

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

		ImGui::CreateContext();
		ImGui::StyleColorsDark();

		ImGuiIO& io = ImGui::GetIO();
		io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
		io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

		switch (renderAPI)
		{
			case RendererAPI::API::Vulkan: PE_ASSERT(false, TEXT("RenderAPI::Vulkan is unsupported"));
#ifdef PLATFORM_WINDOWS
			case RendererAPI::API::DirectX11: PE_ASSERT(false, TEXT("RenderAPI::DirectX11 is unsupported"));
			case RendererAPI::API::DirectX12: PE_ASSERT(false, TEXT("RenderAPI::DirectX12 is unsupported"));
#elif PLATFORM_MAC
			case RendererAPI::API::Metal: PE_ASSERT(false, TEXT("RenderAPI::Metal is unsupported"));
#endif
		}
	}

	void ImGuiLayer::OnDetach()
	{
		if (!m_Disabled)
			return;
		ImGui::DestroyContext();
	}

	void ImGuiLayer::OnImGuiRender()
	{
		if (!m_Disabled)
			return;
	}

	void ImGuiLayer::BeginRender()
	{
		if (!m_Disabled)
			return;
	}

	void ImGuiLayer::EndRender()
	{
		if (!m_Disabled)
			return;
	}

}