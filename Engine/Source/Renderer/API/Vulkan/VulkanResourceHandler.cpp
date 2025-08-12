#include "VulkanResourceHandler.h"
#include <set>

#include "VulkanFunctions.h"
#include "VulkanPipeline.h"
#include "VulkanCommandBuffer.h"
#include "VulkanRenderAPI.h"
#include "VulkanUniform.h"
#include "Renderer/RenderCommand.h"
#include "Renderer/Base/Buffer.h"

namespace ME::Render
{

	VulkanResourceHandler::VulkanResourceHandler(uint32 bufferCount)
		: m_DescriptorPool(nullptr), m_BufferCount(bufferCount)
	{
		Init();
	}

	VulkanResourceHandler::~VulkanResourceHandler()
	{
		Shutdown();
	}

	void VulkanResourceHandler::Shutdown()
	{
		for (auto element : m_Layouts)
		{
			vkDestroyDescriptorSetLayout(RenderCommand::Get()->As<VulkanRenderAPI>()->GetDevice(), element.vkLayout, nullptr);
			element.vkLayout = nullptr;
		}
		if (m_DescriptorPool != nullptr)
		{
			vkDestroyDescriptorPool(RenderCommand::Get()->As<VulkanRenderAPI>()->GetDevice(), m_DescriptorPool, nullptr);
			m_DescriptorPool = nullptr;
		}
	}

	uint32 VulkanResourceHandler::CreateLayout(const ME::Render::ResourceLayout& layout)
	{
		int32 index = GetLayoutIndex(layout);
		if (index >= 0) return index;

		DescriptorSetLayoutPair pair = {};
		pair.Layout = layout;
		pair.vkLayout = nullptr;

		int32 result = CreateDescriptorSetLayout(pair);
		if (ME_VK_FAILED(result))
		{
			ME_ASSERT(false, TEXT("Vulkan: failed to create descriptor set layout! Error: {0}"), result);
			return static_cast<uint32>(m_Layouts.GetSize());
		}

		m_Layouts.EmplaceBack(pair);

		return static_cast<uint32>(m_Layouts.GetSize() - 1);
	}

	uint32 VulkanResourceHandler::CreateResourceSet(uint32 layout)
	{
		VkDescriptorSet descSet = nullptr;
		CreateDescriptorSet(layout, &descSet);
		m_Sets.EmplaceBack(descSet);
		return m_Sets.GetSize() - 1;
	}

	uint32 VulkanResourceHandler::CreateResourceSet(const ME::Render::ResourceLayout& layout)
	{
		return CreateResourceSet(GetResourceLayoutIndex(layout));
	}

	uint32 VulkanResourceHandler::GetResourceLayoutIndex(const ME::Render::ResourceLayout& layout)
	{
		int32 index = GetLayoutIndex(layout);
		if (index < 0) return ~0u;
		return static_cast<uint32>(index);
	}

	void VulkanResourceHandler::WriteResource(ME::Core::Memory::Reference<ME::Render::Uniform> buffer)
	{
		VkDescriptorBufferInfo bufferInfo = {};
		bufferInfo.buffer = buffer->As<VulkanUniform>()->GetBuffer();
		bufferInfo.range = static_cast<VkDeviceSize>(buffer->GetSpecification().Size);
		bufferInfo.offset = 0;

		VkWriteDescriptorSet writeDesc = {};
		writeDesc.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDesc.dstSet = m_Sets[buffer->GetResourceSet()];
		writeDesc.dstBinding = buffer->GetSpecification().Binding;
		writeDesc.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		writeDesc.dstArrayElement = 0;
		writeDesc.descriptorCount = 1;
		writeDesc.pBufferInfo = &bufferInfo;
		writeDesc.pImageInfo = nullptr;
		writeDesc.pTexelBufferView = nullptr;
		writeDesc.pNext = nullptr;

		vkUpdateDescriptorSets(RenderCommand::Get()->As<VulkanRenderAPI>()->GetDevice(), 1, &writeDesc, 0, nullptr);
	}

	void VulkanResourceHandler::WriteResource(ME::Core::Memory::Reference<ME::Render::StorageBuffer> buffer)
	{
		return;
		VkDescriptorBufferInfo bufferInfo = {};
		bufferInfo.buffer = buffer->As<VulkanUniform>()->GetBuffer();
		bufferInfo.offset = static_cast<VkDeviceSize>(buffer->GetSpecification().Size);
		bufferInfo.offset = 0;

		VkWriteDescriptorSet writeDesc = {};
		writeDesc.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDesc.dstSet = m_Sets[buffer->GetResourceSet()];
		writeDesc.dstBinding = buffer->GetSpecification().Binding;
		writeDesc.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		writeDesc.dstArrayElement = 0;
		writeDesc.descriptorCount = 1;
		writeDesc.pBufferInfo = &bufferInfo;
		writeDesc.pImageInfo = nullptr;
		writeDesc.pTexelBufferView = nullptr;
		writeDesc.pNext = nullptr;

		vkUpdateDescriptorSets(RenderCommand::Get()->As<VulkanRenderAPI>()->GetDevice(), 1, &writeDesc, 0, nullptr);
	}

	void VulkanResourceHandler::BindResourceSet(ME::Core::Memory::Reference<Render::CommandBuffer> commandBuffer, ME::Core::Memory::Reference<Render::Pipeline> pipeline, uint32 set, uint32 setIndex)
	{
		vkCmdBindDescriptorSets(commandBuffer->As<VulkanCommandBuffer>()->GetBuffer(), pipeline->As<VulkanPipeline>()->GetPipelineBindPoint(), pipeline->As<VulkanPipeline>()->GetPipelineLayout(), set, 1, &m_Sets[setIndex], 0, nullptr);
	}

	ME::Core::Containers::Array<VkDescriptorSetLayout> VulkanResourceHandler::GetDescriptorSetLayouts(ME::Core::Containers::Array<uint32> layouts) const
	{
		ME::Core::Containers::Array<VkDescriptorSetLayout> result;

		for (const auto& layoutIndex : layouts)
		{
			if (layoutIndex > m_Layouts.GetSize())
			{
				ME_ERROR(TEXT("Layout with index {0}, doesn't exist!"), layoutIndex);
				return {};
			}
			result.EmplaceBack(m_Layouts[layoutIndex].vkLayout);
		}

		return result;
	}

	void VulkanResourceHandler::CreateDescriptorSet(uint32 layout, VkDescriptorSet* descriptor) const
	{
		*descriptor = nullptr;
		VkDescriptorSetAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = m_DescriptorPool;
		allocInfo.descriptorSetCount = 1;
		allocInfo.pSetLayouts = &m_Layouts[layout].vkLayout;

		VkResult result = vkAllocateDescriptorSets(RenderCommand::Get()->As<VulkanRenderAPI>()->GetDevice(), &allocInfo, descriptor);
		if (ME_VK_FAILED(result))
		{
			ME_ASSERT(false, TEXT("Vulkan: failed to allocate DescriptorSet! Error: {0}"), ME_VK_RETURN_V(result));
		}
	}

	void VulkanResourceHandler::DestroyDescriptor(VkDescriptorSet* descriptor) const
	{
		vkFreeDescriptorSets(RenderCommand::Get()->As<VulkanRenderAPI>()->GetDevice(), m_DescriptorPool, 1, descriptor);
	}

	int32 VulkanResourceHandler::CreateDescriptorSetLayout(DescriptorSetLayoutPair& layout)
	{
		constexpr VkDescriptorBindingFlags bindingFlags =
			VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT | VK_DESCRIPTOR_BINDING_UPDATE_UNUSED_WHILE_PENDING_BIT;
		ME::Core::Containers::Array<VkDescriptorSetLayoutBinding> bindings;
		ME::Core::Containers::Array<VkDescriptorBindingFlags> bindFlags;

		for (uint32 u = 0; u < layout.Layout.GetSize(); u++)
		{
			VkDescriptorSetLayoutBinding vkBinding = {};
			vkBinding.binding = u;
			vkBinding.descriptorType = ConvertResourceTypeVulkan(layout.Layout[u].Type);
			vkBinding.stageFlags = ConvertShaderStageVulkan(layout.Layout[u].Stage);
			vkBinding.descriptorCount = layout.Layout[u].MaxSize;
			bindings.EmplaceBack(vkBinding);
			bindFlags.EmplaceBack(bindingFlags);
		}

		VkDescriptorSetLayoutBindingFlagsCreateInfo bindingFlagsCreateInfo = {};
		bindingFlagsCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
		bindingFlagsCreateInfo.bindingCount = static_cast<uint32>(bindFlags.GetSize());
		bindingFlagsCreateInfo.pBindingFlags = bindFlags.Data();
		bindingFlagsCreateInfo.pNext = nullptr;

		VkDescriptorSetLayoutCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		createInfo.bindingCount = static_cast<uint32>(bindings.GetSize());
		createInfo.pBindings = bindings.Data();
		createInfo.pNext = &bindingFlagsCreateInfo;
		createInfo.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT;

		return ME_VK_RETURN_V(vkCreateDescriptorSetLayout(RenderCommand::Get()->As<VulkanRenderAPI>()->GetDevice(), &createInfo, nullptr, &layout.vkLayout));
	}

	void VulkanResourceHandler::Init()
	{
		ME::Core::Containers::Array<VkDescriptorPoolSize> poolSizes;
		
		VkDescriptorPoolSize poolSize = {};
		poolSize.descriptorCount = ME_VK_INITIAL_MAX_DESCRIPTOR_SETS * m_BufferCount;
		poolSize.type = VK_DESCRIPTOR_TYPE_SAMPLER;
		poolSizes.EmplaceBack(poolSize);

		poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSizes.EmplaceBack(poolSize);

		poolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		poolSizes.EmplaceBack(poolSize);

		poolSize.type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
		poolSizes.EmplaceBack(poolSize);

		poolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		poolSizes.EmplaceBack(poolSize);

		poolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		poolSizes.EmplaceBack(poolSize);

		poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
		poolSizes.EmplaceBack(poolSize);

		poolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
		poolSizes.EmplaceBack(poolSize);

		poolSize.type = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
		poolSizes.EmplaceBack(poolSize);

		VkDescriptorPoolCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		createInfo.poolSizeCount = static_cast<uint32>(poolSizes.GetSize());
		createInfo.pPoolSizes = poolSizes.Data();
		createInfo.maxSets = ME_VK_INITIAL_MAX_DESCRIPTOR_SETS;
		createInfo.flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT;

		VkResult result = vkCreateDescriptorPool(RenderCommand::Get()->As<VulkanRenderAPI>()->GetDevice(), &createInfo, nullptr, &m_DescriptorPool);
		if (ME_VK_FAILED(result))
		{
			ME_ASSERT(false, TEXT("Vulkan: Failed to create descriptor set pool! Error: {0}"), ME_VK_RETURN_V(result));
			Shutdown();
		}
	}

	int32 VulkanResourceHandler::GetLayoutIndex(const ME::Render::ResourceLayout& layout) const
	{
		for (uint32 i = 0; i < m_Layouts.GetSize(); ++i)
			if (m_Layouts[i].Layout == layout) return static_cast<uint32>(i);
		return -1;
	}
}
