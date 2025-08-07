#pragma once

#include <Core.hpp>
#include "Base/RendererAPI.h"
#include "Base/SwapChain.h"
#include "Base/Framebuffer.h"
#include "Base/CommandBuffer.h"

#define CmdBufFucntion(name, ...)																	\
inline static void name(ME::Core::Memory::Reference<ME::Render::CommandBuffer> buffer, __VA_ARGS__)
#define CmdBufFunctionImpl(name, ...) { s_Renderer->name(buffer, __VA_ARGS__); }

#define CmdBuffFunctionNA(name)																		\
inline static void name(ME::Core::Memory::Reference<ME::Render::CommandBuffer> buffer)	\
{ s_Renderer->name(buffer); }

namespace ME::Render
{
	class MOON_API RenderCommand
	{
	public:
		static bool Init();
		static void Shutdown();

	public:
		inline static void NewFrame()
		{
			s_Renderer->NewFrame();
		}

		inline static void Submit(ME::Core::Memory::Reference<Render::CommandBuffer> buffer)
		{
			s_Renderer->Submit(buffer);
		}

		inline static void Present()
		{
			s_Renderer->Present();
		}

		inline static void Clear(ME::Core::Math::Vector4D32 color)
		{
			s_Renderer->Clear(color);
		}

		inline static void Draw(ME::Core::Memory::Reference<Render::CommandBuffer> buffer, uint32 vertexCount, uint32 instanceCount)
		{
			s_Renderer->Draw(buffer, vertexCount, instanceCount);
		}

		inline static void DrawIndexed(ME::Core::Memory::Reference<Render::CommandBuffer> buffer, uint32 indexCount, uint32 index)
		{
			s_Renderer->DrawIndexed(buffer, indexCount, index);
		}
		inline static void OnWindowUpdate(int32 x, int32 y)
		{
			s_Renderer->OnWindowEvent(x, y);
		}

		inline static int32 GetFrameIndex()
		{
			return s_Renderer->GetSwapChain()->GetFrameIndex();
		}

		inline static ME::Core::Memory::Reference<ME::Render::CommandBuffer> GetAvailableCommandBuffer()
		{
			return s_Renderer->GetAvailableCommandBuffer();
		}

		CmdBufFucntion(BeginRenderPass, ME::Render::RenderPassBeginInfo& info)
		CmdBufFunctionImpl(BeginRenderPass, info)

		CmdBuffFunctionNA(EndRenderPass)

		static ME::Core::Memory::Reference<RendererAPI> Get() { return s_Renderer; }

	private:
		static ME::Core::Memory::Reference<RendererAPI> s_Renderer;
		static SwapChain* s_SwapChain;
		static Framebuffer* s_Framebuffer;

	};

}

#undef CmdBufFucntion