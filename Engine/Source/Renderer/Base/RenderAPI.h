#pragma once

#include <Core.hpp>
#include <Core/Math/Math.hpp>

#include "ResourceHandler.h"
#include "Renderer/Base/Framebuffer.h"
#include "Renderer/Base/CommandBuffer.h"

namespace ME::Render
{
	class SwapChain;
	struct RenderPassBeginInfo;

	class MOON_API RenderAPI : public RenderObject
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
		virtual void PostInit() = 0;

		virtual void NewFrame() = 0;
		virtual void BeginRenderPass(ME::Core::Memory::Reference<ME::Render::CommandBuffer> buffer, ME::Render::RenderPassBeginInfo& info) = 0;
		virtual void EndRenderPass(ME::Core::Memory::Reference<ME::Render::CommandBuffer> buffer) = 0;

		virtual void Draw(ME::Core::Memory::Reference<Render::CommandBuffer> buffer, uint32 vertexCount, uint32 instanceCount) = 0;
		virtual void DrawIndexed(ME::Core::Memory::Reference<Render::CommandBuffer> buffer, uint32 indexCount, uint32 index) = 0;

		inline virtual void WriteResource(ME::Core::Memory::Reference<ME::Render::Uniform> buffer) = 0;
		inline virtual void WriteResource(ME::Core::Memory::Reference<ME::Render::StorageBuffer> buffer) = 0;
		inline virtual void BindResourceSet(ME::Core::Memory::Reference<Render::CommandBuffer> commandBuffer, ME::Core::Memory::Reference<Render::Pipeline> pipeline, ME::Core::Memory::Reference<ME::Render::Uniform> buffer) = 0;
		inline virtual void BindResourceSet(ME::Core::Memory::Reference<Render::CommandBuffer> commandBuffer, ME::Core::Memory::Reference<Render::Pipeline> pipeline, ME::Core::Memory::Reference<ME::Render::StorageBuffer> buffer) = 0;

		virtual void Clear(ME::Core::Math::Vector4D32 color) = 0;

		virtual void Submit(ME::Core::Memory::Reference<Render::CommandBuffer> buffer) = 0;
		virtual void Present() = 0;

		virtual inline ME::Core::Memory::Reference<ME::Render::SwapChain> GetSwapChain() = 0;
		virtual inline ME::Core::Memory::Reference<ME::Render::ResourceHandler> GetResourceHandler() = 0;

		virtual ME::Core::Memory::Reference<ME::Render::Framebuffer> GetAvailableFramebuffer() const = 0;
		virtual ME::Core::Memory::Reference<ME::Render::CommandBuffer> GetAvailableCommandBuffer() = 0;

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
