#include "Shader.hpp"
#include "Renderer/Renderer.hpp"

namespace ME::Render
{
	ME::Core::Memory::Reference<Shader> Shader::Create(const ShaderSpecification& specification)
	{
        switch (RenderAPI::API render = Renderer::GetRenderAPI())
		{
			case ME::Render::RenderAPI::API::Vulkan:
				return CreateVulkan(specification);
            default:
            {
                ME_ASSERT(false, "Create Shader: Unsupported renderer!");
                return nullptr;
            }
		}
	}
}
