#include "ResourceHandler.h"

#include "RenderAPI.h"
#include "Renderer/Renderer.h"

namespace ME::Render
{
	ME::Core::Memory::Reference<ME::Render::ResourceHandler> ResourceHandler::Create()
	{
        RenderAPI::API renderAPI = Renderer::GetRenderAPI();

        switch (renderAPI)
        {
        case ME::Render::RenderAPI::API::Vulkan:
        {
            return CreateVulkan();
        }
        default:
        {
            ME_ASSERT(false, TEXT("ResourceHandler: Requested creation with unsupported API! {0}"), (int32)renderAPI);
            return nullptr;
        }
        }
	}
}
