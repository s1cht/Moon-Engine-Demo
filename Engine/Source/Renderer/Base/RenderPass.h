#pragma once

#include <Core.hpp>
#include <Core/Containers/Array.hpp>
#include <Core/Containers/String/String.hpp>

#include "Texture.h"
#include "Pipeline.h"
#include "Core/Math/Math.hpp"
#include "Core/Misc/Rect2D.hpp"

namespace ME::Render
{
	class CommandBuffer;
	class Framebuffer;
}

namespace ME::Render
{
	enum class StoreOperation : uint8
	{
		None = 0,
		DontCare,
		Store,
	};

	enum class LoadOperation : uint8
	{
		None = 0,
		DontCare,
		Load, Clear,
	};

	struct MEAPI AttachmentSpecification
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

	struct MEAPI SubpassSpecification
	{
		ME::Render::PipelineBindPoint PipelineBindPoint;
		ME::Core::Containers::Array<uint64> InputAttachmentRefs;
		ME::Core::Containers::Array<uint64> ColorAttachmentRefs;
		uint64 DepthStencilAttachmentRef;
	};

	struct MEAPI SubpassDependency
	{
		uint32 SubpassSrc;
		uint32 SubpassDst;
		PipelineStageFlags PipelineStageFlagsSrc;
		PipelineStageFlags PipelineStageFlagsDst;
		AccessFlags AccessFlagsSrc;
		AccessFlags AccessFlagsDst;
	};

	struct MEAPI RenderPassSpecification
	{
		ME::Core::Containers::Array<AttachmentSpecification> AttachmentSpecs;
		ME::Core::Containers::Array<SubpassSpecification> SubpassSpecs;
		ME::Core::Containers::Array<SubpassDependency> SubpassDependencies;
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
		ME::Core::Memory::Reference<Render::RenderPass> RenderPass;
		ME::Core::Math::Rect2D RenderArea;
		ME::Core::Containers::Array<ME::Render::ClearValue> ClearValues;
	};

	class MEAPI RenderPass : public RenderObject
	{
	public:
		virtual void Begin(ME::Render::CommandBuffer* buffer, RenderPassBeginInfo& beginInfo) = 0;
		virtual void End(ME::Render::CommandBuffer* buffer) = 0;

	public:
		static Core::Memory::Reference<Render::RenderPass> Create(RenderPassSpecification& specification);

	private:
		static Core::Memory::Reference<Render::RenderPass> CreateVulkanRenderPass(RenderPassSpecification& specification);
		
	};

}
