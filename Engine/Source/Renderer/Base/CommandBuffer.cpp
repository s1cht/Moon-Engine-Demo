#include "CommandBuffer.h"
#include "Renderer/Renderer.h"

namespace ME::Render
{
    ME::Core::Memory::Reference<Render::CommandBuffer> CommandBuffer::CreateCommandBuffer()
	{
        RendererAPI::API renderAPI = Renderer::GetRenderAPI();

        switch (renderAPI)
        {
            case ME::Render::RendererAPI::API::None:
            case ME::Render::RendererAPI::API::Metal:
            case ME::Render::RendererAPI::API::DirectX12:
            {
                ME_ASSERT(false, TEXT("CommandBuffer: Requested creation with unsupported API! {0}"), (int32)renderAPI);
                return nullptr;
                break;
            }
            case ME::Render::RendererAPI::API::Vulkan:
            {
                return CreateVulkanCommandBuffer();
                break;
            }
        }
	}

}
