#include "ResourceHandler.hpp"
#include "Renderer/Renderer.hpp"

namespace ME::Render
{
	ME::Core::Memory::Reference<ME::Render::ResourceHandler> ResourceHandler::Create(uint32 bufferCount)
	{
        switch (RenderAPI::API renderAPI = Renderer::GetRenderAPI())
            {
            case ME::Render::RenderAPI::API::Vulkan:
                return CreateVulkan(bufferCount);
            default:
            {
                ME_ASSERT(false, "ResourceHandler: Requested creation with unsupported API! {0}", static_cast<int32>(renderAPI));
                return nullptr;
            }
        }
	}
}
