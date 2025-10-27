#include "Framebuffer.h"
#include "Renderer/Renderer.h"

namespace  ME::Render
{
    Core::Memory::Reference<Render::Framebuffer> Framebuffer::Create(FramebufferSpecification& specification)
    {
        RenderAPI::API renderAPI = Renderer::GetRenderAPI();

        switch (renderAPI)
        {
            case ME::Render::RenderAPI::API::Vulkan:
	        {
                return CreateVulkanFramebuffer(specification);
	        }
            default:
            {
                ME_ASSERT(false, TEXT("Framebuffer: Requested creation with unsupported API! {0}"), (int32)renderAPI);
                return nullptr;
            }
        }

    }
}
