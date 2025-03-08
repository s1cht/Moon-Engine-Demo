#include "pch.h"
#include "Renderer.h"

namespace Pawn::Render
{
	void Renderer::BeginScene()
	{
	
	}
	
	void Renderer::EndScene()
	{
	
	}
	
	void Renderer::Submit()
	{
	
	}

	void Renderer::SetRenderAPI(RendererAPI::API api)
	{
		RendererAPI::SetRendererAPI(api);
	}
	inline RendererAPI::API Renderer::GetRenderAPI()
	{
		return RendererAPI::GetRendererAPI();
	}
}