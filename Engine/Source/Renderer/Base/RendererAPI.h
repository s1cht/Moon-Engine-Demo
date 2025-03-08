#pragma once

#include <Core.h>
#include <Core/Math/Math.h>

#include "Platform/Base/Window.h"

namespace Pawn::Render
{
	class RendererAPI
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
		virtual void SetClearColor(Pawn::Math::Vector4D color) = 0;
		virtual void Clear() = 0;

		virtual void DrawIndexed(/* const VertexArray& array */) = 0;

	public:
		inline static void SetRendererAPI(API api) {};
		inline static API GetRendererAPI() { return s_API; };

	private:
		static API s_API;

	};
}