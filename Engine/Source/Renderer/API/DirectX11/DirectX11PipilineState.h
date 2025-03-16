#pragma once

#include <Core.h>

#include "Renderer/Base/PipelineState.h"
#include "Platform/Platform.h"
#include "Renderer/API/DirectX11/DirectX11Buffer.h"
#include "Renderer/API/DirectX11/DirectX11Shader.h"

namespace Pawn::Render
{
	class PAWN_API DirectX11PipelineState : public PipelineState
	{
	public:
		DirectX11PipelineState();
		~DirectX11PipelineState();

	public:
		void SetInputLayout(BufferLayout& layout, InputClassification inputSlotClass, uint32 instanceDataStepRate) override;
		void SetPrimitiveTopology(PrimitiveTopology topology, uint8 patchListPointCount) override;
		void SetDepthStencilState(bool depthEnabled, bool stencilEnable, DepthComparison depthFunc) override;
		void SetBlendState(bool enableBlend, BlendMask mask) override;
		void SetRasterizerState(RasterizerCull cull, RasterizerFill fill,
			bool frontCounterClockwise, bool scissorEnabled,
			bool slopeScaledDepthBias, int32 depthBias, float32 depthBiasClamp,
			bool multisampleEnabled, int32 sampleCount) override;

		void SetViewport(uint32 x, uint32 y) override;

		void SetVertexShader(Memory::Reference<Shader> vertexShader) override;
		void SetPixelShader(Memory::Reference<Shader> pixelShader) override;
		void SetComputeShader(Memory::Reference<Shader> computeShader) override;
		void SetGeometryShader(Memory::Reference<Shader> geometryShader) override;
		void SetHullShader(Memory::Reference<Shader> hullShader) override;
		void SetDomainShader(Memory::Reference<Shader> domainShader) override;

		void Bind() override;
		void BindUniforms(Array<Uniform*>& uniforms, Shader::Type stage) override;

	private:
		D3D11_VIEWPORT m_Viewport;
		Memory::Scope<ID3D11InputLayout> m_InputLayout;
		Memory::Scope<ID3D11DepthStencilState> m_DepthStencilState;
		Memory::Scope<ID3D11RasterizerState> m_RasterizerState;
		Memory::Scope<ID3D11BlendState> m_BlendState;

	private:
		Memory::Reference<DirectX11Shader> m_VertexShader;
		Memory::Reference<DirectX11Shader> m_PixelShader;
		Memory::Reference<DirectX11Shader> m_ComputeShader;
		Memory::Reference<DirectX11Shader> m_GeometryShader;
		Memory::Reference<DirectX11Shader> m_HullShader;
		Memory::Reference<DirectX11Shader> m_DomainShader;

	};
}

