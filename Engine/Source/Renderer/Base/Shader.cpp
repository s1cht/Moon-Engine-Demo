#include "Shader.hpp"
#include "Renderer/RenderCommand.hpp"
#include "Renderer/Renderer.hpp"

namespace ME::Render
{
	ME::Core::Memory::Reference<ME::Render::Shader> Shader::Create(
        const ShaderSpecification& specification)
	{
        ME::Core::Memory::Reference<ME::Render::Shader> obj = nullptr;
        switch (Renderer::GetRenderAPI())
        {
            case ME::Render::RenderAPI::API::Vulkan: obj = CreateVulkan(specification); break;
            default:
            {
                ME_ASSERT(false, "Shader: Requested creation with an unsupported API!");
                return nullptr;
            }
        }
        RenderCommand::RenderObjectCreated(obj);
        return obj;
	}
}
