#include "VulkanShader.h"

#include "VulkanResourceHandler.h"
#include "VulkanRenderAPI.h"
#include "VulkanFunctions.h"
#include "Renderer/RenderCommand.h"
#include "Renderer/RenderResourcesTracker.hpp"

namespace ME::Render
{
	ME::Core::Memory::Reference<Shader> Shader::CreateVulkanShader(const ShaderSpecification& specification)
	{
		auto object = ME::Core::Memory::MakeReference<Render::VulkanShader>(specification);
		RenderResourcesTracker::Get().AddItem(object);
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
		VkShaderModuleCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = m_Specification.CompiledShader.Size;
		createInfo.pCode = static_cast<uint32*>(m_Specification.CompiledShader.Bytecode);

		VkResult result = vkCreateShaderModule(Render::RenderCommand::Get()->As<VulkanRenderAPI>()->GetDevice(), &createInfo, nullptr, &m_Shader);
		if (ME_VK_FAILED(result))
			ME_ASSERT(TEXT("Vulkan shader: failed to create shader! Error: {0}"), static_cast<uint32>(result));

		CreateDescriptorSetLayout();
	}

	void VulkanShader::CreateDescriptorSetLayout()
	{
		for (uint32 i = 0; i < m_Specification.Layouts.GetSize(); i++)
			m_Layouts.EmplaceBack(RenderCommand::Get()->As<VulkanRenderAPI>()->GetVulkanResourceHandler()->CreateLayout(m_Specification.Layouts[i]));
	}

	uint32 ConvertShaderTypeVulkan(ShaderType type)
	{
		switch (type)
		{
			case ME::Render::ShaderType::Float: return VK_FORMAT_R32_SFLOAT;
			case ME::Render::ShaderType::Float2: return VK_FORMAT_R32G32_SFLOAT;
			case ME::Render::ShaderType::Float3: return VK_FORMAT_R32G32B32_SFLOAT;
			case ME::Render::ShaderType::Float4: return VK_FORMAT_R32G32B32A32_SFLOAT;
			case ME::Render::ShaderType::Int: return VK_FORMAT_R32_SINT;
			case ME::Render::ShaderType::Int2: return VK_FORMAT_R32G32_SINT;
			case ME::Render::ShaderType::Int3: return VK_FORMAT_R32G32B32_SINT;
			case ME::Render::ShaderType::Int4: return VK_FORMAT_R32G32B32A32_SINT;
			case ME::Render::ShaderType::Bool: return VK_FORMAT_R8_UINT;
			case ME::Render::ShaderType::Uint: return VK_FORMAT_R32_UINT;
			case ME::Render::ShaderType::Uint2: return VK_FORMAT_R32G32_UINT;
			case ME::Render::ShaderType::Uint3: return VK_FORMAT_R32G32B32_UINT;
			case ME::Render::ShaderType::Uint4: return VK_FORMAT_R32G32B32A32_UINT;
			default:
			{
				ME_ASSERT(false, TEXT("Vulkan: ShaderTypeConversion: Shader type None can't be	converted!"));
				return VK_FORMAT_UNDEFINED;
			}
		}
	}
}
