#include "SwapChain.h"
#include "Application/Application.h"
#include "Renderer/Renderer.h"
#include "Renderer/RenderCommand.h"

namespace Pawn::Render
{
	SwapChain* SwapChain::Create(Window* window)
	{
		RendererAPI::API renderApi = Renderer::GetRenderAPI();

		switch (renderApi)
		{
		case Pawn::Render::RendererAPI::API::None:
		case Pawn::Render::RendererAPI::API::Vulkan:
		case Pawn::Render::RendererAPI::API::DirectX12:
		case Pawn::Render::RendererAPI::API::Metal:
		{
			PE_ASSERT(false, TEXT("SwapChain: Unsupported render API! {0}"), (int32)renderApi);
			return nullptr;
			break;
		}
		case Pawn::Render::RendererAPI::API::DirectX11:
		{
			return CreateDirectX11SwapChain(window);
			break;
		}
		}
	}
}