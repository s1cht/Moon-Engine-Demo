#include "Renderer.h"

#include "RenderCommand.h"

namespace Pawn::Render
{
	bool Renderer::Init()
	{
		return RenderCommand::Init();
	}
	void Renderer::Shutdown()
	{
		RenderCommand::Shutdown();
	}
	void Renderer::BeginScene(Pawn::Core::Memory::Reference<Pawn::Render::Camera::Camera> camera)
	{
	}

	void Renderer::EndScene()
	{
	}

	void Renderer::Submit(uint32 indexCount, uint32 index)
	{
		RenderCommand::DrawIndexed(indexCount, index);
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