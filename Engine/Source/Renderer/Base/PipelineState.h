#pragma once

#include <Core.h>

#include "Renderer/Base/Buffer.h"
#include "Renderer/Base/Shader.h"

import Pawn.Core.Memory;
import Pawn.Core.Container.Array;

namespace Pawn::Render
{
	class Shader;

	enum class PrimitiveTopology
	{
		None = 0,
		PointList,
		LineList,
		LineStrip,
		TriangleList,
		TriangleStrip,
		TriangleFan,
		PatchList
	};

	enum class RasterizerFill
	{
		Fill = 0,
		Wireframe = 1,
	};

	enum class RasterizerCull
	{
		None = 0,
		Back = 1,
		Front = 2,
	};

	enum class BlendMask
	{
		All = 0,
		Red = 1, Green = 2, Blue = 3, Alpha = 4,
	};

	enum class DepthComparison
	{
		None = 0,
		Less = 1, Greater = 2,
		LessEqual = 3, GreaterEqual = 4,
		NotEqual = 5, Equal = 6,
		Always = 7
	};

	enum class InputClassification
	{
		None = 0,
		PerVertex, PerInstance
	};

	class PAWN_API PipelineState
	{
	public:
		virtual ~PipelineState() {};

		virtual void SetInputLayout(BufferLayout& layout, InputClassification inputSlotClass, uint32 instanceDataStepRate) = 0;

		virtual void SetPrimitiveTopology(PrimitiveTopology topology, uint8 patchListPointCount) = 0;

		virtual void SetDepthStencilState(bool depthEnabled, bool stencilEnable, DepthComparison depthFunc) = 0;

		virtual void SetBlendState(bool enableBlend, BlendMask mask) = 0;

		virtual void SetViewport(uint32 x, uint32 y) = 0;

		virtual void SetRasterizerState(RasterizerCull cull, RasterizerFill fill, 
			bool frontCounterClockwise, bool scissorEnabled, bool depthClipEnabled,
			bool slopeScaledDepthBias, int32 depthBias, float32 depthBiasClamp,
			bool multisampleEnabled, int32 sampleCount ) = 0;
		

		virtual void SetVertexShader(Pawn::Core::Memory::Reference<Shader> vertexShader) = 0;
		virtual void SetPixelShader(Pawn::Core::Memory::Reference<Shader> pixelShader) = 0;
		virtual void SetComputeShader(Pawn::Core::Memory::Reference<Shader> computeShader) = 0;
		virtual void SetGeometryShader(Pawn::Core::Memory::Reference<Shader> geometryShader) = 0;
		virtual void SetHullShader(Pawn::Core::Memory::Reference<Shader> hullShader) = 0;
		virtual void SetDomainShader(Pawn::Core::Memory::Reference<Shader> domainShader) = 0;

		virtual void BindInput() = 0;
		virtual void Bind() = 0;
		virtual void BindUniforms(Pawn::Core::Containers::Array<Uniform*>& uniforms, Shader::Type stage) = 0;

	public:
		static PipelineState* Create();

	private:
		static PipelineState* CreateDirectX11PipelineState();
		static PipelineState* CreateDirectX12PipelineState();
		static PipelineState* CreateVulkanPipelineState();
		//...

	};

}

