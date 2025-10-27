#pragma once

#include <Core.hpp>
#include <Core/Math/Math.hpp>

#include "ResourceHandler.h"
#include "Renderer/Base/Framebuffer.h"
#include "Renderer/Base/CommandBuffer.h"

namespace ME::Render
{
    class Buffer;
}

namespace ME::Render
{
    class IndirectBuffer;
    class SwapChain;
	struct RenderPassBeginInfo;

	struct DrawIndirectIndexedData
	{
		uint32 indexCount;
		uint32 instanceCount;
		uint32 firstIndex;
		uint32 vertexOffset;
		uint32 firstInstance;
	};

	class MEAPI RenderAPI : public RenderObject
	{
	public:
		enum class API : uint8
		{
			None = 0,
			Vulkan,
			DirectX12,
			Metal,
		};

	public:
		virtual void NewFrame() = 0;
		virtual void EndFrame() = 0;
		virtual void BeginRenderPass(ME::Core::Memory::Reference<ME::Render::CommandBuffer> buffer, ME::Render::RenderPassBeginInfo& info) = 0;
		virtual void EndRenderPass(ME::Core::Memory::Reference<ME::Render::CommandBuffer> buffer) = 0;

		virtual void Draw(ME::Core::Memory::Reference<Render::CommandBuffer> buffer, uint32 vertexCount, uint32 instanceCount) = 0;
		virtual void DrawIndexed(ME::Core::Memory::Reference<Render::CommandBuffer> buffer, uint32 indexCount, uint32 index) = 0;
		virtual void DrawIndexedIndirect(ME::Core::Memory::Reference<Render::CommandBuffer> commandBuffer,
			const ME::Core::Memory::Reference<Render::IndirectBuffer>& buffer, SIZE_T offset, 
			uint32 drawCount, uint32 stride) = 0;
		virtual void DrawIndexedIndirectCount(ME::Core::Memory::Reference<Render::CommandBuffer> commandBuffer, 
			const ME::Core::Memory::Reference<Render::IndirectBuffer>& buffer, SIZE_T offset, 
			const ME::Core::Memory::Reference<Render::IndirectBuffer>& drawCount, uint32 drawCountOffset, 
			uint32 maxDrawCount, uint32 stride) = 0;
		virtual void DispatchMesh(ME::Core::Memory::Reference<Render::CommandBuffer> commandBuffer, uint32 groupCountX, uint32 groupCountY, uint32 groupCountZ) = 0;
		virtual void Dispatch(ME::Core::Memory::Reference<Render::CommandBuffer> commandBuffer, uint32 groupCountX, uint32 groupCountY, uint32 groupCountZ) = 0;

		inline virtual void WriteResource(ME::Core::Memory::Reference<ME::Render::Uniform> buffer) = 0;
		inline virtual void WriteResource(ME::Core::Memory::Reference<ME::Render::StorageBuffer> buffer) = 0;
		inline virtual void BindResourceSet(ME::Core::Memory::Reference<Render::CommandBuffer> commandBuffer, 
			ME::Core::Memory::Reference<Render::Pipeline> pipeline, 
			ME::Core::Memory::Reference<ME::Render::Buffer> buffer) = 0;

		inline virtual void BindTexture(ME::Core::Memory::Reference<Render::CommandBuffer> commandBuffer, ME::Core::Memory::Reference<Render::Pipeline> pipeline, ME::Core::Memory::Reference<ME::Render::Texture2D> texture, uint32 set) = 0;

		virtual void Clear(ME::Core::Math::Vector4D32 color) = 0;

		virtual void Submit(ME::Core::Memory::Reference<Render::CommandBuffer> buffer) = 0;
		virtual void Present() = 0;

		virtual inline ME::Core::Memory::Reference<ME::Render::SwapChain> GetSwapChain() = 0;
		virtual inline ME::Core::Memory::Reference<ME::Render::ResourceHandler> GetResourceHandler() = 0;

		virtual ME::Core::Memory::Reference<ME::Render::Framebuffer> GetAvailableFramebuffer() const = 0;
		virtual ME::Core::Memory::Reference<ME::Render::CommandBuffer> GetAvailableCommandBuffer() = 0;

		virtual ME::Core::Memory::Reference<ME::Render::CommandBuffer> GetSingleUseCommandBuffer() = 0;
		virtual void SubmitAndFreeSingleUseCommandBuffer(ME::Core::Memory::Reference<ME::Render::CommandBuffer> buffer) = 0;

		virtual void PostInit() = 0;
		virtual void Resize(uint32 x, uint32 y) = 0;
		virtual void OnWindowEvent(int32 x, int32 y) = 0;
		virtual void CreateFramebuffers(ME::Core::Memory::Reference<ME::Render::RenderPass> renderPass) = 0;

	public:
		inline static void SetRendererAPI(API api) { s_API = api; }
		inline static API GetRendererAPI() { return s_API; }

		static ME::Core::Memory::Reference<RenderAPI> Create();

	protected:
		inline static ME::Core::Memory::Reference<RenderAPI> CreateVulkan();
		inline static ME::Core::Memory::Reference<RenderAPI> CreateDirectX12();
		inline static ME::Core::Memory::Reference<RenderAPI> CreateMetal();

	private:
		static API s_API;

	};
}
