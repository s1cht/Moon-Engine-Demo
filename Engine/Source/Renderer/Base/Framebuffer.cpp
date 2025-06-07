#include "Framebuffer.h"
#include "Renderer/Renderer.h"

namespace  Pawn::Render
{
    Framebuffer* Framebuffer::Create(FramebufferSpecification& specification)
    {
        RendererAPI::API renderAPI = Renderer::GetRenderAPI();

        switch (renderAPI)
        {
            case Pawn::Render::RendererAPI::API::None:
            case Pawn::Render::RendererAPI::API::Metal:
            case Pawn::Render::RendererAPI::API::DirectX12:
            {
                PE_ASSERT(false, TEXT("Framebuffer: Requested creation with unsupported API! {0}"), (int32)renderAPI);
                return nullptr;
            }
            case Pawn::Render::RendererAPI::API::Vulkan:
	        {
                return CreateVulkanFramebuffer(specification);
	        }
        	case Pawn::Render::RendererAPI::API::DirectX11:
            {
                return CreateDirectX11Framebuffer(1, 1);break;
            }
        }

    }
}
