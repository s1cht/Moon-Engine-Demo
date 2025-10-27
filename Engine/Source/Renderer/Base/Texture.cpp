#include "Texture.h"

#include "Renderer/Renderer.h"

namespace ME::Render
{
	ME::Core::Memory::Reference<Texture2D> Texture2D::Create(const Texture2DSpecification& specification)
	{
		RenderAPI::API renderApi = Renderer::GetRenderAPI();

		switch (renderApi)
		{
		case ME::Render::RenderAPI::API::Vulkan:
		{
			return CreateVulkanTexture(specification);
			break;
		}
		default:
		{
			ME_ASSERT(false, TEXT("Texture: Unsupported render API! {0}"), static_cast<int32>(renderApi));
			return nullptr;
			break;
		}
		}
	}
}
