#include "Buffer.h"
#include "Renderer/Renderer.h"


namespace Pawn::Render
{
    SIZE_T SizeOfShaderType(Pawn::Render::ShaderType type)
    {
        switch (type)
        {
            case Pawn::Render::ShaderType::None:
            {
                return 0;
                break;
            }
            case Pawn::Render::ShaderType::Bool:
            {
                return 1;
                break;
            }
            case Pawn::Render::ShaderType::Float:
            case Pawn::Render::ShaderType::Int:
            case Pawn::Render::ShaderType::Uint:
            {
                return 4;
                break;
            }
            case Pawn::Render::ShaderType::Float2:
			case Pawn::Render::ShaderType::Int2:
			case Pawn::Render::ShaderType::Uint2:
            {
                return 4 * 2;
                break;
            }
            case Pawn::Render::ShaderType::Float3:
			case Pawn::Render::ShaderType::Int3:
			case Pawn::Render::ShaderType::Uint3:
            {
                return 4 * 3;
                break;
            }
            case Pawn::Render::ShaderType::Float4:
			case Pawn::Render::ShaderType::Int4:
			case Pawn::Render::ShaderType::Uint4:
            {
                return 4 * 4;
                break;
            }
            case Pawn::Render::ShaderType::Mat3x3:
            {
                return 4 * 3 * 3;
                break;
            }
            case Pawn::Render::ShaderType::Mat4x4:
            {
                return 4 * 4 * 4;
                break;
            }
        }
        
        PE_ASSERT(false, TEXT("Unknown type"));
        return 0;
    }

    uint32 Pawn::Render::GetTypeAPISpecificShaderType(Pawn::Render::ShaderType type)
    {
        RendererAPI::API render = Renderer::GetRenderAPI();

        switch (render)
        {
            case Pawn::Render::RendererAPI::API::None:
            case Pawn::Render::RendererAPI::API::Vulkan:
            case Pawn::Render::RendererAPI::API::DirectX12:
            case Pawn::Render::RendererAPI::API::Metal:
            {
                PE_ASSERT(false, TEXT("GetTypeAPISpecificShaderType: Unsupported renderer!"));
                break;
            }
            case Pawn::Render::RendererAPI::API::DirectX11:
            {
                return ConvertShaderTypeDirectX11(type);
                break;
            }
        }

        return 0;
    }

    VertexBuffer* VertexBuffer::Create(void* data, SIZE_T size, Usage usage)
    {
        RendererAPI::API render = Renderer::GetRenderAPI();

        switch (render)
        {
            case Pawn::Render::RendererAPI::API::None:
            case Pawn::Render::RendererAPI::API::Vulkan:
            case Pawn::Render::RendererAPI::API::DirectX12:
            case Pawn::Render::RendererAPI::API::Metal:
            {
                PE_ASSERT(false, TEXT("Create VertexBuffer: Unsupported renderer!"));
                break;
            }
            case Pawn::Render::RendererAPI::API::DirectX11:
            {
                return CreateDirectX11VertexBuffer(data, size, usage);
                break;
            }
        }

        return nullptr;
    }

	IndexBuffer* IndexBuffer::Create(void* data, uint32 count, Usage usage)
	{
		RendererAPI::API render = Renderer::GetRenderAPI();

		switch (render)
		{
		case Pawn::Render::RendererAPI::API::None:
		case Pawn::Render::RendererAPI::API::Vulkan:
		case Pawn::Render::RendererAPI::API::DirectX12:
		case Pawn::Render::RendererAPI::API::Metal:
		{
			PE_ASSERT(false, TEXT("Create IndexBuffer: Unsupported renderer!"));
			break;
		}
		case Pawn::Render::RendererAPI::API::DirectX11:
		{
			return CreateDirectX11IndexBuffer(data, count, usage);
			break;
		}
		}

		return nullptr;
	}

	Pawn::Render::Uniform* Uniform::Create(SIZE_T size, Usage usage)
	{
		RendererAPI::API render = Renderer::GetRenderAPI();

		switch (render)
		{
		case Pawn::Render::RendererAPI::API::None:
		case Pawn::Render::RendererAPI::API::Vulkan:
		case Pawn::Render::RendererAPI::API::DirectX12:
		case Pawn::Render::RendererAPI::API::Metal:
		{
			PE_ASSERT(false, TEXT("Create IndexBuffer: Unsupported renderer!"));
			break;
		}
		case Pawn::Render::RendererAPI::API::DirectX11:
		{
			return CreateDirectX11Uniform(size, usage);
			break;
		}
		}

		return nullptr;
	}

}