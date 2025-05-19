#include "Shader.h"
#include "Renderer/Renderer.h"

namespace Pawn::Render
{
	const uchar* Shader::s_ShaderSourceExtension;
	const uchar* Shader::s_CompiledShaderExtension;

	Shader* Shader::CreateShader(const uchar* fileName, Shader::Type type, bool compiled)
	{
		if (compiled)
			return CreateCompiledShader(fileName, type);
		else
			return CreateAndCompileShader(fileName, type);
	}

	Shader* Shader::CreateCompiledShader(const uchar* fileName, Shader::Type type)
	{
		RendererAPI::API render = Renderer::GetRenderAPI();

		switch (render)
		{
			case Pawn::Render::RendererAPI::API::None:
			case Pawn::Render::RendererAPI::API::Vulkan:
			case Pawn::Render::RendererAPI::API::DirectX12:
			case Pawn::Render::RendererAPI::API::Metal:
			{
				PE_ASSERT(false, TEXT("Create Shader: Unsupported renderer!"));
				break;
			}
			case Pawn::Render::RendererAPI::API::DirectX11:
			{
				return CreateCompiledDirectX11Shader(fileName, type);
				break;
			}
		}

		return nullptr;
	}

	Shader* Shader::CreateAndCompileShader(const uchar* fileName, Shader::Type type)
	{
		RendererAPI::API render = Renderer::GetRenderAPI();

		switch (render)
		{
		case Pawn::Render::RendererAPI::API::None:
		case Pawn::Render::RendererAPI::API::Vulkan:
		case Pawn::Render::RendererAPI::API::DirectX12:
		case Pawn::Render::RendererAPI::API::Metal:
		{
			PE_ASSERT(false, TEXT("Create Shader: Unsupported renderer!"));
			break;
		}
		case Pawn::Render::RendererAPI::API::DirectX11:
		{
			return CreateAndCompileDirectX11Shader(fileName, type);
			break;
		}
		}

		return nullptr;
	}

}