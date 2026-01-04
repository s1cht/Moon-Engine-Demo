#include "CommandBuffer.hpp"
#include "Renderer/RenderCommand.hpp"
#include "Renderer/Renderer.hpp"

namespace ME::Render
{
    ME::Core::Memory::Reference<Render::CommandBuffer> CommandBuffer::CreateCommandBuffer()
	{
        ME::Core::Memory::Reference<ME::Render::CommandBuffer> obj = nullptr;
        switch (Renderer::GetRenderAPI())
        {
            case ME::Render::RenderAPI::API::Vulkan: obj = CreateVulkan(); break;
            default:
            {
                ME_ASSERT(false, "CommandBuffer: Requested creation with an unsupported API!");
                return nullptr;
            }
        }
        RenderCommand::RenderObjectCreated(obj);
        return obj;
	}
}
