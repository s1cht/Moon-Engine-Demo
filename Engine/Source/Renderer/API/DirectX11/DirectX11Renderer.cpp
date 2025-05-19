#include "DirectX11Macros.h"
#include "DirectX11Renderer.h"
#include "Application/Application.h"

namespace Pawn::Render
{
	inline RendererAPI* RendererAPI::CreateDirectX11()
	{
		return new DirectX11Renderer();
	}

	DirectX11Renderer::DirectX11Renderer()
	{
		m_Device = nullptr;
		m_DeviceContext = nullptr;

		m_Factory = nullptr;
		m_Output = nullptr;
		m_Adapter = nullptr;
		int32 result = Init();
		PE_ASSERT(!result, TEXT("DirectX 11: Initialization failed! Error: {0}"), result);
	}
	DirectX11Renderer::~DirectX11Renderer()
	{
		Shutdown();
	}
	int32 Pawn::Render::DirectX11Renderer::Init()
	{
		HRESULT result;

		uint32 deviceFlags;
		const D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_1 };
		D3D_FEATURE_LEVEL featureLevel;
		IDXGIDevice2* dxgiDevice;

#ifdef PE_DEBUG
		deviceFlags = D3D12_DEVICE_FLAG_DEBUG_LAYER_ENABLED;
#else
		deviceFlags = 0;
#endif

		result = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, deviceFlags, featureLevels, 1, D3D11_SDK_VERSION, &m_Device, &featureLevel, &m_DeviceContext);
		if (result == DXGI_ERROR_UNSUPPORTED)
		{
			result = D3D11CreateDevice(m_Adapter, D3D_DRIVER_TYPE_WARP, nullptr, deviceFlags, featureLevels, 1, D3D11_SDK_VERSION, &m_Device, &featureLevel, &m_DeviceContext);
			PE_D3D11_CHECK(result);
		}

		result = m_Device->QueryInterface(__uuidof(IDXGIDevice2), (void**)&dxgiDevice);
		PE_D3D11_CHECK(result);

		result = dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&m_Adapter);
		PE_D3D11_CHECK(result);

		result = m_Adapter->GetParent(__uuidof(IDXGIFactory2), (void**)&m_Factory);
		PE_D3D11_CHECK(result);

		return result;
	}

	void DirectX11Renderer::Shutdown()
	{
		PE_D3D11_SHUTDOWN(m_Framebuffer);
		PE_D3D11_SHUTDOWN(m_SwapChain);

		PE_D3D11_RELEASE(m_Output);
		PE_D3D11_RELEASE(m_Adapter);
		PE_D3D11_RELEASE(m_Factory);
		PE_D3D11_RELEASE(m_DeviceContext);
		PE_D3D11_RELEASE(m_Device);
	}

	void DirectX11Renderer::OnWindowEvent(int32 x, int32 y)
	{
		m_DeviceContext->Flush();
		m_Framebuffer->Unbind();
		PE_D3D11_SHUTDOWN(m_Framebuffer);
		m_SwapChain->Resize((uint32)x, (uint32)y);

		m_Framebuffer = new DirectX11Framebuffer((uint32)x, (uint32)y, true);
		m_Framebuffer->Bind();
	}

	void DirectX11Renderer::PostInit()
	{
		Window* window;
		window = Application::Get().GetWindow();

 		m_SwapChain = new DirectX11SwapChain(window);
		m_Framebuffer = new DirectX11Framebuffer((uint32)window->GetWidth(), (uint32)window->GetHeight(), true);

		m_Framebuffer->Bind();
	}

	void DirectX11Renderer::Present()
	{
		m_SwapChain->Present();
	}

	void DirectX11Renderer::Clear(Pawn::Core::Math::Vector4D32 color)
	{
		m_DeviceContext->ClearRenderTargetView(m_Framebuffer->GetRenderTargetView(), color.XYZW);
		m_DeviceContext->ClearDepthStencilView(m_Framebuffer->GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}

	void DirectX11Renderer::DrawIndexed(uint32 indexCount, uint32 index)
	{
		m_DeviceContext->DrawIndexed(indexCount, index, 0);
	}

	void DirectX11Renderer::BindBackBuffer()
	{
		m_Framebuffer->Bind();
	}

	void DirectX11Renderer::UnbindBackBuffer()
	{
		m_Framebuffer->Unbind();
	}

}