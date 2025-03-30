#pragma once
#include "Platform/Platform.h"
#include "Renderer/Base/RendererAPI.h"

#include "DirectX11SwapChain.h"
#include "DirectX11Framebuffer.h"

namespace Pawn::Render
{
	class PAWN_API DirectX11Renderer : public RendererAPI
	{
	public:
		DirectX11Renderer();
		~DirectX11Renderer();

	public:
		void PostInit() override;
		
		void Present() override;
		void Clear(Pawn::Core::Math::Vector4D32 color) override;
		void DrawIndexed(uint32 indexCount, uint32 index) override;
		void Shutdown() override;

		void OnWindowEvent(int32 x, int32 y) override;

		void BindBackBuffer() override;
		void UnbindBackBuffer() override;


	public:
		ID3D11Device* GetDevice() { return m_Device; }
		ID3D11DeviceContext* GetDeviceContext() { return m_DeviceContext; }
		IDXGIFactory2* GetFactory() { return m_Factory; }
		DirectX11SwapChain* GetSwapChain() { return m_SwapChain; }

	private:
		int32 Init();

	private:
		ID3D11Device* m_Device;
		ID3D11DeviceContext* m_DeviceContext;

		IDXGIFactory2* m_Factory;
		IDXGIOutput* m_Output;
		IDXGIAdapter* m_Adapter;

	private:
		DirectX11SwapChain* m_SwapChain;
		DirectX11Framebuffer* m_Framebuffer;

	};
}