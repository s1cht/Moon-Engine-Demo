#pragma once

#include "Renderer/Base/Pipeline.h"
#include "Platform/Platform.h"
#include "Renderer/API/DirectX11/DirectX11Buffer.h"
#include "Renderer/API/DirectX11/DirectX11Shader.h"

namespace Pawn::Render
{
//	class PAWN_API DirectX11Pipeline : public Pipeline
//	{
//	public:
//		DirectX11Pipeline();
//		~DirectX11Pipeline();
//
//	public:
//		void SetInputLayout(BufferLayout& layout, InputClassification inputSlotClass, uint32 instanceDataStepRate) override;
//		void SetPrimitiveTopology(PrimitiveTopology topology, uint8 patchListPointCount) override;
//		void SetDepthStencilState(bool depthEnabled, bool stencilEnable, DepthComparison depthFunc) override;
//		void SetBlendState(bool enableBlend, BlendMask mask) override;
//		void SetRasterizerState(RasterizerCull cull, RasterizerFill fill,
//			bool frontCounterClockwise, bool scissorEnabled, bool depthClipEnabled,
//			bool slopeScaledDepthBias, int32 depthBias, float32 depthBiasClamp,
//			bool multisampleEnabled, int32 sampleCount) override;
//
//		void SetViewport(uint32 x, uint32 y) override;
//
//		void SetVertexShader(Pawn::Core::Memory::Reference<Shader> vertexShader) override;
//		void SetPixelShader(Pawn::Core::Memory::Reference<Shader> pixelShader) override;
//		void SetComputeShader(Pawn::Core::Memory::Reference<Shader> computeShader) override;
//		void SetGeometryShader(Pawn::Core::Memory::Reference<Shader> geometryShader) override;
//		void SetHullShader(Pawn::Core::Memory::Reference<Shader> hullShader) override;
//		void SetDomainShader(Pawn::Core::Memory::Reference<Shader> domainShader) override;
//
//		void BindInput() override;
//		void Bind() override;
//		void BindUniforms(Pawn::Core::Containers::Array<Uniform*>& uniforms, Shader::Type stage) override;
//
//	private:
//		D3D11_VIEWPORT m_Viewport;
//		Pawn::Core::Memory::Scope<ID3D11InputLayout> m_InputLayout;
//		Pawn::Core::Memory::Scope<ID3D11DepthStencilState> m_DepthStencilState;
//		Pawn::Core::Memory::Scope<ID3D11RasterizerState> m_RasterizerState;
//		Pawn::Core::Memory::Scope<ID3D11BlendState> m_BlendState;
//
//	private:
//		Pawn::Core::Memory::Reference<DirectX11Shader> m_VertexShader;
//		Pawn::Core::Memory::Reference<DirectX11Shader> m_PixelShader;
//		Pawn::Core::Memory::Reference<DirectX11Shader> m_ComputeShader;
//		Pawn::Core::Memory::Reference<DirectX11Shader> m_GeometryShader;
//		Pawn::Core::Memory::Reference<DirectX11Shader> m_HullShader;
//		Pawn::Core::Memory::Reference<DirectX11Shader> m_DomainShader;
//
//	};
}

