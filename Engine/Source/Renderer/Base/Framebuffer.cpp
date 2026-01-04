#include "Framebuffer.hpp"
#include "Renderer/RenderCommand.hpp"
#include "Renderer/Renderer.hpp"

namespace  ME::Render
{
    Core::Memory::Reference<Render::Framebuffer> Framebuffer::Create(
        const FramebufferSpecification& specification)
    {
        ME::Core::Memory::Reference<ME::Render::Framebuffer> obj = nullptr;
        switch (Renderer::GetRenderAPI())
        {
            case ME::Render::RenderAPI::API::Vulkan: obj = CreateVulkan(specification); break;
            default:
            {
                ME_ASSERT(false, "Framebuffer: Requested creation with an unsupported API!");
                return nullptr;
            }
        }
        RenderCommand::RenderObjectCreated(obj);
        return obj;
    }
}
