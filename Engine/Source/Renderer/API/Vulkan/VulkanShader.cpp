#include "VulkanShader.h"

#include "VulkanMacros.hpp"
#include "VulkanRenderer.h"
#include "Renderer/RenderCommand.h"
#include "Renderer/RenderResourcesTracker.hpp"

namespace ME::Render
{
	ME::Core::Memory::Reference<Shader> Shader::CreateVulkanShader(const ME::Render::CompiledShader& compiledShader, Shader::Type shaderType)
	{
		auto object = ME::Core::Memory::Reference<Render::Shader>(new VulkanShader(compiledShader, shaderType));
		RenderResourcesTracker::Get().AddItem(object);
		return object;
	}

	VulkanShader::VulkanShader(const ME::Render::CompiledShader& compiledShader, Shader::Type shaderType)
	{
		Init(compiledShader, shaderType);
	}

	VulkanShader::~VulkanShader()
	{
		Shutdown();
	}

	void VulkanShader::Shutdown()
	{
		if (m_Shader != nullptr)
		{
			vkDestroyShaderModule(ME::Render::RenderCommand::Get()->As<VulkanRenderer>()->GetDevice(), m_Shader, nullptr);
			m_Shader = nullptr;
		}
		if (m_CompiledShader.Bytecode != nullptr)
		{
			::operator delete(m_CompiledShader.Bytecode, m_CompiledShader.Size);
			m_CompiledShader.Bytecode = nullptr;
			m_CompiledShader.Size = 0;
		}
	}

	void VulkanShader::Init(const ME::Render::CompiledShader& compiledShader, Shader::Type shaderType)
	{
		VkResult result;
		m_CompiledShader = compiledShader;
		m_Type = shaderType;

		VkShaderModuleCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = m_CompiledShader.Size;
		createInfo.pCode = static_cast<uint32*>(m_CompiledShader.Bytecode);

		result = vkCreateShaderModule(Render::RenderCommand::Get()->As<VulkanRenderer>()->GetDevice(), &createInfo, nullptr, &m_Shader);
		if (ME_VK_FAILED(result))
			ME_ASSERT(TEXT("Vulkan shader: failed to create shader! Error: {0}"), (uint32)result);
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
			case ME::Render::ShaderType::None:
			{
				ME_ASSERT(false, TEXT("Vulkan: ShaderTypeConversion: Shader type None can't be	converted!"));
				return VK_FORMAT_UNDEFINED;
			}
		}
	}
}
