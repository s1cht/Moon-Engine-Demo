#pragma once

#include <Core.hpp>
#include "Base/RendererAPI.h"
#include "Base/Buffer.h"
#include "Camera/Camera.h"

namespace ME::Render
{
	class MOON_API Renderer
	{
	public:
		//static void 

		static bool Init();
		static void Shutdown();

	public:
		static void BeginScene(ME::Core::Memory::Reference<ME::Render::Camera::Camera> camera);
		static void EndScene();

		static void Submit(uint32 indexCount, uint32 index);

	public:
		inline static void SetRenderAPI(RendererAPI::API api);
		inline static RendererAPI::API GetRenderAPI();

	private:

	};

}