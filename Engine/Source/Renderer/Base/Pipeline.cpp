#include "Pipeline.h"
#include "Renderer/Renderer.hpp"

namespace ME::Render
{
    ME::Core::Memory::Reference<Pipeline> Pipeline::Create(const PipelineSpecification& specification)
    {
        switch (RenderAPI::API renderAPI = Renderer::GetRenderAPI())
        {
            case ME::Render::RenderAPI::API::Vulkan:
                return CreateVulkanPipeline(specification);
            default:
            {
                ME_ASSERT(false, "Pipeline: Requested creation with unsupported API! {0}", static_cast<int32>(renderAPI));
                return nullptr;
            }
        }
    }
}