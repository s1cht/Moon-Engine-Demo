#include "Buffer.hpp"
#include "Renderer/Renderer.hpp"

namespace ME::Render
{
	ME::Core::Memory::Reference<ME::Render::VertexBuffer> VertexBuffer::Create(const VertexBufferSpecification& specification)
	{
        switch (Renderer::GetRenderAPI())
        {
            case ME::Render::RenderAPI::API::Vulkan:
                return CreateVulkan(specification);
            default:
            {
                ME_ASSERT(false, "VertexBuffer: Requested creation with an unsupported API!");
                return nullptr;
            }
        }
	}

    ME::Core::Memory::Reference<ME::Render::IndexBuffer> IndexBuffer::Create(const IndexBufferSpecification& specification)
    {
        switch (Renderer::GetRenderAPI())
        {
            case ME::Render::RenderAPI::API::Vulkan:
                return CreateVulkan(specification);
            default:
            {
                ME_ASSERT(false, "IndexBuffer: Requested creation with an unsupported API!");
                return nullptr;
            }
        }
    }

    ME::Core::Memory::Reference<ME::Render::Uniform> Uniform::Create(const UniformSpecification& specification)
    {
        switch (Renderer::GetRenderAPI())
        {
            case ME::Render::RenderAPI::API::Vulkan:
                return CreateVulkan(specification);
            default:
            {
                ME_ASSERT(false, "Uniform: Requested creation with an unsupported API!");
                return nullptr;
            }
        }
    }

    ME::Core::Memory::Reference<ME::Render::StorageBuffer> StorageBuffer::Create(
        const StorageBufferSpecification& specification)
    {
        switch (Renderer::GetRenderAPI())
        {
            case ME::Render::RenderAPI::API::Vulkan:
                return CreateVulkan(specification);
            default:
            {
                ME_ASSERT(false, "Uniform: Requested creation with an unsupported API!");
                return nullptr;
            }
        }
    }

    ME::Core::Memory::Reference<ME::Render::IndirectBuffer> IndirectBuffer::Create(
        const IndirectBufferSpecification& specification)
    {
        switch (Renderer::GetRenderAPI())
        {
            case ME::Render::RenderAPI::API::Vulkan:
                return CreateVulkan(specification);
            default:
            {
                ME_ASSERT(false, "Uniform: Requested creation with an unsupported API!");
                return nullptr;
            }
        }
    }
}
