#include "CommandBuffer.hpp"
#include "Renderer/Renderer.hpp"

namespace ME::Render
{
    ME::Core::Memory::Reference<Render::CommandBuffer> CommandBuffer::CreateCommandBuffer()
	{
        switch (RenderAPI::API renderAPI = Renderer::GetRenderAPI())
        {
            case ME::Render::RenderAPI::API::Vulkan:
                return CreateVulkan();
            default:
            {
                ME_ASSERT(false, "CommandBuffer: Requested creation with unsupported API! {0}", static_cast<int32>(renderAPI));
                return nullptr;
            }
        }
	}
}
