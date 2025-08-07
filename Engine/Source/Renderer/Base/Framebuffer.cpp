#include "Framebuffer.h"
#include "Renderer/Renderer.h"

namespace  ME::Render
{
    Core::Memory::Reference<Render::Framebuffer> Framebuffer::Create(FramebufferSpecification& specification)
    {
        RendererAPI::API renderAPI = Renderer::GetRenderAPI();

        switch (renderAPI)
        {
            case ME::Render::RendererAPI::API::None:
            case ME::Render::RendererAPI::API::Metal:
            case ME::Render::RendererAPI::API::DirectX12:
            {
                ME_ASSERT(false, TEXT("Framebuffer: Requested creation with unsupported API! {0}"), (int32)renderAPI);
                return nullptr;
            }
            case ME::Render::RendererAPI::API::Vulkan:
	        {
                return CreateVulkanFramebuffer(specification);
	        }
        }

    }
}
