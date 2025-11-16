#include "Framebuffer.hpp"
#include "Renderer/Renderer.hpp"

namespace  ME::Render
{
    Core::Memory::Reference<Render::Framebuffer> Framebuffer::Create(FramebufferSpecification& specification)
    {
        switch (RenderAPI::API renderAPI = Renderer::GetRenderAPI())
        {
            case ME::Render::RenderAPI::API::Vulkan:
                return CreateVulkanFramebuffer(specification);
            default:
            {
                ME_ASSERT(false, "Framebuffer: Requested creation with unsupported API! {0}", static_cast<int32>(renderAPI));
                return nullptr;
            }
        }
    }
}
