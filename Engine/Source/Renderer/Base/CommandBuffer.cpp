#include "CommandBuffer.h"
#include "Renderer/Renderer.h"

namespace ME::Render
{
    ME::Core::Memory::Reference<Render::CommandBuffer> CommandBuffer::CreateCommandBuffer()
	{
        RenderAPI::API renderAPI = Renderer::GetRenderAPI();

        switch (renderAPI)
        {
            case ME::Render::RenderAPI::API::Vulkan:
            {
                return CreateVulkanCommandBuffer();
                break;
            }
            default:
            {
                ME_ASSERT(false, TEXT("CommandBuffer: Requested creation with unsupported API! {0}"), (int32)renderAPI);
                return nullptr;
                break;
            }
        }
	}

}
