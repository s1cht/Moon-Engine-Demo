#include "VulkanTexture2D.hpp"

#include "VulkanRenderAPI.hpp"
#include "VulkanCommandBuffer.hpp"
#include "VulkanFunctions.hpp"
#include "VulkanResourceHandler.hpp"
#include "Renderer/RenderCommand.hpp"
#include "Renderer/RenderResourcesTracker.hpp"

namespace ME::Render
{
	ME::Core::Memory::Reference<Texture2D> Texture2D::CreateVulkan(const Texture2DSpecification& specification)
	{
		auto object = ME::Core::Memory::MakeReference<VulkanTexture2D>(specification);
		RenderResourcesTracker::Get().AddItem(object);
		return object;
	}

	VulkanTexture2D::VulkanTexture2D(const Texture2DSpecification& specification)
		: Texture2D(specification)
	{
		Init();
	}

	VulkanTexture2D::VulkanTexture2D(VkImage image, const Texture2DSpecification& specification)
		: Texture2D(specification)
	{
		Init(image);
	}

	VulkanTexture2D::~VulkanTexture2D()
	{
		Shutdown();
	}

	void VulkanTexture2D::SetData(void* data, SIZE_T size)
	{
		UpdateImage(data, size);
	}

	void VulkanTexture2D::Shutdown()
	{
		VulkanRenderAPI* render = Render::RenderCommand::Get()->As<VulkanRenderAPI>();

		if (m_Sampler != nullptr)
		{
			vkDestroySampler(render->GetDevice(), m_Sampler, nullptr);
			m_Sampler = nullptr;
        }

		if (m_ImageView != nullptr)
		{
			vkDestroyImageView(render->GetDevice(), m_ImageView, nullptr);
			m_ImageView = nullptr;
		}

		if (m_Image != nullptr && m_Specification.bOwnsImage == true)
		{
			vmaDestroyImage(render->GetAllocator(), m_Image, m_Allocation);
			m_Image = VK_NULL_HANDLE;
			m_Allocation = VK_NULL_HANDLE;
		}
	}

    void VulkanTexture2D::Bind(ME::Core::Memory::Reference<CommandBuffer> commandBuffer, ME::Core::Memory::Reference<Pipeline> pipeline)
    {
		RenderCommand::GetResourceHandler()->As<VulkanResourceHandler>()->BindResourceSet(commandBuffer, pipeline, m_Specification.Set, m_Specification.SetIndex);
    }

    void VulkanTexture2D::Barrier(ME::Core::Memory::Reference<CommandBuffer> commandBuffer, BarrierInfo src,
        BarrierInfo dst)
    {
		RenderCommand::GetResourceHandler()->As<VulkanResourceHandler>()->TextureBarrier(commandBuffer, m_Image, GetBaseSpecification(), src, dst);
    }

    void VulkanTexture2D::Write()
    {
		RenderCommand::GetResourceHandler()->As<VulkanResourceHandler>()->WriteResource(this);
    }

    void VulkanTexture2D::Init()
	{
		m_ImageFormat = ME::Render::ConvertFormatVulkan(m_Specification.Format);

		VkResult result = CreateImage();
		if (ME_VK_FAILED(result))
		{
			ME_ASSERT(false, "Vulkan Texture2D: failed to create image!: Error: {0}", static_cast<uint32>(result));
			Shutdown();
			return;
		}

		result = UpdateImage(m_Specification.Data, m_Specification.DataSize);
		if (ME_VK_FAILED(result))
		{
			ME_ASSERT(false, "Vulkan Texture2D: failed to update image data!: Error: {0}", static_cast<uint32>(result));
			return;
		}

		result = CreateImageView();
		if (ME_VK_FAILED(result))
		{
			ME_ASSERT(false, "Vulkan Texture2D: failed to create image view!: Error: {0}", static_cast<uint32>(result));
			return;
		}

		result = CreateSampler();
		if (ME_VK_FAILED(result))
			ME_ASSERT(false, "Vulkan Texture2D: failed to create sampler!: Error: {0}", static_cast<uint32>(result));
	}
	
	void VulkanTexture2D::Init(VkImage image)
	{
        m_Image = image;
		m_ImageFormat = ME::Render::ConvertFormatVulkan(m_Specification.Format);

		VkResult result = CreateImageView();
		if (ME_VK_FAILED(result))
			ME_ASSERT(false, "Vulkan Texture2D: failed to create image view!: Error: {0}", static_cast<uint32>(result));
	}

	VkResult VulkanTexture2D::CreateImage()
	{
		VulkanRenderAPI* render = Render::RenderCommand::Get()->As<VulkanRenderAPI>();

		VkImageCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		createInfo.imageType = VK_IMAGE_TYPE_2D;
		createInfo.format = ME::Render::ConvertFormatVulkan(m_Specification.Format);
		createInfo.extent.width = m_Specification.Resolution.x;
		createInfo.extent.height = m_Specification.Resolution.y;
		createInfo.extent.depth = 1;
		createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		createInfo.mipLevels = m_Specification.MipLevels;
		createInfo.queueFamilyIndexCount = 0;
		createInfo.pQueueFamilyIndices = nullptr;
		createInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		createInfo.samples = ME::Render::ConvertSampleCountVulkan(m_Specification.SampleCount);
		createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.usage = (ME::Render::ConvertImageUsageFlagsVulkan(m_Specification.Usage) & VK_IMAGE_USAGE_TRANSFER_DST_BIT) ? ME::Render::ConvertImageUsageFlagsVulkan(m_Specification.Usage) : ME::Render::ConvertImageUsageFlagsVulkan(m_Specification.Usage) | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		createInfo.arrayLayers = 1;

		VmaAllocationCreateInfo allocCreateInfo = {};
		allocCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

		return vmaCreateImage(render->GetAllocator(), &createInfo, &allocCreateInfo, &m_Image, &m_Allocation, nullptr);
	}

	VkResult VulkanTexture2D::CreateImageView()
	{
		VkImageViewCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = m_Image;
		createInfo.format = m_ImageFormat;
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.levelCount = m_Specification.MipLevels;
		createInfo.subresourceRange.layerCount = m_Specification.CubeMapCount > 0 ? 6 * m_Specification.CubeMapCount : 1;
		createInfo.viewType = m_Specification.CubeMapCount > 0 ? VK_IMAGE_VIEW_TYPE_CUBE : VK_IMAGE_VIEW_TYPE_2D;
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		if (m_Specification.IsDepth)  createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		if (m_Specification.IsStencil) createInfo.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;

		return vkCreateImageView(Render::RenderCommand::Get()->As<VulkanRenderAPI>()->GetDevice(), &createInfo, nullptr, &m_ImageView);
	}

	VkResult VulkanTexture2D::CreateSampler()
	{
		VkSamplerCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		createInfo.minFilter = ConvertSamplerFilterVulkan(m_Specification.MinFilter);
		createInfo.magFilter = ConvertSamplerFilterVulkan(m_Specification.MagFilter);
		createInfo.anisotropyEnable = m_Specification.EnableAnisotropy;
		createInfo.maxAnisotropy = m_Specification.MaxAnisotropy;
		createInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		createInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		createInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		createInfo.minLod = 0;
		createInfo.maxLod = m_Specification.MaxLOD;
		createInfo.mipmapMode = m_Specification.MaxLOD > 0 ? VK_SAMPLER_MIPMAP_MODE_LINEAR : VK_SAMPLER_MIPMAP_MODE_NEAREST;
		createInfo.mipLodBias = 0;
		
		return vkCreateSampler(Render::RenderCommand::Get()->As<VulkanRenderAPI>()->GetDevice(), &createInfo, nullptr, &m_Sampler);
	}

	VkResult VulkanTexture2D::UpdateImage(void* data, SIZE_T size)
	{
		if (data == nullptr || size <= 0) return VK_SUCCESS;
		ME::Core::Memory::Reference<ME::Render::CommandBuffer> commandBuffer = RenderCommand::Get()->GetSingleUseCommandBuffer();

		VkBuffer stagingBuffer = nullptr;
		VmaAllocation stagingAllocation = nullptr;
		void* mappedData = nullptr;

		VkBufferCreateInfo stagingBufferCreateInfo = {};
		stagingBufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		stagingBufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		stagingBufferCreateInfo.size = static_cast<uint64>(m_Specification.DataSize);
		stagingBufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		stagingBufferCreateInfo.queueFamilyIndexCount = 0;
		stagingBufferCreateInfo.pQueueFamilyIndices = nullptr;

		VmaAllocationCreateInfo allocCreateInfo = {};
		allocCreateInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;
		allocCreateInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;

		VkBufferImageCopy region = {};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;

		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = m_Specification.CubeMapCount > 0 ? 6 * m_Specification.CubeMapCount : 1;
		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		if (m_Specification.IsDepth)  region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		if (m_Specification.IsStencil) region.imageSubresource.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;

		region.imageOffset = { 0, 0, 0 };
		region.imageExtent = 
		{
			m_Specification.Resolution.x,
			m_Specification.Resolution.y,
			1
		};

		VkResult result = vmaCreateBuffer(Render::RenderCommand::Get()->As<VulkanRenderAPI>()->GetAllocator(), &stagingBufferCreateInfo, &allocCreateInfo, &stagingBuffer, &stagingAllocation, nullptr);
		if (ME_VK_FAILED(result))
			return result;

		result = vmaMapMemory(Render::RenderCommand::Get()->As<VulkanRenderAPI>()->GetAllocator(), stagingAllocation, &mappedData);
		if (ME_VK_FAILED(result))
			return result;

		memcpy(mappedData, data, size);

		vmaUnmapMemory(Render::RenderCommand::Get()->As<VulkanRenderAPI>()->GetAllocator(), stagingAllocation);

		VkImageMemoryBarrier barrier = {};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.image = m_Image;
		barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
		barrier.subresourceRange.layerCount = m_Specification.CubeMapCount > 0 ? 6 * m_Specification.CubeMapCount : 1;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		if (m_Specification.IsDepth)  barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		if (m_Specification.IsStencil) barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;

		vkCmdPipelineBarrier(commandBuffer->As<VulkanCommandBuffer>()->GetCommandBuffer(), VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

	    vkCmdCopyBufferToImage(commandBuffer->As<VulkanCommandBuffer>()->GetCommandBuffer(), stagingBuffer, m_Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier.newLayout = ConvertImageLayoutVulkan(m_Specification.Layout);
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		vkCmdPipelineBarrier(commandBuffer->As<VulkanCommandBuffer>()->GetCommandBuffer(), VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &barrier);

		RenderCommand::Get()->SubmitAndFreeSingleUseCommandBuffer(commandBuffer);

		vmaDestroyBuffer(Render::RenderCommand::Get()->As<VulkanRenderAPI>()->GetAllocator(), stagingBuffer, stagingAllocation);

		return VK_SUCCESS;
	}
}
