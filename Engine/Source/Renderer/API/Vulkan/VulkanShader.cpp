#include "VulkanShader.hpp"
#include "VulkanResourceHandler.hpp"
#include "VulkanRenderAPI.hpp"
#include "VulkanFunctions.hpp"
#include "Renderer/RenderCommand.hpp"


namespace ME::Render
{
	ME::Core::Memory::Reference<Shader> Shader::CreateVulkan(const ShaderSpecification& specification)
	{
		auto object = ME::Core::Memory::MakeReference<Render::VulkanShader>(specification);
		return object;
	}

	VulkanShader::VulkanShader(const ShaderSpecification& specification)
		: m_Shader(nullptr), m_Specification(specification)
	{
		Init();
	}

	VulkanShader::~VulkanShader()
	{
		VulkanShader::Shutdown();
	}

	void VulkanShader::Shutdown()
	{
		if (m_Shader != nullptr)
		{
			vkDestroyShaderModule(ME::Render::RenderCommand::Get()->As<VulkanRenderAPI>()->GetDevice(), m_Shader, nullptr);
			m_Shader = nullptr;
		}
		if (m_Specification.CompiledShader.Bytecode != nullptr)
		{
			::operator delete(m_Specification.CompiledShader.Bytecode, m_Specification.CompiledShader.Size);
			m_Specification.CompiledShader.Bytecode = nullptr;
			m_Specification.CompiledShader.Size = 0;
		}
	}

	void VulkanShader::Init()
	{
		m_DebugName = m_Specification.DebugName;

		VkShaderModuleCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = m_Specification.CompiledShader.Size;
		createInfo.pCode = static_cast<uint32*>(m_Specification.CompiledShader.Bytecode);

		VkResult result = vkCreateShaderModule(Render::RenderCommand::Get()->As<VulkanRenderAPI>()->GetDevice(), &createInfo, nullptr, &m_Shader);
		if (ME_VK_FAILED(result))
			ME_ASSERT("Vulkan shader: failed to create shader! Error: {0}", static_cast<uint32>(result));

		Render::RenderCommand::Get()->As<VulkanRenderAPI>()->NameVulkanObject(m_DebugName, ME_VK_TO_UINT_HANDLE(m_Shader), VK_OBJECT_TYPE_SHADER_MODULE);

		CreateDescriptorSetLayout();
	}

	void VulkanShader::CreateDescriptorSetLayout()
	{
		for (uint32 i = 0; i < m_Specification.Layouts.Size(); i++)
			m_Layouts.EmplaceBack(RenderCommand::Get()->As<VulkanRenderAPI>()->GetVulkanResourceHandler()->CreateLayout(m_Specification.Layouts[i]));
	}
}
