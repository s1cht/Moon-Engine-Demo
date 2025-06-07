#include "RenderPass.h"

#include "Renderer/Renderer.h"

namespace Pawn::Render
{
	RenderPass* RenderPass::Create(RenderPassSpecification& specification)
	{
        RendererAPI::API renderAPI = Renderer::GetRenderAPI();

        switch (renderAPI)
        {
            case Pawn::Render::RendererAPI::API::None:
            case Pawn::Render::RendererAPI::API::Metal:
            case Pawn::Render::RendererAPI::API::DirectX12:
            {
                PE_ASSERT(false, TEXT("Render pass: Requested creation with unsupported API! {0}"), (int32)renderAPI);
                return nullptr;
                break;
            }
            case Pawn::Render::RendererAPI::API::Vulkan:
            {
                return CreateVulkanRenderPass(specification);
                break;
            }
            case Pawn::Render::RendererAPI::API::DirectX11:
            {
                //return CreateDirectX11Pipeline();
                return nullptr;
                break;
            }
        }

        return nullptr;
	}
}
