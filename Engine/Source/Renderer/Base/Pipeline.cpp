#include "Pipeline.h"

#include "Renderer/Renderer.h"

#include <Core/Misc/Assertion.h>

namespace Pawn::Render
{
    Pipeline* Pipeline::Create()
    {
        RendererAPI::API renderAPI = Renderer::GetRenderAPI();

        switch (renderAPI)
        {
        case Pawn::Render::RendererAPI::API::None:
        case Pawn::Render::RendererAPI::API::Metal:
        case Pawn::Render::RendererAPI::API::DirectX12:
        {
            PE_ASSERT(false, TEXT("Framebuffer: Requested creation with unsupported API! {0}"), (int32)renderAPI);
            return nullptr;
            break;
        }
        case Pawn::Render::RendererAPI::API::Vulkan:
        {
            //return CreateVulkanPipeline();
            break;
        }
        case Pawn::Render::RendererAPI::API::DirectX11:
        {
            //return CreateDirectX11Pipeline();
            break;
        }
        }
        
        return nullptr;
    }
}