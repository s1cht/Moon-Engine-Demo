#pragma once
#include <Core.hpp>
#include <Core/Memory/Memory.hpp>
#include <Core/Containers/Array.hpp>
#include <Core/Math/Math.hpp>
#include <Core/Math/Rect2D.hpp>

#include "RenderCore.hpp"
#include "Renderer/Managers/ShaderManager.hpp"

namespace ME::Render
{
	struct InputAssemblySpecification
	{
		PrimitiveTopology Topology;
		bool PrimitiveRestart;
	};

	struct RasterizationSpecification
	{
		bool DiscardEnabled;
		bool FrontCounterClockwise;
		bool DepthBiasEnabled;
		bool DepthClampEnabled;
		float32 DepthBiasConstantFactor;
		float32 DepthBiasClamp;
		float32 DepthBiasSlopeFactor;
		float32 LineWidth;
		RasterizationCull Cull;
		RasterizationFill Fill;
	};

	struct MultisamplingSpecification
	{
		bool EnableSampleShading;
		bool AlphaToOne;
		bool AlphaToCoverage;
		float32 MinSampleShading;
		SampleCount Samples;
	};

    struct StencilFaceSpecification
    {
		StencilOperation FailOp = StencilOperation::Keep;
		StencilOperation PassOp = StencilOperation::Keep;
		StencilOperation DepthFailOp = StencilOperation::Keep;
		DepthComparison CompareOp = DepthComparison::Always;

		uint32 CompareMask = 0xFF;
		uint32 WriteMask = 0xFF;
		uint32 Reference = 0;
    };

	struct DepthStencilSpecification
	{
		bool DepthEnabled;
		bool DepthWriteEnabled;
		DepthComparison DepthFunction;

		bool DepthBoundsEnabled;
		float32 MinDepthBounds;
		float32 MaxDepthBounds;

	    bool StencilEnabled;
		StencilFaceSpecification FrontFace = {};
		StencilFaceSpecification BackFace = {};
	};

	struct BlendAttachmentSpecification
	{
		bool EnableBlend;
		bool EnableAlphaMask;
		
	};

	struct ColorBlendingSpecification
	{
		ME::Core::Array<BlendAttachmentSpecification> Attachments;
		ME::Core::Math::Vector4D32 BlendConstants;
		LogicOperation LogicOperation;
	};

	struct PipelineSpecification
	{
		PipelineType Type;
		ME::Core::Memory::Reference<RenderPass> RenderPass;
		uint32 Subpass;
		PushConstants Constants;

		// Compute pipeline
	    ME::Core::Memory::Reference<ME::Render::Shader> ComputeShader;
		// Graphics pipeline
		Render::Manager::ShaderManager::ShaderGroup Shaders;
		VertexBufferLayout BufferLayout;
		InputAssemblySpecification InputAssembly;
		uint32 PatchControlPoints;
		RasterizationSpecification Rasterization;
		MultisamplingSpecification Multisampling;
		DepthStencilSpecification DepthStencil;
		ColorBlendingSpecification ColorBlending;
		ME::Core::Memory::Reference<Pipeline> BasePipeline;
	};

	struct ViewportSpecification
	{
		int32 X;
		int32 Y;
		uint32 Width;
		uint32 Height;
		int32 MinDepth;
		int32 MaxDepth;
	};

	class MEAPI Pipeline : public RenderBindable
	{
	public:
		virtual void SetViewports(ME::Core::Memory::Reference<ME::Render::CommandBuffer> buffer, ME::Core::Array<ME::Render::ViewportSpecification> specifications) = 0;
		virtual void SetScissors(ME::Core::Memory::Reference<ME::Render::CommandBuffer> buffer, ME::Core::Array<ME::Core::Math::Rect2D> scissors) = 0;
		virtual void SetConstants(ME::Core::Memory::Reference<ME::Render::CommandBuffer> buffer, ShaderStage shaderStage, void* constants,
			SIZE_T constantsSize) = 0;

		virtual inline const PipelineSpecification& GetSpecification() const = 0;

	public:
		static ME::Core::Memory::Reference<Pipeline> Create(const PipelineSpecification& specification);

	private:
		static ME::Core::Memory::Reference<Pipeline> CreateVulkan(const PipelineSpecification& specification);
	};
}

