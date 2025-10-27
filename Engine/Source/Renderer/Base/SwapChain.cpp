#include "SwapChain.h"
#include "Application/Application.h"
#include "Renderer/Renderer.h"
#include "Renderer/RenderCommand.h"

namespace ME::Render
{
	ME::Core::Memory::Reference<SwapChain> SwapChain::Create(int32& result, Window* window)
	{
		RenderAPI::API renderApi = Renderer::GetRenderAPI();

		switch (renderApi)
		{
			case ME::Render::RenderAPI::API::Vulkan:
			{
				return CreateVulkanSwapChain(result);
				break;
			}
			default:
			{
				ME_ASSERT(false, TEXT("SwapChain: Unsupported render API! {0}"), static_cast<int32>(renderApi));
				return nullptr;
				break;
			}
		}
	}
}