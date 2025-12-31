#include "RenderPass.hpp"
#include "Renderer/Renderer.hpp"

namespace ME::Render
{
    ME::Core::Memory::Reference<Render::RenderPass> RenderPass::Create(const RenderPassSpecification& specification)
	{
        switch (RenderAPI::API renderAPI = Renderer::GetRenderAPI())
        {
            case ME::Render::RenderAPI::API::Vulkan:
                return CreateVulkan(specification);
            default:
            {
                ME_ASSERT(false, "Render pass: Requested creation with unsupported API! {0}", static_cast<int32>(renderAPI));
                return nullptr;
            }
        }
	}
}
