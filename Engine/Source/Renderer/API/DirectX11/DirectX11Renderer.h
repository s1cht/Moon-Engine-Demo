#pragma once
#include <Core.h>

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
		virtual void PostInit() override;
		
		virtual void Present() override;
		virtual void Clear(Pawn::Math::Vector4D32 color) override;
		virtual void DrawIndexed(uint32 indexCount, uint32 index) override;
		virtual void Shutdown() override;

		virtual void OnWindowEvent(int32 x, int32 y) override;

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

		D3D11_VIEWPORT m_Viewport;

	private:
		DirectX11SwapChain* m_SwapChain;
		DirectX11Framebuffer* m_Framebuffer;

	};
}

#define PE_D3D11_CHECK(x)		\
if (FAILED(x))					\
{								\
	Shutdown();					\
	PE_ASSERT(false,			\
TEXT(							\
"Assertion failed! Error: {0}"	\
),								\
(int32)x);						\
	return x;					\
}

#define PE_D3D11_RELEASE(x)		\
if (x != nullptr)				\
{								\
	x->Release();				\
	x = nullptr;				\
}

#define PE_D3D11_SHUTDOWN(x)	\
if (x != nullptr)				\
{								\
	x->Shutdown();				\
	delete x;					\
	x = nullptr;				\
}