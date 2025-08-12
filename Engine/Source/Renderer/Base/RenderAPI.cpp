#include "RenderAPI.h"

namespace ME::Render
{
	RenderAPI::API RenderAPI::s_API = RenderAPI::API::Vulkan;

	ME::Core::Memory::Reference<RenderAPI> RenderAPI::Create()
	{
		switch (s_API)
		{
			case ME::Render::RenderAPI::API::Vulkan:
			{
				return CreateVulkan();
				break;
			}
			case ME::Render::RenderAPI::API::None:
			case ME::Render::RenderAPI::API::DirectX12:
			case ME::Render::RenderAPI::API::Metal:
			{
				ME_ASSERT(0, TEXT("Requested creation of a unsupported renderer! {0}"), (int32)s_API);
				break;
			}
		}
		return nullptr;
	}

}