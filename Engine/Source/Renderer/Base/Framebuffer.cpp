#include "Framebuffer.h"
#include "Renderer/Renderer.h"

namespace  Pawn::Render
{
    Framebuffer* Framebuffer::Create(uint32 x, uint32 y)
    {
        RendererAPI::API renderAPI = Renderer::GetRenderAPI();

        switch (renderAPI)
        {
        case Pawn::Render::RendererAPI::API::None:
        case Pawn::Render::RendererAPI::API::Vulkan:
        case Pawn::Render::RendererAPI::API::Metal:
        case Pawn::Render::RendererAPI::API::DirectX12:
        {
            PE_ASSERT(false, TEXT("Framebuffer: Requested creation with unsupported API! {0}"), (int32)renderAPI);
            return nullptr;
            break;
        }
        case Pawn::Render::RendererAPI::API::DirectX11:
        {
            return CreateDirectX11Framebuffer(x, y);
            break;
        }
        }

    }
}
