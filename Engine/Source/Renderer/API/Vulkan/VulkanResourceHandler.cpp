#include "VulkanResourceHandler.hpp"
#include "VulkanFunctions.hpp"
#include "VulkanPipeline.hpp"
#include "VulkanCommandBuffer.hpp"
#include "VulkanIndexBuffer.hpp"
#include "VulkanIndirectBuffer.hpp"
#include "VulkanRenderAPI.hpp"
#include "VulkanStorageBuffer.hpp"
#include "VulkanTexture1D.hpp"
#include "VulkanTexture2D.hpp"
#include "VulkanTexture3D.hpp"
#include "VulkanUniform.hpp"
#include "VulkanVertexBuffer.hpp"
#include "Renderer/RenderCommand.hpp"
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
		if (index < 0) return CreateLayout(layout);
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
		bufferInfo.buffer = buffer->As<VulkanStorageBuffer>()->GetBuffer();
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
		bufferInfo.buffer = buffer->As<VulkanIndirectBuffer>()->GetBuffer();
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
		bufferInfo.buffer = buffer->As<VulkanVertexBuffer>()->GetBuffer();
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
		bufferInfo.buffer = buffer->As<VulkanIndexBuffer>()->GetBuffer();
		bufferInfo.range = static_cast<VkDeviceSize>(buffer->GetCount() * sizeof(uint32));
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

    void VulkanResourceHandler::WriteResource(ME::Core::Memory::Reference<ME::Render::Texture1D> texture)
    {
		VkDescriptorImageInfo textureInfo = {};
		textureInfo.imageLayout = ConvertImageLayoutVulkan(texture->GetSpecification().Layout);
		textureInfo.imageView = texture->As<VulkanTexture1D>()->GetImageView();
		textureInfo.sampler = texture->As<VulkanTexture1D>()->GetSampler();

		VkWriteDescriptorSet writeDesc = {};
		writeDesc.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDesc.dstSet = m_Sets[texture->GetSpecification().SetIndex].Set;
		writeDesc.dstBinding = texture->GetSpecification().Binding;
		writeDesc.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		writeDesc.dstArrayElement = 0;
		writeDesc.descriptorCount = 1;
		writeDesc.pBufferInfo = nullptr;
		writeDesc.pImageInfo = &textureInfo;
		writeDesc.pTexelBufferView = nullptr;
		writeDesc.pNext = nullptr;

		vkUpdateDescriptorSets(RenderCommand::Get()->As<VulkanRenderAPI>()->GetDevice(),
			1,
			&writeDesc,
			0,
			nullptr);
    }

    void VulkanResourceHandler::WriteResource(ME::Core::Memory::Reference<ME::Render::Texture2D> texture)
    {
		VkDescriptorImageInfo textureInfo = {};
		textureInfo.imageLayout = ConvertImageLayoutVulkan(texture->GetSpecification().Layout);
		textureInfo.imageView = texture->As<VulkanTexture2D>()->GetImageView();
		textureInfo.sampler = texture->As<VulkanTexture2D>()->GetSampler();

		VkWriteDescriptorSet writeDesc = {};
		writeDesc.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDesc.dstSet = m_Sets[texture->GetSpecification().SetIndex].Set;
		writeDesc.dstBinding = texture->GetSpecification().Binding;
		writeDesc.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		writeDesc.dstArrayElement = 0;
		writeDesc.descriptorCount = 1;
		writeDesc.pBufferInfo = nullptr;
		writeDesc.pImageInfo = &textureInfo;
		writeDesc.pTexelBufferView = nullptr;
		writeDesc.pNext = nullptr;

		vkUpdateDescriptorSets(RenderCommand::Get()->As<VulkanRenderAPI>()->GetDevice(),
			1,
			&writeDesc,
			0,
			nullptr);
    }

    void VulkanResourceHandler::WriteResource(ME::Core::Memory::Reference<ME::Render::Texture3D> texture)
    {
    }

    void VulkanResourceHandler::BufferBarrier(ME::Core::Memory::Reference<Render::CommandBuffer> commandBuffer,
                                              const ME::Core::Memory::Reference<Render::VertexBuffer>& buffer, BarrierInfo src, BarrierInfo dst)
	{
		VkBufferMemoryBarrier bufferBarrier = {};
		bufferBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
		bufferBarrier.srcAccessMask = ConvertAccessFlagsVulkan(src.Access);
		bufferBarrier.dstAccessMask = ConvertAccessFlagsVulkan(dst.Access);
		bufferBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		bufferBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		bufferBarrier.buffer = buffer->As<VulkanVertexBuffer>()->GetBuffer();
		bufferBarrier.offset = 0;
		bufferBarrier.size = VK_WHOLE_SIZE;

		vkCmdPipelineBarrier(commandBuffer->As<VulkanCommandBuffer>()->GetCommandBuffer(),
			ConvertPipelineStageFlagsVulkan(src.PipelineStage),
			ConvertPipelineStageFlagsVulkan(dst.PipelineStage),
			0,
			0, nullptr,
			1, &bufferBarrier,
			0, nullptr);
	}

	void VulkanResourceHandler::BufferBarrier(ME::Core::Memory::Reference<Render::CommandBuffer> commandBuffer,
		const ME::Core::Memory::Reference<Render::IndexBuffer>& buffer, BarrierInfo src, BarrierInfo dst)
	{
		VkBufferMemoryBarrier bufferBarrier = {};
		bufferBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
		bufferBarrier.srcAccessMask = ConvertAccessFlagsVulkan(src.Access);
		bufferBarrier.dstAccessMask = ConvertAccessFlagsVulkan(dst.Access);
		bufferBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		bufferBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		bufferBarrier.buffer = buffer->As<VulkanIndexBuffer>()->GetBuffer();
		bufferBarrier.offset = 0;
		bufferBarrier.size = VK_WHOLE_SIZE;

		vkCmdPipelineBarrier(commandBuffer->As<VulkanCommandBuffer>()->GetCommandBuffer(),
			ConvertPipelineStageFlagsVulkan(src.PipelineStage),
			ConvertPipelineStageFlagsVulkan(dst.PipelineStage),
			0,
			0, nullptr,
			1, &bufferBarrier,
			0, nullptr);
	}

	void VulkanResourceHandler::BufferBarrier(ME::Core::Memory::Reference<Render::CommandBuffer> commandBuffer,
		const ME::Core::Memory::Reference<Render::IndirectBuffer>& buffer, BarrierInfo src,
		BarrierInfo dst)
	{
		VkBufferMemoryBarrier bufferBarrier = {};
		bufferBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
		bufferBarrier.srcAccessMask = ConvertAccessFlagsVulkan(src.Access);
		bufferBarrier.dstAccessMask = ConvertAccessFlagsVulkan(dst.Access);
		bufferBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		bufferBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		bufferBarrier.buffer = buffer->As<VulkanIndirectBuffer>()->GetBuffer();
		bufferBarrier.offset = 0;
		bufferBarrier.size = VK_WHOLE_SIZE;

		vkCmdPipelineBarrier(commandBuffer->As<VulkanCommandBuffer>()->GetCommandBuffer(),
			ConvertPipelineStageFlagsVulkan(src.PipelineStage),
			ConvertPipelineStageFlagsVulkan(dst.PipelineStage),
			0,
			0, nullptr,
			1, &bufferBarrier,
			0, nullptr);
	}

	void VulkanResourceHandler::BufferBarrier(ME::Core::Memory::Reference<Render::CommandBuffer> commandBuffer,
		const ME::Core::Memory::Reference<Render::StorageBuffer>& buffer, BarrierInfo src,
		BarrierInfo dst)
	{
		VkBufferMemoryBarrier bufferBarrier = {};
		bufferBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
		bufferBarrier.srcAccessMask = ConvertAccessFlagsVulkan(src.Access);
		bufferBarrier.dstAccessMask = ConvertAccessFlagsVulkan(dst.Access);
		bufferBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		bufferBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		bufferBarrier.buffer = buffer->As<VulkanStorageBuffer>()->GetBuffer();
		bufferBarrier.offset = 0;
		bufferBarrier.size = VK_WHOLE_SIZE;

		vkCmdPipelineBarrier(commandBuffer->As<VulkanCommandBuffer>()->GetCommandBuffer(),
			ConvertPipelineStageFlagsVulkan(src.PipelineStage),
			ConvertPipelineStageFlagsVulkan(dst.PipelineStage),
			0,
			0, nullptr,
			1, &bufferBarrier,
			0, nullptr);
	}

	void VulkanResourceHandler::BufferBarrier(ME::Core::Memory::Reference<Render::CommandBuffer> commandBuffer,
		const ME::Core::Memory::Reference<Render::Uniform>& buffer, BarrierInfo src, BarrierInfo dst)
	{
		VkBufferMemoryBarrier bufferBarrier = {};
		bufferBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
		bufferBarrier.srcAccessMask = ConvertAccessFlagsVulkan(src.Access);
		bufferBarrier.dstAccessMask = ConvertAccessFlagsVulkan(dst.Access);
		bufferBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		bufferBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		bufferBarrier.buffer = buffer->As<VulkanUniform>()->GetBuffer();
		bufferBarrier.offset = 0;
		bufferBarrier.size = VK_WHOLE_SIZE;

		vkCmdPipelineBarrier(commandBuffer->As<VulkanCommandBuffer>()->GetCommandBuffer(),
			ConvertPipelineStageFlagsVulkan(src.PipelineStage),
			ConvertPipelineStageFlagsVulkan(dst.PipelineStage),
			0,
			0, nullptr,
			1, &bufferBarrier,
			0, nullptr);
	}

	void VulkanResourceHandler::TextureBarrier(ME::Core::Memory::Reference<Render::CommandBuffer> commandBuffer,
		const ME::Core::Memory::Reference<Render::Texture1D>& texture, BarrierInfo src, BarrierInfo dst)
	{
		Texture1DSpecification texSpecs = texture->As<VulkanTexture1D>()->GetSpecification();

		VkImageSubresourceRange range = {};
		range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		if (texSpecs.IsDepth)  range.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		if (texSpecs.IsStencil) range.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
		range.baseMipLevel = 0;
		range.baseArrayLayer = 0;
		range.levelCount = texSpecs.MipLevels;
		range.layerCount = texSpecs.CubeMapCount > 0 ? 6 * texSpecs.CubeMapCount : 1;

		VkImageMemoryBarrier textureBarrier = {};
		textureBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		textureBarrier.srcAccessMask = ConvertAccessFlagsVulkan(src.Access);
		textureBarrier.dstAccessMask = ConvertAccessFlagsVulkan(dst.Access);
		textureBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		textureBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		textureBarrier.image = texture->As<VulkanTexture1D>()->GetImage();
		textureBarrier.subresourceRange = range;
		textureBarrier.oldLayout = ConvertImageLayoutVulkan(src.Layout);
		textureBarrier.newLayout = ConvertImageLayoutVulkan(dst.Layout);

		vkCmdPipelineBarrier(commandBuffer->As<VulkanCommandBuffer>()->GetCommandBuffer(),
			ConvertPipelineStageFlagsVulkan(src.PipelineStage),
			ConvertPipelineStageFlagsVulkan(dst.PipelineStage),
			0,
			0, nullptr,
			0, nullptr,
			1, &textureBarrier);
	}

	void VulkanResourceHandler::TextureBarrier(ME::Core::Memory::Reference<Render::CommandBuffer> commandBuffer,
		const ME::Core::Memory::Reference<Render::Texture2D>& texture, BarrierInfo src, BarrierInfo dst)
	{
		Texture2DSpecification texSpecs = texture->As<VulkanTexture2D>()->GetSpecification();

		VkImageSubresourceRange range = {};
		range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		if (texSpecs.IsDepth)  range.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		if (texSpecs.IsStencil) range.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
		range.baseMipLevel = 0;
		range.baseArrayLayer = 0;
		range.levelCount = texSpecs.MipLevels;
		range.layerCount = texSpecs.CubeMapCount > 0 ? 6 * texSpecs.CubeMapCount : 1;

		VkImageMemoryBarrier textureBarrier = {};
		textureBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		textureBarrier.srcAccessMask = ConvertAccessFlagsVulkan(src.Access);
		textureBarrier.dstAccessMask = ConvertAccessFlagsVulkan(dst.Access);
		textureBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		textureBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		textureBarrier.image = texture->As<VulkanTexture2D>()->GetImage();
		textureBarrier.subresourceRange = range;
		textureBarrier.oldLayout = ConvertImageLayoutVulkan(src.Layout);
		textureBarrier.newLayout = ConvertImageLayoutVulkan(dst.Layout);

		vkCmdPipelineBarrier(commandBuffer->As<VulkanCommandBuffer>()->GetCommandBuffer(),
		    ConvertPipelineStageFlagsVulkan(src.PipelineStage),
		    ConvertPipelineStageFlagsVulkan(dst.PipelineStage),
		    0,
		    0, nullptr,
		    0, nullptr,
		    1, &textureBarrier);
	}

	void VulkanResourceHandler::TextureBarrier(ME::Core::Memory::Reference<Render::CommandBuffer> commandBuffer,
		const ME::Core::Memory::Reference<Render::Texture3D>& texture, BarrierInfo src, BarrierInfo dst)
	{
		Texture3DSpecification texSpecs = texture->As<VulkanTexture3D>()->GetSpecification();

		VkImageSubresourceRange range = {};
		range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		if (texSpecs.IsDepth)  range.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		if (texSpecs.IsStencil) range.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
		range.baseMipLevel = 0;
		range.baseArrayLayer = 0;
		range.levelCount = texSpecs.MipLevels;
		range.layerCount = texSpecs.CubeMapCount > 0 ? 6 * texSpecs.CubeMapCount : 1;

		VkImageMemoryBarrier textureBarrier = {};
		textureBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		textureBarrier.srcAccessMask = ConvertAccessFlagsVulkan(src.Access);
		textureBarrier.dstAccessMask = ConvertAccessFlagsVulkan(dst.Access);
		textureBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		textureBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		textureBarrier.image = texture->As<VulkanTexture3D>()->GetImage();
		textureBarrier.subresourceRange = range;
		textureBarrier.oldLayout = ConvertImageLayoutVulkan(src.Layout);
		textureBarrier.newLayout = ConvertImageLayoutVulkan(dst.Layout);

		vkCmdPipelineBarrier(commandBuffer->As<VulkanCommandBuffer>()->GetCommandBuffer(),
			ConvertPipelineStageFlagsVulkan(src.PipelineStage),
			ConvertPipelineStageFlagsVulkan(dst.PipelineStage),
			0,
			0, nullptr,
			0, nullptr,
			1, &textureBarrier);
	}

    void VulkanResourceHandler::WriteResource(VulkanTexture1D* texture)
    {
		VkDescriptorImageInfo textureInfo = {};
		textureInfo.imageLayout = ConvertImageLayoutVulkan(texture->GetSpecification().Layout);
		textureInfo.imageView = texture->GetImageView();
		textureInfo.sampler = texture->GetSampler();

		VkWriteDescriptorSet writeDesc = {};
		writeDesc.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDesc.dstSet = m_Sets[texture->GetSpecification().SetIndex].Set;
		writeDesc.dstBinding = texture->GetSpecification().Binding;
		writeDesc.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		writeDesc.dstArrayElement = 0;
		writeDesc.descriptorCount = 1;
		writeDesc.pBufferInfo = nullptr;
		writeDesc.pImageInfo = &textureInfo;
		writeDesc.pTexelBufferView = nullptr;
		writeDesc.pNext = nullptr;

		vkUpdateDescriptorSets(RenderCommand::Get()->As<VulkanRenderAPI>()->GetDevice(),
			1,
			&writeDesc,
			0,
			nullptr);
    }

    void VulkanResourceHandler::WriteResource(VulkanTexture2D* texture)
    {
		VkDescriptorImageInfo textureInfo = {};
		textureInfo.imageLayout = ConvertImageLayoutVulkan(texture->GetSpecification().Layout);
		textureInfo.imageView = texture->GetImageView();
		textureInfo.sampler = texture->GetSampler();

		VkWriteDescriptorSet writeDesc = {};
		writeDesc.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDesc.dstSet = m_Sets[texture->GetSpecification().SetIndex].Set;
		writeDesc.dstBinding = texture->GetSpecification().Binding;
		writeDesc.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		writeDesc.dstArrayElement = 0;
		writeDesc.descriptorCount = 1;
		writeDesc.pBufferInfo = nullptr;
		writeDesc.pImageInfo = &textureInfo;
		writeDesc.pTexelBufferView = nullptr;
		writeDesc.pNext = nullptr;

		vkUpdateDescriptorSets(RenderCommand::Get()->As<VulkanRenderAPI>()->GetDevice(),
			1,
			&writeDesc,
			0,
			nullptr);
    }

    void VulkanResourceHandler::WriteResource(VulkanTexture3D* texture)
    {
		VkDescriptorImageInfo textureInfo = {};
		textureInfo.imageLayout = ConvertImageLayoutVulkan(texture->GetSpecification().Layout);
		textureInfo.imageView = texture->GetImageView();
		textureInfo.sampler = texture->GetSampler();

		VkWriteDescriptorSet writeDesc = {};
		writeDesc.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDesc.dstSet = m_Sets[texture->GetSpecification().SetIndex].Set;
		writeDesc.dstBinding = texture->GetSpecification().Binding;
		writeDesc.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		writeDesc.dstArrayElement = 0;
		writeDesc.descriptorCount = 1;
		writeDesc.pBufferInfo = nullptr;
		writeDesc.pImageInfo = &textureInfo;
		writeDesc.pTexelBufferView = nullptr;
		writeDesc.pNext = nullptr;

		vkUpdateDescriptorSets(RenderCommand::Get()->As<VulkanRenderAPI>()->GetDevice(),
			1,
			&writeDesc,
			0,
			nullptr);
    }

    void VulkanResourceHandler::WriteResource(VulkanUniform* buffer)
    {
		VkDescriptorBufferInfo bufferInfo = {};
		bufferInfo.buffer = buffer->GetBuffer();
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

    void VulkanResourceHandler::WriteResource(VulkanIndexBuffer* buffer)
    {
		VkDescriptorBufferInfo bufferInfo = {};
		bufferInfo.buffer = buffer->GetBuffer();
		bufferInfo.range = static_cast<VkDeviceSize>(buffer->GetCount() * sizeof(uint32));
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

    void VulkanResourceHandler::WriteResource(VulkanVertexBuffer* buffer)
    {
		VkDescriptorBufferInfo bufferInfo = {};
		bufferInfo.buffer = buffer->GetBuffer();
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

    void VulkanResourceHandler::WriteResource(VulkanIndirectBuffer* buffer)
    {
		VkDescriptorBufferInfo bufferInfo = {};
		bufferInfo.buffer = buffer->As<VulkanIndirectBuffer>()->GetBuffer();
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

    void VulkanResourceHandler::WriteResource(VulkanStorageBuffer* buffer)
    {
		VkDescriptorBufferInfo bufferInfo = {};
		bufferInfo.buffer = buffer->As<VulkanStorageBuffer>()->GetBuffer();
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

    void VulkanResourceHandler::BufferBarrier(ME::Core::Memory::Reference<Render::CommandBuffer> commandBuffer,
        VkBuffer buffer, BarrierInfo src, BarrierInfo dst)
    {
		VkBufferMemoryBarrier bufferBarrier = {};
		bufferBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
		bufferBarrier.srcAccessMask = ConvertAccessFlagsVulkan(src.Access);
		bufferBarrier.dstAccessMask = ConvertAccessFlagsVulkan(dst.Access);
		bufferBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		bufferBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		bufferBarrier.buffer = buffer;
		bufferBarrier.offset = 0;
		bufferBarrier.size = VK_WHOLE_SIZE;

		vkCmdPipelineBarrier(commandBuffer->As<VulkanCommandBuffer>()->GetCommandBuffer(),
			ConvertPipelineStageFlagsVulkan(src.PipelineStage),
			ConvertPipelineStageFlagsVulkan(dst.PipelineStage),
			0,
			0, nullptr,
			1, &bufferBarrier,
			0, nullptr);
    }

    void VulkanResourceHandler::TextureBarrier(ME::Core::Memory::Reference<Render::CommandBuffer> commandBuffer,
        VkImage image, const TextureSpecification& specs, BarrierInfo src, BarrierInfo dst)
    {
		VkImageSubresourceRange range = {};
		range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		if (specs.IsDepth)  range.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		if (specs.IsStencil) range.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
		range.baseMipLevel = 0;
		range.baseArrayLayer = 0;
		range.levelCount = specs.MipLevels;
		range.layerCount = specs.CubeMapCount > 0 ? 6 * specs.CubeMapCount : 1;

		VkImageMemoryBarrier textureBarrier = {};
		textureBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		textureBarrier.srcAccessMask = ConvertAccessFlagsVulkan(src.Access);
		textureBarrier.dstAccessMask = ConvertAccessFlagsVulkan(dst.Access);
		textureBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		textureBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		textureBarrier.image = image;
		textureBarrier.subresourceRange = range;
		textureBarrier.oldLayout = ConvertImageLayoutVulkan(src.Layout);
		textureBarrier.newLayout = ConvertImageLayoutVulkan(dst.Layout);

		vkCmdPipelineBarrier(commandBuffer->As<VulkanCommandBuffer>()->GetCommandBuffer(),
			ConvertPipelineStageFlagsVulkan(src.PipelineStage),
			ConvertPipelineStageFlagsVulkan(dst.PipelineStage),
			0,
			0, nullptr,
			0, nullptr,
			1, &textureBarrier);
    }

    void VulkanResourceHandler::BindResourceSet(ME::Core::Memory::Reference<Render::CommandBuffer> commandBuffer, ME::Core::Memory::Reference<Render::Pipeline> pipeline, uint32 set, uint32 setIndex)
	{
		vkCmdBindDescriptorSets(commandBuffer->As<VulkanCommandBuffer>()->GetCommandBuffer(), pipeline->As<VulkanPipeline>()->GetPipelineBindPoint(), pipeline->As<VulkanPipeline>()->GetPipelineLayout(), set, 1, &m_Sets[setIndex].Set, 0, nullptr);
	}

	ME::Core::Array<VkDescriptorSetLayout> VulkanResourceHandler::GetDescriptorSetLayouts(ME::Core::Array<uint32> layouts) const
	{
		ME::Core::Array<VkDescriptorSetLayout> result;

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
		ME::Core::Array<VkDescriptorSetLayoutBinding> bindings;
		ME::Core::Array<VkDescriptorBindingFlags> bindFlags;

		uint32 binding = 0;
		for (ResourceLayout::Iterator it = layout.Layout.Begin(); it != layout.Layout.End(); ++it)
		{
			VkDescriptorSetLayoutBinding vkBinding = {};
			vkBinding.binding = binding;
		    vkBinding.stageFlags = ConvertShaderStageVulkan(it->Stage);
			vkBinding.descriptorType = ConvertResourceTypeVulkan(it->Type);
		    if (it->Combined && (it->Type == ResourceType::Texture1D || it->Type == ResourceType::Texture2D || it->Type == ResourceType::Texture3D))
			    if ((it + 1)->Type == ResourceType::Sampler)
			    {
					vkBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
					++it;
			    }

			vkBinding.descriptorCount = 1;

		    bindings.EmplaceBack(vkBinding);
			bindFlags.EmplaceBack(bindingFlags);
		    binding++;
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
		ME::Core::Array<VkDescriptorPoolSize> poolSizes;
		
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
