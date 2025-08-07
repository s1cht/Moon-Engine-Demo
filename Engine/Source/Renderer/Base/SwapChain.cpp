#include "SwapChain.h"
#include "Application/Application.h"
#include "Renderer/Renderer.h"
#include "Renderer/RenderCommand.h"

namespace ME::Render
{
	ME::Core::Memory::Reference<SwapChain> SwapChain::Create(int32& result, Window* window)
	{
		RendererAPI::API renderApi = Renderer::GetRenderAPI();

		switch (renderApi)
		{
			case ME::Render::RendererAPI::API::None:
			case ME::Render::RendererAPI::API::DirectX12:
			case ME::Render::RendererAPI::API::Metal:
			{
				ME_ASSERT(false, TEXT("SwapChain: Unsupported render API! {0}"), (int32)renderApi);
				return nullptr;
				break;
			}
			case ME::Render::RendererAPI::API::Vulkan:
			{
				return CreateVulkanSwapChain(result);
				break;
			}
		}
	}
}