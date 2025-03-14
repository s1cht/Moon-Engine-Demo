#include "DirectX11PipilineState.h"

#include "Renderer/API/DirectX11/DirectX11Renderer.h"
#include "Renderer/RenderCommand.h"
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

namespace Pawn::Render
{
	PipelineState* PipelineState::CreateDirectX11PipelineState()
	{
		//return new DirectX11PipilineState();
		return nullptr;
	}

	DirectX11PipilineState::DirectX11PipilineState()
	{
		Init();
	}

	DirectX11PipilineState::~DirectX11PipilineState()
	{
	}

	void DirectX11PipilineState::SetDepthStencilState(bool depthEnabled, bool stencilEnable, DepthComparison depthFunc)
	{
	}

	void DirectX11PipilineState::SetBlendState(bool enableBlend, BlendMask mask)
	{
	}

	void DirectX11PipilineState::SetRasterizerState(RasterizerCull cull, RasterizerFill fill, bool frontCounterClockwide, int32 depthBias, float32 depthBiasClamp)
	{
	}

	void DirectX11PipilineState::SetInputLayout(Shader* vertexShader, BufferLayout& layout)
	{
	}

	void DirectX11PipilineState::Bind()
	{
		DirectX11Renderer* render = static_cast<DirectX11Renderer*>(RenderCommand::Get());

		render->GetDeviceContext()->RSSetState(m_RasterizerState);
		render->GetDeviceContext()->OMSetDepthStencilState(m_DepthStencilState, 1);
		render->GetDeviceContext()->OMSetBlendState(m_BlendState, nullptr, 0xFFFFFFFF);
	}

	void DirectX11PipilineState::Init()
	{
		
	}

	void DirectX11PipilineState::SetPrimitiveTopology(PrimitiveTopology topology, uint8 patchListPointCount)
	{

	}

}