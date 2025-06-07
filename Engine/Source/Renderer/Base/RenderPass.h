#pragma once

#include <Core.hpp>
#include <Core/Containers/Array.hpp>
#include <Core/Containers/String/String.hpp>

#include "Texture.h"
#include "Pipeline.h"
#include "Core/Math/Math.hpp"
#include "Core/Misc/Rect2D.hpp"

namespace Pawn::Render
{
	class CommandBuffer;
	class Framebuffer;
}

namespace Pawn::Render
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

	struct PAWN_API AttachmentSpecification
	{
		Pawn::Render::Format AttachmentFormat;
		Pawn::Render::StoreOperation StoreOp;
		Pawn::Render::LoadOperation LoadOp;
		Pawn::Render::ImageLayout InitialLayout;
		Pawn::Render::ImageLayout FinalLayout;
		bool IsDepth;
		bool IsStencil;
	};

	struct PAWN_API SubpassSpecification
	{
		Pawn::Render::PipelineBindPoint PipelineBindPoint;
		Pawn::Core::Containers::Array<uint64> InputAttachmentRefs;
		Pawn::Core::Containers::Array<uint64> ColorAttachmentRefs;
		uint64 DepthStencilAttachmentRef;
	};

	struct PAWN_API SubpassDependency
	{
		uint32 SubpassSrc;
		uint32 SubpassDst;
		PipelineStageFlags PipelineStageFlagsSrc;
		PipelineStageFlags PipelineStageFlagsDst;
		AccessFlags AccessFlagsSrc;
		AccessFlags AccessFlagsDst;
	};

	struct PAWN_API RenderPassSpecification
	{
		Pawn::Core::Containers::Array<AttachmentSpecification> AttachmentSpecs;
		Pawn::Core::Containers::Array<SubpassSpecification> SubpassSpecs;
		Pawn::Core::Containers::Array<SubpassDependency> SubpassDependencies;
		Pawn::Core::Containers::AnsiString DebugName;
	};

	union ClearValue
	{
		Pawn::Core::Math::Vector4D32 ColorClearValue;
		struct
		{
			float32 Depth;
			uint32 Stencil;
		} DepthClearValue;
	};

	struct RenderPassBeginInfo
	{
		Framebuffer* Framebuffer;
		Pawn::Core::Math::Rect2D RenderArea;
		Pawn::Core::Containers::Array<Pawn::Render::ClearValue> ClearValues;
	};

	class PAWN_API RenderPass
	{
	public:
		virtual ~RenderPass() = default;

		virtual void Begin(Pawn::Render::CommandBuffer* buffer, RenderPassBeginInfo& beginInfo) = 0;
		virtual void End(Pawn::Render::CommandBuffer* buffer) = 0;

	public:
		static RenderPass* Create(RenderPassSpecification& specification);

	private:
		static RenderPass* CreateDirectX11RenderPass(RenderPassSpecification& specification);
		static RenderPass* CreateVulkanRenderPass(RenderPassSpecification& specification);
		
	};

}
