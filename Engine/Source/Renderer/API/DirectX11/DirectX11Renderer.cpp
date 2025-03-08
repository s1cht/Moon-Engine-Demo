#include "pch.h"
#include "DirectX11Renderer.h"

#define PE_D3D11_CHECK(x) if (FAILED(x)) return x;

namespace Pawn::Render
{
	DirectX11Renderer::DirectX11Renderer()
	{
		PE_ASSERT(Init(), TEXT("DirectX 11: Initialization failed!"));
	}
	DirectX11Renderer::~DirectX11Renderer()
	{
		Shutdown();
	}
	int32 Pawn::Render::DirectX11Renderer::Init()
	{
		HRESULT result;
		IDXGIFactory* factory;
		IDXGIAdapter* adapter;
		IDXGIOutput* output;

		result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
		PE_D3D11_CHECK(result);

		result = factory->EnumAdapters(0, &adapter);
		PE_D3D11_CHECK(result);

		result = adapter->EnumOutputs(0, &output);
		PE_D3D11_CHECK(result);

		//result = output->GetDisplayModeList();

		return result;
	}
	void DirectX11Renderer::Shutdown()
	{
		if (m_device)
		{
			m_device->Release();
			m_device = nullptr;
		}
		if (m_deviceContext)
		{
			m_deviceContext->Release();
			m_deviceContext = nullptr;
		}
	}
	void DirectX11Renderer::SetClearColor(Pawn::Math::Vector4D color)
	{
	}
	void DirectX11Renderer::Clear()
	{
	}
	void DirectX11Renderer::DrawIndexed()
	{
	}
}