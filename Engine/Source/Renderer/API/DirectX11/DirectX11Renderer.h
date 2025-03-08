#pragma once
#include <Core.h>

#include "Platform/Platform.h"
#include "Renderer/Base/RendererAPI.h"

namespace Pawn::Render
{
	class DirectX11Renderer : RendererAPI
	{
	public:
		DirectX11Renderer();
		~DirectX11Renderer();


		virtual void SetClearColor(Pawn::Math::Vector4D color) override;
		virtual void Clear() override;

		virtual void DrawIndexed(/* const VertexArray& array */) override;
	private:
		int32 Init();
		void Shutdown();

	private:
		ID3D11Device* m_device;
		ID3D11DeviceContext* m_deviceContext;

	};
}
