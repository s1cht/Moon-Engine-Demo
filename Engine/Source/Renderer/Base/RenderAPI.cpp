#include "RenderAPI.h"

namespace ME::Render
{
	RenderAPI::API RenderAPI::s_API = RenderAPI::API::Vulkan;

	ME::Core::Memory::Reference<RenderAPI> RenderAPI::Create()
	{
		switch (s_API)
		{
			case ME::Render::RenderAPI::API::Vulkan:
				return CreateVulkan();
            default:
		    {
                ME_ASSERT(0, "Requested creation of a unsupported renderer! {0}", static_cast<int32>(s_API));
				return nullptr;
			}
		}
	}

}