#pragma once

#include <Core.hpp>

#include "VulkanRenderer.h"
#include "Renderer/Base/RenderPass.h"

namespace ME::Render
{
	class MOON_API VulkanRenderPass : public RenderPass
	{
	public:
		VulkanRenderPass(RenderPassSpecification& specification);
		~VulkanRenderPass() override;

	public:
		void Shutdown() override;

		void Begin(CommandBuffer* buffer, RenderPassBeginInfo& beginInfo) override;
		void End(CommandBuffer* buffer) override;

	public:
		inline VkRenderPass GetRenderPass() { return m_Pass; };

	private:
		void Init(RenderPassSpecification& specification);

	private:
		VkRenderPass m_Pass;

	};
}