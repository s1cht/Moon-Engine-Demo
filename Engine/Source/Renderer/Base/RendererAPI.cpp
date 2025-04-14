#include "RendererAPI.h"

#include <Core/Misc/Assertion.h>

namespace Pawn::Render
{
	RendererAPI::API RendererAPI::s_API = RendererAPI::API::DirectX11;

	RendererAPI* RendererAPI::Create()
	{
		switch (s_API)
		{
			case Pawn::Render::RendererAPI::API::DirectX11:
			{
				return CreateDirectX11();
				break;
			}
			case Pawn::Render::RendererAPI::API::Vulkan:
			{
				return CreateVulkan();
				break;
			}
			case Pawn::Render::RendererAPI::API::None:
			case Pawn::Render::RendererAPI::API::DirectX12:
			case Pawn::Render::RendererAPI::API::Metal:
			{
				PE_ASSERT(0, TEXT("Requested creation of a unsupported renderer! {0}"), (int32)s_API);
				break;
			}
		}
		return nullptr;
	}

}