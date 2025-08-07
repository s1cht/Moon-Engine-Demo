#pragma once

#include <Core.hpp>
#include <vulkan/vulkan.hpp>

#include "Renderer/Base/Shader.h"


namespace ME::Render
{
	class MOON_API VulkanShader : public Shader
	{
	public:
		VulkanShader(const ME::Render::CompiledShader& compiledShader, Shader::Type shaderType);
		~VulkanShader() override;

	public:
		void Shutdown() override;

		inline ME::Render::CompiledShader GetCompiledShader() override { return m_CompiledShader; }
		constexpr Shader::Type GetShaderType() const override { return m_Type; }

	public:
		inline VkShaderModule GetShaderModule() const { return m_Shader; }

	private:
		void Init(const ME::Render::CompiledShader& compiledShader, Shader::Type shaderType);

	private:
		Shader::Type m_Type;
		ME::Render::CompiledShader m_CompiledShader;

		VkShaderModule m_Shader;

	};
}