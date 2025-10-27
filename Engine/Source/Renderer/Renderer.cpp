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

	void Renderer::SetRenderAPI(RenderAPI::API api)
	{
		RenderAPI::SetRendererAPI(api);
	}
	inline RenderAPI::API Renderer::GetRenderAPI()
	{
		return RenderAPI::GetRendererAPI();
	}
}