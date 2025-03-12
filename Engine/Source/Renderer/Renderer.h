#pragma once

#include "Core.h"
#include "Base/RendererAPI.h"
#include "Base/Buffer.h"

namespace Pawn::Render
{
	class PAWN_API Renderer
	{
	public:
		//static void 

		static bool Init();
		static void Shutdown();

	public:
		static void BeginScene();
		static void EndScene();

		static void Submit(/* const VertexArray& array */);

	public:
		inline static void SetRenderAPI(RendererAPI::API api);
		inline static RendererAPI::API GetRenderAPI();

	private:

	};

}