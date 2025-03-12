#include "DirectX11Framebuffer.h"
#include "Renderer/RenderCommand.h"
#include "Renderer/API/DirectX11/DirectX11Renderer.h"
#include "Renderer/API/DirectX11/DirectX11SwapChain.h"

namespace Pawn::Render
{
	Framebuffer* Framebuffer::CreateDirectX11Framebuffer(uint32 x, uint32 y)
	{
		return new DirectX11Framebuffer(x, y, false);
	}

	DirectX11Framebuffer::DirectX11Framebuffer(uint32 x, uint32 y, bool usingBackBuffer)
	{
		m_Buffer = nullptr;
		m_DepthBuffer = nullptr;
		m_DepthStencilView = nullptr;
		m_RTV = nullptr;
		m_UsingBackBuffer = usingBackBuffer;
		Init(x, y);
	}
	DirectX11Framebuffer::~DirectX11Framebuffer()
	{
		Shutdown();
	}

	bool DirectX11Framebuffer::Bind()
	{
		DirectX11Renderer* render;
		render = static_cast<DirectX11Renderer*>(RenderCommand::Get());
		if (!render)
			return false;

		render->GetDeviceContext()->OMSetRenderTargets(1, &m_RTV, m_DepthStencilView);
		return true;
	}
	
	bool DirectX11Framebuffer::Unbind()
	{
		DirectX11Renderer* render;
		render = static_cast<DirectX11Renderer*>(RenderCommand::Get());
		if (!render)
			return false;
		render->GetDeviceContext()->OMSetRenderTargets(0, nullptr, nullptr);
		return true;
	}

	void DirectX11Framebuffer::Shutdown()
	{
		PE_D3D11_RELEASE(m_DepthStencilView);
		PE_D3D11_RELEASE(m_RTV);
		PE_D3D11_RELEASE(m_DepthBuffer);
		PE_D3D11_RELEASE(m_Buffer);
	}

	int32 DirectX11Framebuffer::Init(uint32 x, uint32 y)
	{
		HRESULT result;
		
		DirectX11Renderer* render;
		DirectX11SwapChain* swapChain;

		D3D11_TEXTURE2D_DESC bufferDesc							= {};
		D3D11_TEXTURE2D_DESC depthBufferDesc					= {};
		D3D11_DEPTH_STENCIL_DESC depthStencilDesc				= {};
		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc		= {};
		D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc		= {};
		
		render = static_cast<DirectX11Renderer*>(RenderCommand::Get());
		if (!render)
			return 1;

		swapChain = static_cast<DirectX11SwapChain*>(render->GetSwapChain());
		if (!swapChain)
			return 2;

		if (!m_UsingBackBuffer)
		{
			bufferDesc.Width = x;
			bufferDesc.Height = y;
			bufferDesc.MipLevels = 1;
			bufferDesc.ArraySize = 1;
			bufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			bufferDesc.SampleDesc.Count = 1;
			bufferDesc.SampleDesc.Quality = 0;
			bufferDesc.Usage = D3D11_USAGE_DEFAULT;
			bufferDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
			bufferDesc.CPUAccessFlags = 0;
			bufferDesc.MiscFlags = 0;

			renderTargetViewDesc.Format = bufferDesc.Format;
			renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
			renderTargetViewDesc.Texture2D.MipSlice = 0;

			result = render->GetDevice()->CreateTexture2D(&bufferDesc, nullptr, &m_Buffer);
			PE_D3D11_CHECK(result);

			result = render->GetDevice()->CreateRenderTargetView(m_Buffer, &renderTargetViewDesc, &m_RTV);
			PE_D3D11_CHECK(result);
		}
		else
		{
			result = render->GetDevice()->CreateRenderTargetView(swapChain->GetBackBuffer(), nullptr, &m_RTV);
			PE_D3D11_CHECK(result);
		}

		depthBufferDesc.Width = x;
		depthBufferDesc.Height = y;
		depthBufferDesc.MipLevels = 1;
		depthBufferDesc.ArraySize = 1;
		depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthBufferDesc.SampleDesc.Count = 1;
		depthBufferDesc.SampleDesc.Quality = 0;
		depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthBufferDesc.CPUAccessFlags = 0;
		depthBufferDesc.MiscFlags = 0;

		result = render->GetDevice()->CreateTexture2D(&depthBufferDesc, nullptr, &m_DepthBuffer);
		PE_D3D11_CHECK(result);

		depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		depthStencilViewDesc.Texture2D.MipSlice = 0;

		result = render->GetDevice()->CreateDepthStencilView(m_DepthBuffer, &depthStencilViewDesc, &m_DepthStencilView);
		PE_D3D11_CHECK(result);

		return result;
	}
}

//depthStencilDesc.DepthEnable = true;
		//depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		//depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
		//depthStencilDesc.StencilEnable = true;
		//depthStencilDesc.StencilReadMask = 0xFF;
		//depthStencilDesc.StencilWriteMask = 0xFF;

		//depthStencilDesc.FrontFace.StencilFailOp =			D3D11_STENCIL_OP_KEEP;
		//depthStencilDesc.FrontFace.StencilDepthFailOp =		D3D11_STENCIL_OP_INCR;
		//depthStencilDesc.FrontFace.StencilPassOp =			D3D11_STENCIL_OP_KEEP;
		//depthStencilDesc.FrontFace.StencilFunc =			D3D11_COMPARISON_ALWAYS;
		//depthStencilDesc.BackFace.StencilFailOp =			D3D11_STENCIL_OP_KEEP;
		//depthStencilDesc.BackFace.StencilDepthFailOp =		D3D11_STENCIL_OP_DECR;
		//depthStencilDesc.BackFace.StencilPassOp =			D3D11_STENCIL_OP_KEEP;
		//depthStencilDesc.BackFace.StencilFunc =				D3D11_COMPARISON_ALWAYS;

		//result = render->GetDevice()->CreateDepthStencilState(&depthStencilDesc, &m_DepthStencilState);
		//PE_D3D11_CHECK(result);