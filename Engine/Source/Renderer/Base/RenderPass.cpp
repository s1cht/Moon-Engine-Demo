#include "RenderPass.hpp"
#include "Renderer/RenderCommand.hpp"
#include "Renderer/Renderer.hpp"

namespace ME::Render
{
    ME::Core::Memory::Reference<Render::RenderPass> RenderPass::Create(
        const RenderPassSpecification& specification)
    {
        ME::Core::Memory::Reference<ME::Render::RenderPass> obj = nullptr;
        switch (Renderer::GetRenderAPI())
        {
            case ME::Render::RenderAPI::API::Vulkan: obj = CreateVulkan(specification); break;
            default:
            {
                ME_ASSERT(false, "RenderPass: Requested creation with an unsupported API!");
                return nullptr;
            }
        }
        RenderCommand::RenderObjectCreated(obj);
        return obj;
    }
}
