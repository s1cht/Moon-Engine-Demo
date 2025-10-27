#include "SwapChain.h"
#include "Application/Application.h"
#include "Renderer/Renderer.hpp"

namespace ME::Render
{
	ME::Core::Memory::Reference<SwapChain> SwapChain::Create(int32& result, Window* window)
	{
        switch (RenderAPI::API renderApi = Renderer::GetRenderAPI())
		{
			case ME::Render::RenderAPI::API::Vulkan:
				return CreateVulkanSwapChain(result);
			default:
			{
				ME_ASSERT(false, "SwapChain: Unsupported render API! {0}", static_cast<int32>(renderApi));
				return nullptr;
			}
		}
	}
}