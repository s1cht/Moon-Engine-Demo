#pragma once
#include <Core.hpp>

#include "Renderer/Base/RenderPass.hpp"

namespace ME::Render
{
	class RenderPass;

	struct FramebufferSpecification
	{
		Core::Memory::Reference<Render::RenderPass> RenderPass;
		ME::Core::Math::Resolution2D<uint32> Resolution;
		ME::Core::Array<ME::Core::Memory::Reference<Render::Texture2D>> Attachments;
		uint32 Layers;
	};

	class MEAPI Framebuffer : public RenderObject
	{
	public:
		virtual inline const FramebufferSpecification& GetSpecification() const = 0;

	public:
		static ME::Core::Memory::Reference<Render::Framebuffer> Create(FramebufferSpecification& specification);
		static ME::Core::Memory::Reference<Render::Framebuffer> CreateVulkanFramebuffer(FramebufferSpecification& specification);

	};
}

