#pragma once

#include <Core.hpp>

#include "Vulkan.hpp"
#include "Renderer/Base/RenderPass.hpp"

namespace ME::Render
{
	class MEAPI VulkanRenderPass final : public RenderPass
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

    public:
		const RenderPassSpecification& GetSpecification() const override { return m_Specification; };

    private:
		VkRenderPass m_Pass;
		RenderPassSpecification m_Specification;
	};
}