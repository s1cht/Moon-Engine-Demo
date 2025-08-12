#pragma once

#include <Core.hpp>
#include "Base/RenderAPI.h"
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
		inline static ME::Core::Memory::Reference<ME::Render::ResourceHandler> GetResourceHandler()
		{
			return s_Renderer->GetResourceHandler();
		}

		inline static void CreateFramebuffers(ME::Core::Memory::Reference<ME::Render::RenderPass> renderPass)
		{
			s_Renderer->CreateFramebuffers(renderPass);
		}

		inline static ME::Core::Memory::Reference<ME::Render::Framebuffer> GetAvailableFramebuffer()
		{
			return s_Renderer->GetAvailableFramebuffer();
		}

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

		inline static void WriteResource(ME::Core::Memory::Reference<ME::Render::Uniform> buffer)
		{
			s_Renderer->WriteResource(buffer);
		}

		inline static void WriteResource(ME::Core::Memory::Reference<ME::Render::StorageBuffer> buffer)
		{
			s_Renderer->WriteResource(buffer);
		}

		inline static void BindResourceSet(ME::Core::Memory::Reference<Render::CommandBuffer> commandBuffer, ME::Core::Memory::Reference<Render::Pipeline> pipeline, ME::Core::Memory::Reference<ME::Render::Uniform> buffer)
		{
			s_Renderer->BindResourceSet(commandBuffer, pipeline, buffer);
		}

		inline static void BindResourceSet(ME::Core::Memory::Reference<Render::CommandBuffer> commandBuffer, ME::Core::Memory::Reference<Render::Pipeline> pipeline, ME::Core::Memory::Reference<ME::Render::StorageBuffer> buffer)
		{
			s_Renderer->BindResourceSet(commandBuffer, pipeline, buffer);
		}

		inline static int32 GetFrameIndex()
		{
			return s_Renderer->GetSwapChain()->GetFrameIndex();
		}

		inline static ME::Core::Memory::Reference<ME::Render::CommandBuffer> GetAvailableCommandBuffer()
		{
			return s_Renderer->GetAvailableCommandBuffer();
		}

		inline static bool UpdateRequired()
		{
			return s_Renderer->GetSwapChain()->UpdateRequired();
		}

		inline static void Updated()
		{
			s_Renderer->GetSwapChain()->Updated();
		}

		CmdBufFucntion(BeginRenderPass, ME::Render::RenderPassBeginInfo& info)
		CmdBufFunctionImpl(BeginRenderPass, info)

		CmdBuffFunctionNA(EndRenderPass)

		static ME::Core::Memory::Reference<RenderAPI> Get() { return s_Renderer; }

	private:
		static ME::Core::Memory::Reference<RenderAPI> s_Renderer;

	};

}

#undef CmdBufFucntion