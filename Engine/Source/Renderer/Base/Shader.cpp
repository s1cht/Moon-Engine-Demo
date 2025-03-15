#include "Shader.h"
#include "Renderer/Renderer.h"

namespace Pawn::Render
{
	Shader* Shader::CreateShader(const String& path, Shader::Type type, bool compiled)
	{
		if (compiled)
			return CreateCompiledShader(path, type);
		else
			return CreateAndCompileShader(path, type);
	}

	Shader* CreateCompiledShader(const String& path, Shader::Type type)
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
				return CreateCompiledDirectX11Shader(path, type);
				break;
			}
		}

		return nullptr;
	}

	Shader* CreateAndCompileShader(const String& path, Shader::Type type)
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
			return CreateAndCompileDirectX11Shader(path, type);
			break;
		}
		}

		return nullptr;
	}

}