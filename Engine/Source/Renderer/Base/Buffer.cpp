#include "Buffer.h"
#include "Renderer/Renderer.h"

namespace ME::Render
{
    VertexBuffer* VertexBuffer::Create(void* data, SIZE_T size, Usage usage)
    {
        RendererAPI::API render = Renderer::GetRenderAPI();

        switch (render)
        {
            case ME::Render::RendererAPI::API::None:
            case ME::Render::RendererAPI::API::Vulkan:
            case ME::Render::RendererAPI::API::DirectX12:
            case ME::Render::RendererAPI::API::Metal:
            {
                ME_ASSERT(false, TEXT("Create VertexBuffer: Unsupported renderer!"));
                break;
            }
        }

        return nullptr;
    }

	IndexBuffer* IndexBuffer::Create(void* data, uint32 count, Usage usage)
	{
		RendererAPI::API render = Renderer::GetRenderAPI();

		switch (render)
		{
		case ME::Render::RendererAPI::API::None:
		case ME::Render::RendererAPI::API::Vulkan:
		case ME::Render::RendererAPI::API::DirectX12:
		case ME::Render::RendererAPI::API::Metal:
		{
			ME_ASSERT(false, TEXT("Create IndexBuffer: Unsupported renderer!"));
			break;
		}
		}

		return nullptr;
	}

	ME::Render::Uniform* Uniform::Create(SIZE_T size, Usage usage)
	{
		RendererAPI::API render = Renderer::GetRenderAPI();

		switch (render)
		{
		case ME::Render::RendererAPI::API::None:
		case ME::Render::RendererAPI::API::Vulkan:
		case ME::Render::RendererAPI::API::DirectX12:
		case ME::Render::RendererAPI::API::Metal:
		{
			ME_ASSERT(false, TEXT("Create IndexBuffer: Unsupported renderer!"));
			break;
		}
		}

		return nullptr;
	}

}