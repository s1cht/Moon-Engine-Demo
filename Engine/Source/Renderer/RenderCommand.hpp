#pragma once
#include <Core.hpp>

#include "Base/RenderAPI.hpp"
#include "Base/SwapChain.hpp"
#include "Base/Framebuffer.hpp"
#include "Base/CommandBuffer.hpp"

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
    class RTexture2D;

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

		inline static void WriteResource(const ME::Core::Memory::Reference<ME::Render::VertexBuffer>& buffer)
		{
			s_Renderer->WriteResource(buffer);
		}

        inline static void WriteResource(const ME::Core::Memory::Reference<ME::Render::IndexBuffer>& buffer)
		{
			s_Renderer->WriteResource(buffer);
		}

		inline static void WriteResource(const ME::Core::Memory::Reference<ME::Render::IndirectBuffer>& buffer)
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

		inline static void CreateFramebuffers(const ME::Core::Memory::Reference<ME::Render::RenderPass>& renderPass,
			const ME::Core::Array<ME::Core::Memory::Reference<Render::RTexture2D>>& attachments)
		{
			s_Renderer->CreateFramebuffers(renderPass, attachments);
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

		CmdBufFunction(DrawIndexedIndirectCount, PipelineStageFlags bufferSrc,
			const ME::Core::Memory::Reference<Render::IndirectBuffer>& buffer, uint32 offset,
			const ME::Core::Memory::Reference<Render::IndirectBuffer>& countBuffer, uint32 countOffset,
			uint32 maxDrawCount, uint32 stride)
		CmdBufFunctionImpl(DrawIndexedIndirectCount, bufferSrc, buffer, offset, countBuffer, countOffset, maxDrawCount, stride)

		CmdBufFunction(Dispatch, uint32 groupCountX, uint32 groupCountY, uint32 groupCountZ)
		CmdBufFunctionImpl(Dispatch, groupCountX, groupCountY, groupCountZ)

		CmdBufFunction(DispatchMesh, uint32 groupCountX, uint32 groupCountY, uint32 groupCountZ)
		CmdBufFunctionImpl(DispatchMesh, groupCountX, groupCountY, groupCountZ)

		CmdBufFunction(BindIndexBuffer, const ME::Core::Memory::Reference<Render::IndexBuffer>& buffer, uint32 offset)
		CmdBufFunctionImpl(BindIndexBuffer, buffer, offset)

        CmdBufFunction(BindVertexBuffer, const ME::Core::Memory::Reference<Render::VertexBuffer>& buffer, uint32 offset)
		CmdBufFunctionImpl(BindVertexBuffer, buffer, offset)

		CmdBufFunction(BufferBarrier, const ME::Core::Memory::Reference<Render::VertexBuffer>& buffer, BarrierInfo src, BarrierInfo dst)
		CmdBufFunctionImpl(BufferBarrier, buffer, src, dst)

		CmdBufFunction(BufferBarrier, const ME::Core::Memory::Reference<Render::IndexBuffer>& buffer, BarrierInfo src, BarrierInfo dst)
		CmdBufFunctionImpl(BufferBarrier, buffer, src, dst)

		CmdBufFunction(BufferBarrier, const ME::Core::Memory::Reference<Render::IndirectBuffer>& buffer, BarrierInfo src, BarrierInfo dst)
		CmdBufFunctionImpl(BufferBarrier, buffer, src, dst)

		CmdBufFunction(BufferBarrier, const ME::Core::Memory::Reference<Render::Uniform>& buffer, BarrierInfo src, BarrierInfo dst)
		CmdBufFunctionImpl(BufferBarrier, buffer, src, dst)

		CmdBufFunction(BufferBarrier, const ME::Core::Memory::Reference<Render::StorageBuffer>& buffer, BarrierInfo src, BarrierInfo dst)
		CmdBufFunctionImpl(BufferBarrier, buffer, src, dst)

		CmdBufFunction(TextureBarrier, const ME::Core::Memory::Reference<Render::Texture1D>& buffer, BarrierInfo src, BarrierInfo dst)
		CmdBufFunctionImpl(TextureBarrier, buffer, src, dst)

		CmdBufFunction(TextureBarrier, const ME::Core::Memory::Reference<Render::Texture2D>& buffer, BarrierInfo src, BarrierInfo dst)
		CmdBufFunctionImpl(TextureBarrier, buffer, src, dst)

		CmdBufFunction(TextureBarrier, const ME::Core::Memory::Reference<Render::Texture3D>& buffer, BarrierInfo src, BarrierInfo dst)
		CmdBufFunctionImpl(TextureBarrier, buffer, src, dst)

		CmdBuffFunctionNA(Submit)

	private:
		static ME::Core::Memory::Reference<RenderAPI> s_Renderer;

	};

}

#undef CmdBufFunction