#include "Texture.hpp"

#include "Renderer/RenderCommand.hpp"
#include "Renderer/Renderer.hpp"

namespace ME::Render
{
	ME::Core::Memory::Reference<ME::Render::Texture1D> Texture1D::Create(const Texture1DSpecification& specification)
	{
		ME::Core::Memory::Reference<ME::Render::Texture1D> obj = nullptr;
		switch (Renderer::GetRenderAPI())
		{
			case ME::Render::RenderAPI::API::Vulkan: obj = CreateVulkan(specification); break;
			default:
			{
				ME_ASSERT(false, "Texture1D: Requested creation with an unsupported API!");
				return nullptr;
			}
		}
		RenderCommand::RenderObjectCreated(obj);
		return obj;
	}

	ME::Core::Memory::Reference<ME::Render::Texture2D> Texture2D::Create(const Texture2DSpecification& specification)
	{
		ME::Core::Memory::Reference<ME::Render::Texture2D> obj = nullptr;
		switch (Renderer::GetRenderAPI())
		{
			case ME::Render::RenderAPI::API::Vulkan: obj = CreateVulkan(specification); break;
			default:
			{
				ME_ASSERT(false, "Texture2D: Requested creation with an unsupported API!");
				return nullptr;
			}
		}
		RenderCommand::RenderObjectCreated(obj);
		return obj;
	}

	ME::Core::Memory::Reference<ME::Render::Texture3D> Texture3D::Create(const Texture3DSpecification& specification)
	{
		ME::Core::Memory::Reference<ME::Render::Texture3D> obj = nullptr;
		switch (Renderer::GetRenderAPI())
		{
			case ME::Render::RenderAPI::API::Vulkan: obj = CreateVulkan(specification); break;
			default:
			{
				ME_ASSERT(false, "Texture3D: Requested creation with an unsupported API!");
				return nullptr;
			}
		}
		RenderCommand::RenderObjectCreated(obj);
		return obj;
	}
}
