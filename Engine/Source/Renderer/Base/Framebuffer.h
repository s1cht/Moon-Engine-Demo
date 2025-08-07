#pragma once

#include <Core.hpp>
#include "Renderer/Base/RenderPass.h"

namespace ME::Render
{
	class RenderPass;

	struct FramebufferSpecification
	{
		Core::Memory::Reference<Render::RenderPass> RenderPass;
		ME::Core::Math::Resolution2D<uint32> Resolution;
		ME::Core::Containers::Array<Texture2D*> Attachments;
		uint32 Layers;
	};

	class MOON_API Framebuffer : public RenderObject
	{
	public:
		virtual ~Framebuffer() = default;

		virtual inline const FramebufferSpecification& GetSpecification() const = 0;

	public:
		static ME::Core::Memory::Reference<Render::Framebuffer> Create(FramebufferSpecification& specification);
		static ME::Core::Memory::Reference<Render::Framebuffer> CreateVulkanFramebuffer(FramebufferSpecification& specification);

	};
}

