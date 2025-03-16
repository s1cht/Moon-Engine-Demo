#pragma once

#include <Core.h>
#include <Core/Math/Math.h>

#include "Platform/Base/Window.h"
#include "Events/WindowEvents.h"

namespace Pawn::Render
{
	class PAWN_API RendererAPI
	{
	public:
		enum class API
		{
			None = 0,
			Vulkan = 1,
			DirectX11 = 2, DirectX12 = 3,
			Metal = 4,
		};

	public:
		virtual void Present() = 0;
		virtual void Clear(Pawn::Math::Vector4D32 color) = 0;
		virtual void DrawIndexed(uint32 indexCount, uint32 index) = 0;
		virtual void Shutdown() = 0;

		virtual void PostInit() = 0;

		virtual void OnWindowEvent(int32 x, int32 y) = 0;

	public:
		inline static void SetRendererAPI(API api) {};
		inline static API GetRendererAPI() { return s_API; };

		static RendererAPI* Create();

	protected:
		inline static RendererAPI* CreateDirectX11();
		inline static RendererAPI* CreateDirectX12();
		inline static RendererAPI* CreateVulkan();
		inline static RendererAPI* CreateMetal();

	private:
		static API s_API;

	};
}