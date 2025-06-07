#pragma once
#include "Renderer/Base/SwapChain.h"
#include "Platform/Windows/Win32Platform.h"

namespace Pawn::Render
{
	class PAWN_API DirectX11SwapChain : public SwapChain
	{
	public:
		DirectX11SwapChain(Window* window);
		~DirectX11SwapChain();

	public:
		virtual void Shutdown() override;

		virtual void Present() override;
		virtual void Resize(uint32 x, uint32 y) override;
		virtual void SetFullscreen(bool fullscreen) override;
		virtual void SetVSync(bool enabled) override;

		inline Pawn::Core::Containers::Array<Texture2D*>& GetImages() override { return m_images; };

	public:
		inline IDXGISwapChain1* GetSwapChain() { return m_SwapChain; }
		inline ID3D11Texture2D* GetBackBuffer() { return m_BackBuffer; }

	private:
		int32 Init(Window* inWindow);

	private:
		bool m_VSYNCEnabled;
		IDXGISwapChain1* m_SwapChain;
		ID3D11Texture2D* m_BackBuffer;
		Pawn::Core::Containers::Array<Texture2D*> m_images;

	};
};

