#include "Framebuffer.hpp"
#include "Renderer/Renderer.hpp"

namespace  ME::Render
{
    Core::Memory::Reference<Render::Framebuffer> Framebuffer::Create(const FramebufferSpecification& specification)
    {
        switch (RenderAPI::API renderAPI = Renderer::GetRenderAPI())
        {
            case ME::Render::RenderAPI::API::Vulkan:
                return CreateVulkan(specification);
            default:
            {
                ME_ASSERT(false, "Framebuffer: Requested creation with unsupported API! {0}", static_cast<int32>(renderAPI));
                return nullptr;
            }
        }
    }
}
