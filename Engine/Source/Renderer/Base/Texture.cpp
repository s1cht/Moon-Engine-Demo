#include "Texture.h"
#include "Renderer/Renderer.hpp"

namespace ME::Render
{
	ME::Core::Memory::Reference<Texture2D> Texture2D::Create(const Texture2DSpecification& specification)
	{
        switch (RenderAPI::API renderApi = Renderer::GetRenderAPI())
		{
			case ME::Render::RenderAPI::API::Vulkan:
				return CreateVulkanTexture(specification);
			default:
			{
				ME_ASSERT(false, "Texture: Unsupported render API! {0}", static_cast<int32>(renderApi));
				return nullptr;
			}
		}
	}
}
