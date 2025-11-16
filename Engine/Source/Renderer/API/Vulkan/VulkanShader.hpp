#pragma once

#include <Core.hpp>

#include "Vulkan.hpp"
#include "Renderer/Base/Shader.hpp"

namespace ME::Render
{
	class MEAPI VulkanShader : public Shader
	{
	public:
		VulkanShader(const ShaderSpecification& specification);
		~VulkanShader() override;

	public:
		void Shutdown() override;

		const ME::Render::ShaderSpecification& GetSpecification() const override { return m_Specification; };

	public:
		inline VkShaderModule GetShaderModule() const { return m_Shader; }
		inline ME::Core::Array<uint32> GetDescriptorSetLayouts() { return m_Layouts; }

	private:
		void Init();
		void CreateDescriptorSetLayout();

	private:
		VkShaderModule m_Shader;
		ME::Core::Array<uint32> m_Layouts;

	private:
		ShaderSpecification m_Specification;
	};
}