#include "Renderer.h"

#include "RenderCommand.h"

namespace ME::Render
{
	bool Renderer::Init()
	{
		return RenderCommand::Init();
	}
	void Renderer::Shutdown()
	{
		RenderCommand::Shutdown();
	}
	void Renderer::BeginScene(ME::Core::Memory::Reference<ME::Render::Camera::Camera> camera)
	{
	}

	void Renderer::EndScene()
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