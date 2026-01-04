#include "RenderCommand.hpp"
#include "Managers/LightManager.hpp"

namespace ME::Render
{
	ME::Core::Memory::Reference<RenderAPI> RenderCommand::s_Renderer = nullptr;
	ME::Core::Memory::Reference<LightManager> RenderCommand::s_LightManager = nullptr; 
	RenderResourceTracker RenderCommand::s_ResourceTracker;

	bool RenderCommand::Init()
	{
		s_Renderer = RenderAPI::Create();

		if (s_Renderer == nullptr)
			return false;

		s_Renderer->PostInit();
		s_LightManager = ME::Core::Memory::MakeReference<LightManager>();

		return true;
	}
	void RenderCommand::Shutdown()
	{
		s_Renderer->YieldUntilIdle();
		s_ResourceTracker.ShutdownAll();
		if (s_Renderer)
			s_Renderer->Shutdown();
	}
}