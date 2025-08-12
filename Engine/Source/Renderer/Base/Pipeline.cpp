#include "Pipeline.h"

#include "Renderer/Renderer.h"

namespace ME::Render
{
    ME::Core::Memory::Reference<Pipeline> Pipeline::Create(const PipelineSpecification& specification)
    {
        RenderAPI::API renderAPI = Renderer::GetRenderAPI();

        switch (renderAPI)
        {
        case ME::Render::RenderAPI::API::None:
        case ME::Render::RenderAPI::API::Metal:
        case ME::Render::RenderAPI::API::DirectX12:
        {
            ME_ASSERT(false, TEXT("Pipeline: Requested creation with unsupported API! {0}"), (int32)renderAPI);
            return nullptr;
            break;
        }
        case ME::Render::RenderAPI::API::Vulkan:
        {
            return CreateVulkanPipeline(specification);
            break;
        }
        }
        
        return nullptr;
    }
}