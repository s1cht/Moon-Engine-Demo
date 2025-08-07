#include "RenderPass.h"

#include "Renderer/Renderer.h"

namespace ME::Render
{
    Core::Memory::Reference<Render::RenderPass> RenderPass::Create(RenderPassSpecification& specification)
	{
        RendererAPI::API renderAPI = Renderer::GetRenderAPI();

        switch (renderAPI)
        {
            case ME::Render::RendererAPI::API::None:
            case ME::Render::RendererAPI::API::Metal:
            case ME::Render::RendererAPI::API::DirectX12:
            {
                ME_ASSERT(false, TEXT("Render pass: Requested creation with unsupported API! {0}"), (int32)renderAPI);
                return nullptr;
                break;
            }
            case ME::Render::RendererAPI::API::Vulkan:
            {
                return CreateVulkanRenderPass(specification);
                break;
            }
        }

        return nullptr;
	}
}
