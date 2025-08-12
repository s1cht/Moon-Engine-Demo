#include "Buffer.h"
#include "Renderer/Renderer.h"

namespace ME::Render
{
	ME::Core::Memory::Reference<ME::Render::VertexBuffer> VertexBuffer::Create(const VertexBufferSpecification& specification)
	{
        RenderAPI::API renderAPI = Renderer::GetRenderAPI();

        switch (renderAPI)
        {
        case ME::Render::RenderAPI::API::Vulkan:
        {
            return CreateVulkan(specification);
            break;
        }
        default:
        {
            ME_ASSERT(false, TEXT("VertexBuffer: Requested creation with unsupported API! {0}"), (int32)renderAPI);
            return nullptr;
            break;
        }
        }
	}

    ME::Core::Memory::Reference<ME::Render::IndexBuffer> IndexBuffer::Create(const IndexBufferSpecification& specification)
    {
        RenderAPI::API renderAPI = Renderer::GetRenderAPI();

        switch (renderAPI)
        {
        case ME::Render::RenderAPI::API::Vulkan:
        {
            return CreateVulkan(specification);
            break;
        }
        default:
        {
            ME_ASSERT(false, TEXT("IndexBuffer: Requested creation with unsupported API! {0}"), (int32)renderAPI);
            return nullptr;
            break;
        }
        }
    }

    ME::Core::Memory::Reference<ME::Render::Uniform> Uniform::Create(const UniformSpecification& specification)
    {
        RenderAPI::API renderAPI = Renderer::GetRenderAPI();

        switch (renderAPI)
        {
        case ME::Render::RenderAPI::API::Vulkan:
        {
            return CreateVulkan(specification);
            break;
        }
        default:
        {
            ME_ASSERT(false, TEXT("Uniform: Requested creation with unsupported API! {0}"), (int32)renderAPI);
            return nullptr;
            break;
        }
        }
    }
}