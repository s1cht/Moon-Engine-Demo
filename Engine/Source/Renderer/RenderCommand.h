#pragma once

#include <Core.hpp>
#include "Base/RenderAPI.h"
#include "Base/SwapChain.h"
#include "Base/Framebuffer.h"
#include "Base/CommandBuffer.h"

#define CmdBufFunction(name, ...)																				\
    inline static void name(ME::Core::Memory::Reference<ME::Render::CommandBuffer> commandBuffer, __VA_ARGS__)
#define CmdBufFunctionImpl(name, ...)						\
    {														\
        s_Renderer->name(commandBuffer, __VA_ARGS__);		\
    }

#define CmdBuffFunctionNA(name)																		\
    inline static void name(ME::Core::Memory::Reference<ME::Render::CommandBuffer> buffer)			\
    {																								\
        s_Renderer->name(buffer);																	\
    }

namespace ME::Render
{
	class MEAPI RenderCommand
	{
	public:
		static bool Init();
		static void Shutdown();

	public:
		static ME::Core::Memory::Reference<RenderAPI> Get()
		{
		    return s_Renderer;
		}

		inline static ME::Core::Memory::Reference<ME::Render::ResourceHandler> GetResourceHandler()
		{
			return s_Renderer->GetResourceHandler();
		}

		inline static ME::Core::Memory::Reference<ME::Render::Framebuffer> GetAvailableFramebuffer()
		{
			return s_Renderer->GetAvailableFramebuffer();
		}

	public:
		inline static void NewFrame()
		{
			s_Renderer->NewFrame();
		}

		inline static void EndFrame()
		{
			s_Renderer->EndFrame();
		}

		inline static void Present()
		{
			s_Renderer->Present();
		}

		inline static void WriteResource(const ME::Core::Memory::Reference<ME::Render::Uniform>& buffer)
		{
			s_Renderer->WriteResource(buffer);
		}

		inline static void WriteResource(const ME::Core::Memory::Reference<ME::Render::StorageBuffer>& buffer)
		{
			s_Renderer->WriteResource(buffer);
		}

		inline static uint32 GetFrameIndex()
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

		inline static void CreateFramebuffers(const ME::Core::Memory::Reference<ME::Render::RenderPass>& renderPass)
		{
			s_Renderer->CreateFramebuffers(renderPass);
		}

	public:
		inline static void OnWindowUpdate(int32 x, int32 y)
		{
			s_Renderer->OnWindowEvent(x, y);
		}

	public:
		CmdBufFunction(BeginRenderPass, ME::Render::RenderPassBeginInfo& info)
		CmdBufFunctionImpl(BeginRenderPass, info)

		CmdBuffFunctionNA(EndRenderPass)

		CmdBufFunction(BindResourceSet, ME::Core::Memory::Reference<Render::Pipeline> pipeline, ME::Core::Memory::Reference<ME::Render::Buffer> buffer)
		CmdBufFunctionImpl(BindResourceSet, pipeline, buffer)

		CmdBufFunction(BindTexture, ME::Core::Memory::Reference<Render::Pipeline> pipeline, ME::Core::Memory::Reference<ME::Render::Texture2D> texture, uint32 set)
		CmdBufFunctionImpl(BindTexture, pipeline, texture, set)

		CmdBufFunction(Draw, uint32 vertexCount, uint32 instanceCount)
		CmdBufFunctionImpl(Draw, vertexCount, instanceCount)

		CmdBufFunction(DrawIndexed, uint32 indexCount, uint32 index)
		CmdBufFunctionImpl(DrawIndexed, indexCount, index)

		CmdBufFunction(DrawIndexedIndirect, const ME::Core::Memory::Reference<Render::IndirectBuffer>& buffer, uint32 offset, uint32 drawCount, uint32 stride)
		CmdBufFunctionImpl(DrawIndexedIndirect, buffer, offset, drawCount, stride)

		CmdBufFunction(DrawIndexedIndirectCount, const ME::Core::Memory::Reference<Render::IndirectBuffer>& buffer, uint32 offset,
			const ME::Core::Memory::Reference<Render::IndirectBuffer>& countBuffer, uint32 countOffset,
			uint32 maxDrawCount, uint32 stride)
		CmdBufFunctionImpl(DrawIndexedIndirectCount, buffer, offset, countBuffer, countOffset, maxDrawCount, stride)

		CmdBufFunction(Dispatch, uint32 groupCountX, uint32 groupCountY, uint32 groupCountZ)
		CmdBufFunctionImpl(Dispatch, groupCountX, groupCountY, groupCountZ)

		CmdBufFunction(DispatchMesh, uint32 groupCountX, uint32 groupCountY, uint32 groupCountZ)
		CmdBufFunctionImpl(DispatchMesh, groupCountX, groupCountY, groupCountZ)

		CmdBuffFunctionNA(Submit)

	private:
		static ME::Core::Memory::Reference<RenderAPI> s_Renderer;

	};

}

#undef CmdBufFunction