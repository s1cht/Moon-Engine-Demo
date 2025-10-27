#include "RenderPass.h"

#include "Renderer/Renderer.h"

namespace ME::Render
{
    Core::Memory::Reference<Render::RenderPass> RenderPass::Create(RenderPassSpecification& specification)
	{
        RenderAPI::API renderAPI = Renderer::GetRenderAPI();

        switch (renderAPI)
        {
            case ME::Render::RenderAPI::API::None:
            case ME::Render::RenderAPI::API::Metal:
            case ME::Render::RenderAPI::API::DirectX12:
            {
                ME_ASSERT(false, TEXT("Render pass: Requested creation with unsupported API! {0}"), (int32)renderAPI);
                return nullptr;
                break;
            }
            case ME::Render::RenderAPI::API::Vulkan:
            {
                return CreateVulkanRenderPass(specification);
                break;
            }
        }

        return nullptr;
	}
}
