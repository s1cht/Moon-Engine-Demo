#pragma once

#include <Core.h>
#include "Base/RendererAPI.h"
#include "Base/SwapChain.h"
#include "Base/Framebuffer.h"

namespace Pawn::Render
{
	class PAWN_API RenderCommand
	{
	public:
		static bool Init();
		static void Shutdown();

	public:
		inline static void Present()
		{
			s_Renderer->Present();
		}

		inline static void Clear(Math::Vector4D32 color)
		{
			s_Renderer->Clear(color);
		}

		inline static void DrawIndexed(/* const VertexArray& array */)
		{
			s_Renderer->DrawIndexed();
		}
		inline static void OnWindowUpdate(int32 x, int32 y)
		{
			s_Renderer->OnWindowEvent(x, y);
		}

		static RendererAPI* Get() { return s_Renderer; }

	private:
		static RendererAPI* s_Renderer;
		static SwapChain* s_SwapChain;
		static Framebuffer* s_Framebuffer;

	};

}