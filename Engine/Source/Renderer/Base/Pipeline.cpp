#include "Pipeline.h"

#include "Renderer/Renderer.h"

namespace ME::Render
{
    ME::Core::Memory::Reference<Pipeline> Pipeline::Create(const PipelineSpecification& specification)
    {
        RendererAPI::API renderAPI = Renderer::GetRenderAPI();

        switch (renderAPI)
        {
        case ME::Render::RendererAPI::API::None:
        case ME::Render::RendererAPI::API::Metal:
        case ME::Render::RendererAPI::API::DirectX12:
        {
            ME_ASSERT(false, TEXT("Pipeline: Requested creation with unsupported API! {0}"), (int32)renderAPI);
            return nullptr;
            break;
        }
        case ME::Render::RendererAPI::API::Vulkan:
        {
            return CreateVulkanPipeline(specification);
            break;
        }
        }
        
        return nullptr;
    }
}