#pragma once

#include <Core.hpp>
#include "Renderer/Base/RenderPass.h"

namespace Pawn::Render
{
	class RenderPass;

	struct FramebufferSpecification
	{
		RenderPass* RenderPass;
		Pawn::Core::Math::Resolution2D<uint32> Resolution;
		Pawn::Core::Containers::Array<Texture*> Attachments;
		uint32 Layers;
	};

	class PAWN_API Framebuffer
	{
	public:
		virtual ~Framebuffer() = default;

	public:
		static Framebuffer* Create(FramebufferSpecification& specification);
		static Framebuffer* CreateDirectX11Framebuffer(uint32 x, uint32 y);
		static Framebuffer* CreateVulkanFramebuffer(FramebufferSpecification& specification);

	};
}

