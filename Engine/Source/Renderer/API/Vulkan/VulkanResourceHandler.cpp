#include "VulkanResourceHandler.hpp"

#include "VulkanFunctions.hpp"
#include "VulkanPipeline.hpp"
#include "VulkanCommandBuffer.hpp"
#include "VulkanRenderAPI.hpp"
#include "VulkanTexture.hpp"
#include "VulkanUniform.hpp"
#include "Renderer/RenderCommand.h"
#include "Renderer/Base/Buffer.hpp"

namespace ME::Render
{
	ME::Core::Memory::Reference<ResourceHandler> ResourceHandler::CreateVulkan(uint32 bufferCount)
	{
		return ME::Core::Memory::MakeReference<VulkanResourceHandler>(bufferCount);
	}

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
			ME_ASSERT(false, "Vulkan: failed to create descriptor set layout! Error: {0}", result);
			return static_cast<uint32>(m_Layouts.Size());
		}

		m_Layouts.EmplaceBack(pair);

		return static_cast<uint32>(m_Layouts.Size() - 1);
	}

	uint32 VulkanResourceHandler::CreateResourceSet(uint32 layout)
	{
		VkDescriptorSet descSet = nullptr;
		CreateDescriptorSet(layout, &descSet);
		m_Sets.EmplaceBack(descSet);
		return static_cast<uint32>(m_Sets.Size() - 1);
	}

	uint32 VulkanResourceHandler::CreateResourceSet(const ME::Render::ResourceLayout& layout)
	{
		return CreateResourceSet(GetResourceLayoutIndex(layout));
	}

	uint32 VulkanResourceHandler::GetResourceSet(uint32 layout)
	{
		return 0;
	}

	uint32 VulkanResourceHandler::GetResourceSet(const ME::Render::ResourceLayout& layout)
	{
		return 0;
	}

	uint32 VulkanResourceHandler::GetTextureSet(uint32 layout)
	{
		if (layout == ~0u)
		{
			ME_ASSERT(layout != ~0u, "Can't find layout!");
			return ~0u;
		}

		uint32 set = FindUnusedTextureSet();

		if (set != ~0u)
			return set;

		VulkanTextureSet texSet = {};
		texSet.References = 1;
		texSet.Set = nullptr;

		CreateDescriptorSet(layout, &texSet.Set);
		m_TextureSets.EmplaceBack(texSet);
		return static_cast<uint32>(m_TextureSets.Size() - 1);
	}

	uint32 VulkanResourceHandler::GetTextureSet(const ME::Render::ResourceLayout& layout)
	{
		return GetTextureSet(GetResourceLayoutIndex(layout));
	}

	bool VulkanResourceHandler::IncrementTextureSetReference(uint32 set)
	{
		if (set >= m_TextureSets.Size()) return false;
		m_TextureSets[set].References++;
		return true;
	}

	void VulkanResourceHandler::DecrementTextureSetReference(uint32 set)
	{
		if (set >= m_TextureSets.Size()) return;
		if (m_TextureSets[set].References <= 0)
			return;
		m_TextureSets[set].References--;
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
		writeDesc.dstSet = m_Sets[buffer->GetResourceSet()].Set;
		writeDesc.dstBinding = buffer->GetSpecification().Binding;
		writeDesc.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		writeDesc.dstArrayElement = 0;
		writeDesc.descriptorCount = 1;
		writeDesc.pBufferInfo = &bufferInfo;
		writeDesc.pImageInfo = nullptr;
		writeDesc.pTexelBufferView = nullptr;
		writeDesc.pNext = nullptr;

		vkUpdateDescriptorSets(RenderCommand::Get()->As<VulkanRenderAPI>()->GetDevice(),
			1,
			&writeDesc,
			0,
			nullptr);
	}

	void VulkanResourceHandler::WriteResource(ME::Core::Memory::Reference<ME::Render::StorageBuffer> buffer)
	{
		VkDescriptorBufferInfo bufferInfo = {};
		bufferInfo.buffer = buffer->As<VulkanUniform>()->GetBuffer();
		bufferInfo.range = static_cast<VkDeviceSize>(buffer->GetSpecification().Size);
		bufferInfo.offset = 0;

		VkWriteDescriptorSet writeDesc = {};
		writeDesc.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDesc.dstSet = m_Sets[buffer->GetResourceSet()].Set;
		writeDesc.dstBinding = buffer->GetSpecification().Binding;
		writeDesc.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		writeDesc.dstArrayElement = 0;
		writeDesc.descriptorCount = 1;
		writeDesc.pBufferInfo = &bufferInfo;
		writeDesc.pImageInfo = nullptr;
		writeDesc.pTexelBufferView = nullptr;
		writeDesc.pNext = nullptr;

		vkUpdateDescriptorSets(RenderCommand::Get()->As<VulkanRenderAPI>()->GetDevice(), 
			1,
			&writeDesc,
			0,
			nullptr);
	}

    void VulkanResourceHandler::WriteResource(ME::Core::Memory::Reference<ME::Render::IndirectBuffer> buffer)
    {
		VkDescriptorBufferInfo bufferInfo = {};
		bufferInfo.buffer = buffer->As<VulkanUniform>()->GetBuffer();
		bufferInfo.range = static_cast<VkDeviceSize>(buffer->GetSpecification().Size);
		bufferInfo.offset = 0;

		VkWriteDescriptorSet writeDesc = {};
		writeDesc.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDesc.dstSet = m_Sets[buffer->GetResourceSet()].Set;
		writeDesc.dstBinding = buffer->GetSpecification().Binding;
		writeDesc.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		writeDesc.dstArrayElement = 0;
		writeDesc.descriptorCount = 1;
		writeDesc.pBufferInfo = &bufferInfo;
		writeDesc.pImageInfo = nullptr;
		writeDesc.pTexelBufferView = nullptr;
		writeDesc.pNext = nullptr;

		vkUpdateDescriptorSets(RenderCommand::Get()->As<VulkanRenderAPI>()->GetDevice(),
			1,
			&writeDesc,
			0,
			nullptr);
    }

    void VulkanResourceHandler::WriteResource(ME::Core::Memory::Reference<ME::Render::VertexBuffer> buffer)
    {
		VkDescriptorBufferInfo bufferInfo = {};
		bufferInfo.buffer = buffer->As<VulkanUniform>()->GetBuffer();
		bufferInfo.range = static_cast<VkDeviceSize>(buffer->GetSpecification().Size);
		bufferInfo.offset = 0;

		VkWriteDescriptorSet writeDesc = {};
		writeDesc.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDesc.dstSet = m_Sets[buffer->GetResourceSet()].Set;
		writeDesc.dstBinding = buffer->GetSpecification().Binding;
		writeDesc.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		writeDesc.dstArrayElement = 0;
		writeDesc.descriptorCount = 1;
		writeDesc.pBufferInfo = &bufferInfo;
		writeDesc.pImageInfo = nullptr;
		writeDesc.pTexelBufferView = nullptr;
		writeDesc.pNext = nullptr;

		vkUpdateDescriptorSets(RenderCommand::Get()->As<VulkanRenderAPI>()->GetDevice(),
			1,
			&writeDesc,
			0,
			nullptr);
    }

    void VulkanResourceHandler::WriteResource(ME::Core::Memory::Reference<ME::Render::IndexBuffer> buffer)
    {
		VkDescriptorBufferInfo bufferInfo = {};
		bufferInfo.buffer = buffer->As<VulkanUniform>()->GetBuffer();
		bufferInfo.range = static_cast<VkDeviceSize>(buffer->GetCount() / sizeof(uint32));
		bufferInfo.offset = 0;

		VkWriteDescriptorSet writeDesc = {};
		writeDesc.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDesc.dstSet = m_Sets[buffer->GetResourceSet()].Set;
		writeDesc.dstBinding = buffer->GetSpecification().Binding;
		writeDesc.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		writeDesc.dstArrayElement = 0;
		writeDesc.descriptorCount = 1;
		writeDesc.pBufferInfo = &bufferInfo;
		writeDesc.pImageInfo = nullptr;
		writeDesc.pTexelBufferView = nullptr;
		writeDesc.pNext = nullptr;

		vkUpdateDescriptorSets(RenderCommand::Get()->As<VulkanRenderAPI>()->GetDevice(),
			1,
			&writeDesc,
			0,
			nullptr);
    }

    void VulkanResourceHandler::QueueTexture(ME::Core::Memory::Reference<ME::Render::Texture1D> texture)
	{
		if (!texture->Loaded())
		{
			ME_ERROR("Trying to queue a texture that isn't loaded! Texture: {0}", texture->GetDebugName());
			return;
		}

		if (m_WritingTextures == true)
		{
			if (texture->GetSet() != m_CurrentTextureSet) return;
		}
		else
		{
			m_CurrentTextureSet = texture->GetSet();
			m_WritingTextures = true;
		}

		VkDescriptorImageInfo image = {};
		image.imageView = texture->As<VulkanTexture2D>()->GetImageView();
		image.imageLayout = VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL;

		VkDescriptorImageInfo sampler = {};
		sampler.sampler = texture->As<VulkanTexture2D>()->GetSampler();

		m_QueuedImages.EmplaceBack(image);
		m_QueuedSamplers.EmplaceBack(sampler);
	}

	void VulkanResourceHandler::QueueTexture(ME::Core::Memory::Reference<ME::Render::Texture2D> texture)
	{
		if (!texture->Loaded())
		{
			ME_ERROR("Trying to queue a texture that isn't loaded! Texture: {0}", texture->GetDebugName());
            return;
		}

		if (m_WritingTextures == true)
		{
			if (texture->GetSet() != m_CurrentTextureSet) return;
		}
		else
		{
			m_CurrentTextureSet = texture->GetSet();
			m_WritingTextures = true;
		}

		VkDescriptorImageInfo image = {};
		image.imageView = texture->As<VulkanTexture2D>()->GetImageView();
		image.imageLayout = VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL;

		VkDescriptorImageInfo sampler = {};
		sampler.sampler = texture->As<VulkanTexture2D>()->GetSampler();

		m_QueuedImages.EmplaceBack(image);
		m_QueuedSamplers.EmplaceBack(sampler);
	}

	void VulkanResourceHandler::QueueTexture(ME::Core::Memory::Reference<ME::Render::Texture3D> texture)
	{
		if (!texture->Loaded())
		{
			ME_ERROR("Trying to queue a texture that isn't loaded! Texture: {0}", texture->GetDebugName());
			return;
		}

		if (m_WritingTextures == true)
		{
			if (texture->GetSet() != m_CurrentTextureSet) return;
		}
		else
		{
			m_CurrentTextureSet = texture->GetSet();
			m_WritingTextures = true;
		}

		VkDescriptorImageInfo image = {};
		image.imageView = texture->As<VulkanTexture2D>()->GetImageView();
		image.imageLayout = VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL;

		VkDescriptorImageInfo sampler = {};
		sampler.sampler = texture->As<VulkanTexture2D>()->GetSampler();

		m_QueuedImages.EmplaceBack(image);
		m_QueuedSamplers.EmplaceBack(sampler);
	}

    void VulkanResourceHandler::ClearTextureQueue()
    {
        m_QueuedImages.Clear();
        m_QueuedSamplers.Clear();
        m_WritingTextures = false;
    }

    void VulkanResourceHandler::WriteTexture()
	{
		if (m_WritingTextures == false)
		{
            ME_WARN("Not writing any textures!");
			return;
		}

		VkWriteDescriptorSet descs[2] = {};

		descs[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descs[0].dstSet = m_TextureSets[m_CurrentTextureSet].Set;
		descs[0].dstBinding = 0;
		descs[0].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
		descs[0].dstArrayElement = 0;
		descs[0].descriptorCount = static_cast<uint32>(m_QueuedImages.Size());
		descs[0].pBufferInfo = nullptr;
		descs[0].pImageInfo = m_QueuedImages.Data();
		descs[0].pTexelBufferView = nullptr;
		descs[0].pNext = nullptr;

		descs[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descs[1].dstSet = m_TextureSets[m_CurrentTextureSet].Set;
		descs[1].dstBinding = 1;
		descs[1].descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
		descs[1].dstArrayElement = 0;
		descs[1].descriptorCount = static_cast<uint32>(m_QueuedSamplers.Size());
		descs[1].pBufferInfo = nullptr;
		descs[1].pImageInfo = m_QueuedSamplers.Data();
		descs[1].pTexelBufferView = nullptr;
		descs[1].pNext = nullptr;

		vkUpdateDescriptorSets(RenderCommand::Get()->As<VulkanRenderAPI>()->GetDevice(), 2, descs, 0, nullptr);
	}

	void VulkanResourceHandler::BindResourceSet(ME::Core::Memory::Reference<Render::CommandBuffer> commandBuffer, ME::Core::Memory::Reference<Render::Pipeline> pipeline, uint32 set, uint32 setIndex)
	{
		vkCmdBindDescriptorSets(commandBuffer->As<VulkanCommandBuffer>()->GetCommandBuffer(), pipeline->As<VulkanPipeline>()->GetPipelineBindPoint(), pipeline->As<VulkanPipeline>()->GetPipelineLayout(), set, 1, &m_Sets[setIndex].Set, 0, nullptr);
	}

	void VulkanResourceHandler::BindTexture(ME::Core::Memory::Reference<Render::CommandBuffer> commandBuffer, ME::Core::Memory::Reference<Render::Pipeline> pipeline, uint32 set, uint32 setIndex)
	{
		vkCmdBindDescriptorSets(commandBuffer->As<VulkanCommandBuffer>()->GetCommandBuffer(), pipeline->As<VulkanPipeline>()->GetPipelineBindPoint(), pipeline->As<VulkanPipeline>()->GetPipelineLayout(), set, 1, &m_TextureSets[setIndex].Set, 0, nullptr);
	}

	ME::Core::Containers::Array<VkDescriptorSetLayout> VulkanResourceHandler::GetDescriptorSetLayouts(ME::Core::Containers::Array<uint32> layouts) const
	{
		ME::Core::Containers::Array<VkDescriptorSetLayout> result;

		for (const auto& layoutIndex : layouts)
		{
			if (layoutIndex > m_Layouts.Size())
			{
				ME_ERROR("Layout with index {0}, doesn't exist!", layoutIndex);
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
			ME_ASSERT(false, "Vulkan: failed to allocate DescriptorSet! Error: {0}", ME_VK_RETURN_V(result));
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

		for (uint32 u = 0; u < layout.Layout.Size(); u++)
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
		bindingFlagsCreateInfo.bindingCount = static_cast<uint32>(bindFlags.Size());
		bindingFlagsCreateInfo.pBindingFlags = bindFlags.Data();
		bindingFlagsCreateInfo.pNext = nullptr;

		VkDescriptorSetLayoutCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		createInfo.bindingCount = static_cast<uint32>(bindings.Size());
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
		createInfo.poolSizeCount = static_cast<uint32>(poolSizes.Size());
		createInfo.pPoolSizes = poolSizes.Data();
		createInfo.maxSets = ME_VK_INITIAL_MAX_DESCRIPTOR_SETS;
		createInfo.flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT;

		VkResult result = vkCreateDescriptorPool(RenderCommand::Get()->As<VulkanRenderAPI>()->GetDevice(), &createInfo, nullptr, &m_DescriptorPool);
		if (ME_VK_FAILED(result))
		{
			ME_ASSERT(false, "Vulkan: Failed to create descriptor set pool! Error: {0}", ME_VK_RETURN_V(result));
			Shutdown();
		}
	}

	int32 VulkanResourceHandler::GetLayoutIndex(const ME::Render::ResourceLayout& layout) const
	{
		for (uint32 i = 0; i < m_Layouts.Size(); ++i)
			if (m_Layouts[i].Layout == layout) return static_cast<uint32>(i);
		return -1;
	}

	uint32 VulkanResourceHandler::FindUnusedTextureSet() const
	{
		for (uint32 i = 0; i < static_cast<int32>(m_TextureSets.Size()); i++)
		{
			if (m_TextureSets[i].References <= 0) return i;
		}
		return ~0u;
	}

}
