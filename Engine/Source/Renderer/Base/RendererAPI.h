#pragma once

#include <Core.hpp>
#include <Core/Math/Math.hpp>

#include "Platform/Base/Window.h"
#include "Events/WindowEvents.h"
#include "Renderer/Base/Framebuffer.h"
#include "Renderer/Base/CommandBuffer.h"

namespace ME::Render
{
	class SwapChain;
	struct RenderPassBeginInfo;

	class MOON_API RendererAPI : public RenderObject
	{
	public:
		enum class API
		{
			None = 0,
			Vulkan,
			DirectX12,
			Metal,
		};

	public:
		virtual void Submit(ME::Core::Memory::Reference<Render::CommandBuffer> buffer) = 0;
		virtual void NewFrame() = 0;
		virtual void Present() = 0;
		virtual void Clear(ME::Core::Math::Vector4D32 color) = 0;
		virtual void Draw(ME::Core::Memory::Reference<Render::CommandBuffer> buffer, uint32 vertexCount, uint32 instanceCount) = 0;
		virtual void DrawIndexed(ME::Core::Memory::Reference<Render::CommandBuffer> buffer, uint32 indexCount, uint32 index) = 0;
		virtual void Shutdown() = 0;

		virtual void PostInit() = 0;

		virtual void OnWindowEvent(int32 x, int32 y) = 0;

		virtual void BeginRenderPass(ME::Core::Memory::Reference<ME::Render::CommandBuffer> buffer, ME::Render::RenderPassBeginInfo& info) = 0;
		virtual void EndRenderPass(ME::Core::Memory::Reference<ME::Render::CommandBuffer> buffer) = 0;

		virtual ME::Core::Memory::Reference<ME::Render::CommandBuffer> GetAvailableCommandBuffer() = 0;

		virtual SwapChain* GetSwapChain() = 0;

	public:
		inline static void SetRendererAPI(API api) { s_API = api; };
		inline static API GetRendererAPI() { return s_API; };

		static ME::Core::Memory::Reference<RendererAPI> Create();

	protected:
		inline static ME::Core::Memory::Reference<RendererAPI> CreateDirectX12();
		inline static ME::Core::Memory::Reference<RendererAPI> CreateVulkan();
		inline static ME::Core::Memory::Reference<RendererAPI> CreateMetal();

	private:
		static API s_API;

	};
}