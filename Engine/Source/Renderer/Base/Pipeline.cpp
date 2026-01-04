#include "Pipeline.hpp"
#include "Renderer/RenderCommand.hpp"
#include "Renderer/Renderer.hpp"

namespace ME::Render
{
    ME::Core::Memory::Reference<Pipeline> Pipeline::Create(
        const PipelineSpecification& specification)
    {
        ME::Core::Memory::Reference<ME::Render::Pipeline> obj = nullptr;
        switch (Renderer::GetRenderAPI())
        {
            case ME::Render::RenderAPI::API::Vulkan: obj = CreateVulkan(specification); break;
            default:
            {
                ME_ASSERT(false, "Pipeline: Requested creation with an unsupported API!");
                return nullptr;
            }
        }
        RenderCommand::RenderObjectCreated(obj);
        return obj;
    }
}