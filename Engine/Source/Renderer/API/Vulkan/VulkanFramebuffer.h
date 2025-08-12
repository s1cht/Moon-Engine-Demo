#pragma once

#include <Core.hpp>

#include "Vulkan.hpp"
#include "Renderer/Base/Framebuffer.h"

namespace ME::Render
{
	class MOON_API VulkanFramebuffer : public Framebuffer
	{
	public:
		VulkanFramebuffer(const FramebufferSpecification& specification);
		~VulkanFramebuffer() override;

	public:
		void Shutdown() override;

	public:
		inline VkFramebuffer GetFramebuffer() { return m_Buffer; }
		inline const FramebufferSpecification& GetSpecification() const override { return m_Specification; }

	private:
		void Init();

	private:
		VkFramebuffer m_Buffer;

		FramebufferSpecification m_Specification;
	};

}