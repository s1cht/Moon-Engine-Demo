#include "SwapChain.hpp"
#include "Application/Application.hpp"
#include "Renderer/Renderer.hpp"

namespace ME::Render
{
	ME::Core::Memory::Reference<ME::Render::SwapChain> SwapChain::Create(int32& result)
	{
        ME::Core::Memory::Reference<ME::Render::SwapChain> obj = nullptr;
        switch (Renderer::GetRenderAPI())
        {
            case ME::Render::RenderAPI::API::Vulkan: obj = CreateVulkan(result); break;
            default:
            {
                ME_ASSERT(false, "SwapChain: Requested creation with an unsupported API!");
                return nullptr;
            }
        }
        return obj;
	}
}