#pragma once

#include "VulkanRenderer.h"
#include "Renderer/Base/Framebuffer.h"

namespace Pawn::Render
{
	class PAWN_API VulkanFramebuffer : public Framebuffer
	{
	public:
		VulkanFramebuffer(FramebufferSpecification& specification);
		~VulkanFramebuffer() override;

	public:
		inline VkFramebuffer GetFramebuffer() { return m_Buffer; }

	private:
		void Init(FramebufferSpecification& specification);

	private:
		VkFramebuffer m_Buffer;

	};

}