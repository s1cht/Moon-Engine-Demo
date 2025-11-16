#pragma once
#include <Core.hpp>

#include "Vulkan.hpp"
#include "Renderer/Base/Pipeline.hpp"

namespace ME::Render
{
	class MEAPI VulkanPipeline final : public Pipeline
	{
	public:
		VulkanPipeline(const PipelineSpecification& specification);
		~VulkanPipeline() override;

	public:
		void Shutdown() override;

		void SetViewports(ME::Core::Memory::Reference<ME::Render::CommandBuffer> buffer, ME::Core::Array<ME::Render::ViewportSpecification> specifications) override;
		void SetScissors(ME::Core::Memory::Reference<ME::Render::CommandBuffer> buffer, ME::Core::Array<ME::Core::Math::Rect2D> scissors) override;
        void SetConstants(ME::Core::Memory::Reference<ME::Render::CommandBuffer> buffer, ShaderStage shaderStage, void* constants,
			SIZE_T constantsSize) override;

		void Bind(ME::Core::Memory::Reference<ME::Render::CommandBuffer> buffer) override;

		inline const PipelineSpecification& GetSpecification() const override { return m_Specification; };

	public:
		inline VkPipeline GetPipeline() const { return m_Pipeline; }
		inline VkPipelineLayout GetPipelineLayout() const { return m_PipelineLayout; }
		inline VkPipelineBindPoint GetPipelineBindPoint() const { return m_PipelineBindPoint; }

	private:
		void Init();

		ME::Core::Array<VkPipelineShaderStageCreateInfo> FormatPipelineShaderStageCI();
		ME::Core::Array<VkPushConstantRange> FormatPushConstantRange();
		void CreatePipelineLayout();
		void CreateGraphicsPipeline();
		void CreateComputePipeline();

    private:
		VkPipeline m_Pipeline;
		VkPipelineLayout m_PipelineLayout;
		VkPipelineBindPoint m_PipelineBindPoint;

	private:
		PipelineSpecification m_Specification;

	};
}