#include "Texture.hpp"
#include "Renderer/Renderer.hpp"

namespace ME::Render
{
	ME::Core::Memory::Reference<Texture1D> Texture1D::Create(const Texture1DSpecification& specification)
	{
		switch (RenderAPI::API renderApi = Renderer::GetRenderAPI())
		{
			case ME::Render::RenderAPI::API::Vulkan:
				return CreateVulkan(specification);
			default:
			{
				ME_ASSERT(false, "Texture1D: Unsupported render API! {0}", static_cast<int32>(renderApi));
				return nullptr;
			}
		}
	}

	ME::Core::Memory::Reference<Texture2D> Texture2D::Create(const Texture2DSpecification& specification)
	{
        switch (RenderAPI::API renderApi = Renderer::GetRenderAPI())
		{
			case ME::Render::RenderAPI::API::Vulkan:
				return CreateVulkan(specification);
			default:
			{
				ME_ASSERT(false, "Texture2D: Unsupported render API! {0}", static_cast<int32>(renderApi));
				return nullptr;
			}
		}
	}

	ME::Core::Memory::Reference<Texture3D> Texture3D::Create(const Texture3DSpecification& specification)
	{
		switch (RenderAPI::API renderApi = Renderer::GetRenderAPI())
		{
			case ME::Render::RenderAPI::API::Vulkan:
				return CreateVulkan(specification);
			default:
			{
				ME_ASSERT(false, "Texture3D: Unsupported render API! {0}", static_cast<int32>(renderApi));
				return nullptr;
			}
		}
	}
}
