#pragma once

#include <Core.hpp>
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

		inline static void Clear(Pawn::Core::Math::Vector4D32 color)
		{
			s_Renderer->Clear(color);
		}

		inline static void DrawIndexed(uint32 indexCount, uint32 index)
		{
			s_Renderer->DrawIndexed(indexCount, index);
		}
		inline static void OnWindowUpdate(int32 x, int32 y)
		{
			s_Renderer->OnWindowEvent(x, y);
		}

		inline static void BindBackBuffer()
		{
			s_Renderer->BindBackBuffer();
		}
		inline static void UnbindBackBuffer()
		{
			s_Renderer->UnbindBackBuffer();
		}

		static RendererAPI* Get() { return s_Renderer; }

	private:
		static RendererAPI* s_Renderer;
		static SwapChain* s_SwapChain;
		static Framebuffer* s_Framebuffer;

	};

}