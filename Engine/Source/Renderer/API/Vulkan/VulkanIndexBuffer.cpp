#include "VulkanIndexBuffer.h"

#include "VulkanRenderAPI.h"
#include "VulkanCommandBuffer.h"
#include "Renderer/RenderCommand.h"
#include "Renderer/RenderResourcesTracker.hpp"

namespace ME::Render
{
	ME::Core::Memory::Reference<ME::Render::IndexBuffer> IndexBuffer::CreateVulkan(const IndexBufferSpecification& specification)
	{
		auto object = ME::Core::Memory::Reference<Render::IndexBuffer>(new VulkanIndexBuffer(specification));
		RenderResourcesTracker::Get().AddItem(object);
		return object;
	}

	VulkanIndexBuffer::VulkanIndexBuffer(const IndexBufferSpecification& specification)
		: m_Buffer(nullptr), m_IndexCount(specification.IndexCount), m_Allocation(nullptr), m_DebugName(specification.DebugName)
	{
		Init(specification);
	}

	VulkanIndexBuffer::~VulkanIndexBuffer()
	{
		Shutdown();
	}

	void VulkanIndexBuffer::Bind(ME::Core::Memory::Reference<ME::Render::CommandBuffer> commandBuffer)
	{
		VkDeviceSize offset = 0;
		vkCmdBindIndexBuffer(commandBuffer->As<VulkanCommandBuffer>()->GetBuffer(), m_Buffer, 0, VK_INDEX_TYPE_UINT32);
	}

	void VulkanIndexBuffer::Unbind()
	{
		//not required in Vulkan
	}

	void VulkanIndexBuffer::SetData(ME::Core::Memory::Reference<ME::Render::CommandBuffer> commandBuffer, void* data, SIZE_T size)
	{
		ME_ASSERT(size % 4 == 0, TEXT("Trying to set data with different size in index buffer \"{0}\"!"));
		SetData(commandBuffer, static_cast<int32*>(data), size / 4);
	}

	void VulkanIndexBuffer::SetData(ME::Core::Memory::Reference<ME::Render::CommandBuffer> commandBuffer, int32* indices, SIZE_T indexCount)
	{
		ME_ASSERT(indexCount == m_IndexCount, TEXT("Trying to set data with different size in index buffer \"{0}\"!"));
		VkResult result;
		void* bufferData;
		VkBufferCopy bufferCopy;
		VulkanRenderAPI* render = Render::RenderCommand::Get()->As<VulkanRenderAPI>();

		result = vmaMapMemory(render->GetAllocator(), m_StagingAllocation, &bufferData);
		if (ME_VK_FAILED(result))
		{
			ME_ASSERT(false, TEXT("Vulkan: data mapping in index buffer \"{0}\" failed! Error: {1}"), m_DebugName.GetString(), static_cast<int32>(result));
			Shutdown();
		}

		memcpy(bufferData, indices, sizeof(int32) * m_IndexCount);

		vmaUnmapMemory(render->GetAllocator(), m_StagingAllocation);

		result = vmaFlushAllocation(render->GetAllocator(), m_StagingAllocation, 0, m_IndexCount * sizeof(4));
		if (ME_VK_FAILED(result))
		{
			ME_ASSERT(false, TEXT("Vulkan: allocation flushing in index buffer \"{0}\" failed! Error: {1}"), m_DebugName.GetString(), static_cast<int32>(result));
			Shutdown();
		}

		bufferCopy = { 0, 0, sizeof(int32) * m_IndexCount};

		vkCmdCopyBuffer(commandBuffer->As<VulkanCommandBuffer>()->GetBuffer(), m_StagingBuffer, m_Buffer, 1, &bufferCopy);

		VkMemoryBarrier barrier = {};
		barrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_INDEX_READ_BIT;

		vkCmdPipelineBarrier(commandBuffer->As<VulkanCommandBuffer>()->GetBuffer(), VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, 0, 1, &barrier, 0, nullptr, 0, nullptr);
	}

	void VulkanIndexBuffer::Shutdown()
	{
		if (m_StagingBuffer)
		{
			vmaDestroyBuffer(Render::RenderCommand::Get()->As<VulkanRenderAPI>()->GetAllocator(), m_StagingBuffer, m_StagingAllocation);
			m_Buffer = nullptr;
		}
		if (m_Buffer)
		{
			vmaDestroyBuffer(Render::RenderCommand::Get()->As<VulkanRenderAPI>()->GetAllocator(), m_Buffer, m_Allocation);
			m_Buffer = nullptr;
		}
	}

	void VulkanIndexBuffer::Init(const IndexBufferSpecification& specification)
	{
		VkResult result = CreateBuffer();
		if (ME_VK_FAILED(result))
		{
			ME_ASSERT(false, TEXT("Vulkan: index buffer \"{0}\" creation failed! Error: {1}"), m_DebugName.GetString(), static_cast<int32>(result));
			Shutdown();
		}

		result = CreateStagingBuffer();
		if (ME_VK_FAILED(result))
		{
			ME_ASSERT(false, TEXT("Vulkan: staging buffer in index buffer \"{0}\" failed! Error: {1}"), m_DebugName.GetString(), static_cast<int32>(result));
			Shutdown();
		}
	}

	VkResult VulkanIndexBuffer::CreateBuffer()
	{
		VkBufferCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		createInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		createInfo.size = static_cast<uint64>(m_IndexCount * sizeof(int32));
		createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0;
		createInfo.pQueueFamilyIndices = nullptr;

		VmaAllocationCreateInfo allocCreateInfo = {};
		allocCreateInfo.flags = 0;
		allocCreateInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
		allocCreateInfo.requiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
		allocCreateInfo.pool = nullptr;

		return vmaCreateBuffer(Render::RenderCommand::Get()->As<VulkanRenderAPI>()->GetAllocator(), &createInfo, &allocCreateInfo, &m_Buffer, &m_Allocation, nullptr);
	}

	VkResult VulkanIndexBuffer::CreateStagingBuffer()
	{
		VkBufferCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		createInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		createInfo.size = static_cast<uint64>(m_IndexCount * sizeof(int32));
		createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0;
		createInfo.pQueueFamilyIndices = nullptr;

		VmaAllocationCreateInfo allocCreateInfo = {};
		allocCreateInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;
		allocCreateInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;

		return vmaCreateBuffer(Render::RenderCommand::Get()->As<VulkanRenderAPI>()->GetAllocator(), &createInfo, &allocCreateInfo, &m_StagingBuffer, &m_StagingAllocation, nullptr);
	}
}
#include "VulkanIndexBuffer.h"
