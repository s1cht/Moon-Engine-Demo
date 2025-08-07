#include "RenderCommand.h"

namespace ME::Render
{
	ME::Core::Memory::Reference<RendererAPI> RenderCommand::s_Renderer;

	bool RenderCommand::Init()
	{
		s_Renderer = RendererAPI::Create();

		if (s_Renderer == nullptr)
			return false;

		s_Renderer->PostInit();

		return true;
	}
	void RenderCommand::Shutdown()
	{
		if (s_Renderer)
			s_Renderer->Shutdown();
	}
}