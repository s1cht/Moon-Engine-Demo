#include "Buffer.hpp"

#include "Renderer/RenderCommand.hpp"
#include "Renderer/Renderer.hpp"

namespace ME::Render
{
	ME::Core::Memory::Reference<ME::Render::VertexBuffer> VertexBuffer::Create(
        const VertexBufferSpecification& specification)
	{
	    ME::Core::Memory::Reference<ME::Render::VertexBuffer> obj = nullptr;
        switch (Renderer::GetRenderAPI())
        {
            case ME::Render::RenderAPI::API::Vulkan: obj = CreateVulkan(specification); break;
            default:
            {
                ME_ASSERT(false, "VertexBuffer: Requested creation with an unsupported API!");
                return nullptr;
            }
        }
        RenderCommand::RenderObjectCreated(obj);
	    return obj;
	}

    ME::Core::Memory::Reference<ME::Render::IndexBuffer> IndexBuffer::Create(
        const IndexBufferSpecification& specification)
    {
        ME::Core::Memory::Reference<ME::Render::IndexBuffer> obj = nullptr;
        switch (Renderer::GetRenderAPI())
        {
            case ME::Render::RenderAPI::API::Vulkan: obj = CreateVulkan(specification); break;
            default:
            {
                ME_ASSERT(false, "IndexBuffer: Requested creation with an unsupported API!");
                return nullptr;
            }
        }
        RenderCommand::RenderObjectCreated(obj);
        return obj;
    }

    ME::Core::Memory::Reference<ME::Render::Uniform> Uniform::Create(
        const UniformSpecification& specification)
    {
        ME::Core::Memory::Reference<ME::Render::Uniform> obj = nullptr;
        switch (Renderer::GetRenderAPI())
        {
            case ME::Render::RenderAPI::API::Vulkan: obj = CreateVulkan(specification); break;
            default:
            {
                ME_ASSERT(false, "Uniform: Requested creation with an unsupported API!");
                return nullptr;
            }
        }
        RenderCommand::RenderObjectCreated(obj);
        return obj;
    }

    ME::Core::Memory::Reference<ME::Render::StorageBuffer> StorageBuffer::Create(
        const StorageBufferSpecification& specification)
    {
        ME::Core::Memory::Reference<ME::Render::StorageBuffer> obj = nullptr;
        switch (Renderer::GetRenderAPI())
        {
            case ME::Render::RenderAPI::API::Vulkan: obj = CreateVulkan(specification); break;
            default:
            {
                ME_ASSERT(false, "StorageBuffer: Requested creation with an unsupported API!");
                return nullptr;
            }
        }
        RenderCommand::RenderObjectCreated(obj);
        return obj;
    }

    ME::Core::Memory::Reference<ME::Render::IndirectBuffer> IndirectBuffer::Create(
        const IndirectBufferSpecification& specification)
    {
        ME::Core::Memory::Reference<ME::Render::IndirectBuffer> obj = nullptr;
        switch (Renderer::GetRenderAPI())
        {
            case ME::Render::RenderAPI::API::Vulkan: obj = CreateVulkan(specification); break;
            default:
            {
                ME_ASSERT(false, "IndirectBuffer: Requested creation with an unsupported API!");
                return nullptr;
            }
        }
        RenderCommand::RenderObjectCreated(obj);
        return obj;
    }
}
