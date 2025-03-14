#pragma once

#include <Core/CoreTypes.h>

#include "Renderer/Base/Buffer.h"
//#include "Renderer/Base/Shader.h"

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

	class PipelineState
	{
	public:
		virtual void SetInputLayout(Shader* vertexShader, BufferLayout& layout) = 0;
		virtual void SetPrimitiveTopology(PrimitiveTopology topology, uint8 patchListPointCount) = 0;
		virtual void SetDepthStencilState(bool depthEnabled, bool stencilEnable, DepthComparison depthFunc) = 0;
		virtual void SetBlendState(bool enableBlend, BlendMask mask) = 0;
		virtual void SetRasterizerState(RasterizerCull cull, RasterizerFill fill, bool frontCounterClockwide, int32 depthBias, float32 depthBiasClamp) = 0;
		
		virtual void Bind() = 0;

	public:
		static PipelineState* Create();

	private:
		static PipelineState* CreateDirectX11PipelineState();
		static PipelineState* CreateDirectX12PipelineState();
		static PipelineState* CreateVulkanPipelineState();
		//...

	};

}

