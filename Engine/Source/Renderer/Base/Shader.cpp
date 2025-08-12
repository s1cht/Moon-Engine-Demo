#include "Shader.h"
#include "Renderer/Renderer.h"

namespace ME::Render
{
	ME::Core::Memory::Reference<Shader> Shader::Create(const ShaderSpecification& specification)
	{
		RenderAPI::API render = Renderer::GetRenderAPI();

		switch (render)
		{
			case ME::Render::RenderAPI::API::Vulkan:
				return CreateVulkanShader(specification);
            default:
            {
                ME_ASSERT(false, TEXT("Create Shader: Unsupported renderer!"));
                return nullptr;
            }
		}
	}

    SIZE_T SizeOfShaderType(ME::Render::ShaderType type)
    {
        switch (type)
        {
        case ME::Render::ShaderType::None:
        {
            return 0;
            break;
        }
        case ME::Render::ShaderType::Bool:
        {
            return 1;
            break;
        }
        case ME::Render::ShaderType::Float:
        case ME::Render::ShaderType::Int:
        case ME::Render::ShaderType::Uint:
        {
            return 4;
            break;
        }
        case ME::Render::ShaderType::Float2:
        case ME::Render::ShaderType::Int2:
        case ME::Render::ShaderType::Uint2:
        {
            return 4 * 2;
            break;
        }
        case ME::Render::ShaderType::Float3:
        case ME::Render::ShaderType::Int3:
        case ME::Render::ShaderType::Uint3:
        {
            return 4 * 3;
            break;
        }
        case ME::Render::ShaderType::Float4:
        case ME::Render::ShaderType::Int4:
        case ME::Render::ShaderType::Uint4:
        {
            return 4 * 4;
            break;
        }
        }

        ME_ASSERT(false, TEXT("Unknown type"));
        return 0;
    }

    uint32 ME::Render::GetTypeAPISpecificShaderType(ME::Render::ShaderType type)
    {
        RenderAPI::API render = Renderer::GetRenderAPI();

        switch (render)
        {
        case ME::Render::RenderAPI::API::None:
        case ME::Render::RenderAPI::API::DirectX12:
        case ME::Render::RenderAPI::API::Metal:
        {
            ME_ASSERT(false, TEXT("GetTypeAPISpecificShaderType: Unsupported renderer!"));
            break;
        }
        case ME::Render::RenderAPI::API::Vulkan:
	    {
            return ConvertShaderTypeVulkan(type);
	    }
        }

        return 0;
    }
}
