#pragma once
#include <Core.h>
#include <Core/Math/Math.h>

#include "Platform/Windows/Win32Platform.h"
#include "Renderer/Base/Framebuffer.h"

namespace Pawn::Render
{
	class PAWN_API DirectX11Framebuffer : public Framebuffer
	{
	public:
		DirectX11Framebuffer(uint32 x, uint32 y, bool usingBackBuffer);
		~DirectX11Framebuffer();

	public:
		virtual bool Bind() override;
		virtual bool Unbind() override;

		virtual void Shutdown() override;

	public:
		ID3D11RenderTargetView* GetRenderTargetView() { return m_RTV; }
		ID3D11DepthStencilView* GetDepthStencilView() { return m_DepthStencilView; }

	private:
		int32 Init(uint32 x, uint32 y);

	private:
		bool m_UsingBackBuffer;

	private:
		ID3D11Texture2D* m_Buffer;
		ID3D11RenderTargetView* m_RTV;

		ID3D11Texture2D* m_DepthBuffer;
		ID3D11DepthStencilView* m_DepthStencilView;

	};

}