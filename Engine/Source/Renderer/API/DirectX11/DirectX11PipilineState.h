#pragma once

#include <Core.h>

#include "Renderer/Base/PipelineState.h"
#include "Platform/Platform.h"


namespace Pawn::Render
{
	class DirectX11Shader;
	class DirectX11VertexBuffer;

	class DirectX11PipilineState : public PipelineState
	{
	public:
		DirectX11PipilineState();
		~DirectX11PipilineState();

	public:
		virtual void SetInputLayout(BufferLayout& layout, InputClassification inputSlotClass, uint32 instanceDataStepRate) override;
		virtual void SetPrimitiveTopology(PrimitiveTopology topology, uint8 patchListPointCount) override;
		virtual void SetDepthStencilState(bool depthEnabled, bool stencilEnable, DepthComparison depthFunc) override;
		virtual void SetBlendState(bool enableBlend, BlendMask mask) override;
		virtual void SetRasterizerState(RasterizerCull cull, RasterizerFill fill, bool frontCounterClockwise, int32 depthBias, float32 depthBiasClamp) override;

		virtual void SetVertexShader(Memory::Reference<Shader> vertexShader) override;
		virtual void SetPixelShader(Memory::Reference<Shader> pixelShader) override;

		virtual void Bind() override;

	private:
		Memory::Scope<ID3D11InputLayout> m_InputLayout;
		Memory::Scope<ID3D11DepthStencilState> m_DepthStencilState;
		Memory::Scope<ID3D11RasterizerState> m_RasterizerState;
		Memory::Scope<ID3D11BlendState> m_BlendState;

	private:
		Memory::Reference<Shader> m_VertexShader;
		Memory::Reference<Shader> m_PixelShader;

	};
}

