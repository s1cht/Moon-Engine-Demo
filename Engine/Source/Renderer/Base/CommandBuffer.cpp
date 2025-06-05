#include "CommandBuffer.h"
#include "Renderer/Renderer.h"

namespace Pawn::Render
{
	CommandBuffer* CommandBuffer::CreateCommandBuffer()
	{
        RendererAPI::API renderAPI = Renderer::GetRenderAPI();

        switch (renderAPI)
        {
            case Pawn::Render::RendererAPI::API::None:
            case Pawn::Render::RendererAPI::API::Metal:
            case Pawn::Render::RendererAPI::API::DirectX12:
            {
                PE_ASSERT(false, TEXT("CommandBuffer: Requested creation with unsupported API! {0}"), (int32)renderAPI);
                return nullptr;
                break;
            }
            case Pawn::Render::RendererAPI::API::Vulkan:
            {
                return CreateVulkanCommandBuffer();
                break;
            }
            case Pawn::Render::RendererAPI::API::DirectX11:
            {
  //              return CreateDirectX11CommandBuffer();
                break;
            }
        }
	}

}
