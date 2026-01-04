#pragma once

#include <Core.hpp>
#include <Core/Containers/Array.hpp>
#include <Core/Containers/String.hpp>

#include "Texture.hpp"
#include "Pipeline.hpp"
#include "Core/Math/Math.hpp"
#include "Core/Math/Rect2D.hpp"

namespace ME::Render
{
	struct AttachmentSpecification
	{
		ME::Render::Format AttachmentFormat;
		ME::Render::StoreOperation StoreOp;
		ME::Render::LoadOperation LoadOp;
		ME::Render::ImageLayout InitialLayout;
		ME::Render::ImageLayout FinalLayout;
		ME::Render::SampleCount SampleCount;
		bool IsDepth;
		bool IsStencil;
	};

	struct SubpassSpecification
	{
		ME::Render::PipelineBindPoint PipelineBindPoint;
		ME::Core::Array<uint64> InputAttachmentRefs;
		ME::Core::Array<uint64> ColorAttachmentRefs;
		uint64 DepthStencilAttachmentRef;
	};

	struct SubpassDependency
	{
		uint32 SubpassSrc;
		uint32 SubpassDst;
		PipelineStageFlags PipelineStageFlagsSrc;
		PipelineStageFlags PipelineStageFlagsDst;
		AccessFlags AccessFlagsSrc;
		AccessFlags AccessFlagsDst;
	};

	struct RenderPassSpecification
	{
		ME::Core::Array<AttachmentSpecification> AttachmentSpecs;
		ME::Core::Array<SubpassSpecification> SubpassSpecs;
		ME::Core::Array<SubpassDependency> SubpassDependencies;
		ME::Core::String DebugName;
	};

	struct DepthStencilClearValue
	{
		float32 Depth;
		uint32 Stencil;
	};

	struct ClearValue
	{
		ME::Core::Math::Vector4D32 ColorClearValue;
		DepthStencilClearValue DepthClearValue;
		bool UsingDepth = false;
	};

	struct RenderPassBeginInfo
	{
		ME::Core::Memory::Reference<Framebuffer> Framebuffer;
		ME::Core::Math::Rect2D RenderArea;
		ME::Core::Array<ME::Render::ClearValue> ClearValues;
	};

	class MEAPI RenderPass : public RenderObject
	{
		ME_RENDER_OBJECT_TYPE(RenderPass);

	public:
		virtual void Begin(ME::Core::Memory::Reference<ME::Render::CommandBuffer> buffer, RenderPassBeginInfo& beginInfo) = 0;
		virtual void End(ME::Core::Memory::Reference<ME::Render::CommandBuffer> buffer) = 0;

		virtual const RenderPassSpecification& GetSpecification() const = 0;

	public:
		static Core::Memory::Reference<Render::RenderPass> Create(const RenderPassSpecification& specification);

	private:
		static Core::Memory::Reference<Render::RenderPass> CreateVulkan(const RenderPassSpecification& specification);
	};
}
