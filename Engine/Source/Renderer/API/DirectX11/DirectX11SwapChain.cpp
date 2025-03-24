#include "DirectX11SwapChain.h"
#include "Renderer/RenderCommand.h"
#include "Platform/Windows/Win32Window.h"
#include "Renderer/API/DirectX11/DirectX11Renderer.h"

namespace Pawn::Render
{
	SwapChain* SwapChain::CreateDirectX11SwapChain(Window* window)
	{
		return new DirectX11SwapChain(window);
	}

	DirectX11SwapChain::DirectX11SwapChain(Window* window)
	{
		m_VSYNCEnabled = false;
		Init(window);
	}

	DirectX11SwapChain::~DirectX11SwapChain()
	{
		Shutdown();
	}

	void DirectX11SwapChain::Shutdown()
	{
		PE_D3D11_RELEASE(m_SwapChain);
	}

	void DirectX11SwapChain::Present()
	{
		if (m_VSYNCEnabled)
			m_SwapChain->Present(1, 0);
		else
			m_SwapChain->Present(0, 0);
	}

	void DirectX11SwapChain::Resize(uint32 x, uint32 y)
	{
		PE_D3D11_RELEASE(m_BackBuffer);
		m_SwapChain->ResizeBuffers(0, x, y, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
		m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&m_BackBuffer);
	}

	void DirectX11SwapChain::SetFullscreen(bool fullscreen)
	{
		m_SwapChain->SetFullscreenState(fullscreen, nullptr);
	}

	void DirectX11SwapChain::SetVSync(bool enabled)
	{
		m_VSYNCEnabled = enabled;
	}

	int32 DirectX11SwapChain::Init(Window* inWindow)
	{
		HRESULT result;
		Win32Window* window;
		DirectX11Renderer* render;

		window = static_cast<Win32Window*>(inWindow);
		if (!window)
			return 1;

		render = static_cast<DirectX11Renderer*>(RenderCommand::Get());
		if (!render)
			return 2;

		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
		DXGI_SWAP_CHAIN_FULLSCREEN_DESC swapChainFSDesc = {};

		swapChainDesc.BufferCount = 1;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.Stereo = false;
		swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
		swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
		swapChainDesc.Width = window->GetWidth();
		swapChainDesc.Height = window->GetHeight();
		swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_DISPLAY_ONLY;

		// Multisampling
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;

		// Fullscreen description
		swapChainFSDesc.RefreshRate.Numerator = 0;
		swapChainFSDesc.RefreshRate.Denominator = 1;
		swapChainFSDesc.Scaling = DXGI_MODE_SCALING_STRETCHED;
		swapChainFSDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE;
		swapChainFSDesc.Windowed = true;

		result = render->GetFactory()->CreateSwapChainForHwnd(render->GetDevice(), window->GetWindowHandle(), &swapChainDesc, &swapChainFSDesc, nullptr, &m_SwapChain);
		PE_D3D11_CHECK(result);

		result = m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&m_BackBuffer);
		PE_D3D11_CHECK(result);

		return result;
	}

}