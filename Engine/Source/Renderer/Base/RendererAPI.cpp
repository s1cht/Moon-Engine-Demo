#include "RendererAPI.h"

namespace ME::Render
{
	RendererAPI::API RendererAPI::s_API = RendererAPI::API::Vulkan;

	ME::Core::Memory::Reference<RendererAPI> RendererAPI::Create()
	{
		switch (s_API)
		{
			case ME::Render::RendererAPI::API::Vulkan:
			{
				return CreateVulkan();
				break;
			}
			case ME::Render::RendererAPI::API::None:
			case ME::Render::RendererAPI::API::DirectX12:
			case ME::Render::RendererAPI::API::Metal:
			{
				ME_ASSERT(0, TEXT("Requested creation of a unsupported renderer! {0}"), (int32)s_API);
				break;
			}
		}
		return nullptr;
	}

}