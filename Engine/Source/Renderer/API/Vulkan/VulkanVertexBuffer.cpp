#include "VulkanVertexBuffer.h"

#include "VulkanRenderAPI.h"
#include "VulkanCommandBuffer.h"
#include "Renderer/RenderCommand.h"
#include "Renderer/RenderResourcesTracker.hpp"

namespace ME::Render
{
	ME::Core::Memory::Reference<ME::Render::VertexBuffer> VertexBuffer::CreateVulkan(const VertexBufferSpecification& specification)
	{
		auto object = ME::Core::Memory::Reference<Render::VertexBuffer>(new VulkanVertexBuffer(specification));
		RenderResourcesTracker::Get().AddItem(object);
		return object;
	}

	VulkanVertexBuffer::VulkanVertexBuffer(const VertexBufferSpecification& specification)
		: m_Buffer(nullptr), m_Size(specification.Size), m_Allocation(nullptr), m_DebugName(specification.DebugName)
	{
		Init(specification);
	}

	VulkanVertexBuffer::~VulkanVertexBuffer()
	{
		Shutdown();
	}

	void VulkanVertexBuffer::Bind(ME::Core::Memory::Reference<ME::Render::CommandBuffer> commandBuffer)
	{
		VkDeviceSize offset = 0;
		vkCmdBindVertexBuffers(commandBuffer->As<VulkanCommandBuffer>()->GetBuffer(), 0, 1, &m_Buffer, &offset);
	}

	void VulkanVertexBuffer::Unbind()
	{
		//not required in Vulkan
	}

	void VulkanVertexBuffer::SetData(void* data, SIZE_T size)
	{
		return;
	}

	void VulkanVertexBuffer::SetData(ME::Core::Memory::Reference<ME::Render::CommandBuffer> commandBuffer, void* data, SIZE_T size)
	{
		ME_ASSERT(size == m_Size, TEXT("Trying to set data with different size in vertex buffer \"{0}\"!"));
		void* bufferData;
		VkBufferCopy bufferCopy;
		VulkanRenderAPI* render = Render::RenderCommand::Get()->As<VulkanRenderAPI>();

		VkResult result = vmaMapMemory(render->GetAllocator(), m_StagingAllocation, &bufferData);
		if (ME_VK_FAILED(result))
		{
			ME_ASSERT(false, TEXT("Vulkan: data mapping in vertex buffer \"{0}\" failed! Error: {1}"), m_DebugName.GetString(), static_cast<int32>(result));
			Shutdown();
		}

		memcpy(bufferData, data, m_Size);

		vmaUnmapMemory(render->GetAllocator(), m_StagingAllocation);

		result = vmaFlushAllocation(render->GetAllocator(), m_StagingAllocation, 0, m_Size);
		if (ME_VK_FAILED(result))
		{
			ME_ASSERT(false, TEXT("Vulkan: allocation flushing in vertex buffer \"{0}\" failed! Error: {1}"), m_DebugName.GetString(), static_cast<int32>(result));
			Shutdown();
		}

		bufferCopy = { 0, 0, m_Size };

		vkCmdCopyBuffer(commandBuffer->As<VulkanCommandBuffer>()->GetBuffer(), m_StagingBuffer, m_Buffer, 1, &bufferCopy);

		VkMemoryBarrier barrier = {};
		barrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;

		vkCmdPipelineBarrier(commandBuffer->As<VulkanCommandBuffer>()->GetBuffer(), VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, 0, 1, &barrier, 0, nullptr, 0, nullptr);
	}

	void VulkanVertexBuffer::Shutdown()
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

	void VulkanVertexBuffer::Init(const VertexBufferSpecification& specification)
	{
		VkResult result = CreateBuffer();
		if (ME_VK_FAILED(result))
		{
			ME_ASSERT(false, TEXT("Vulkan: vertex buffer \"{0}\" creation failed! Error: {1}"), m_DebugName.GetString(), static_cast<int32>(result));
			Shutdown();
		}

		result = CreateStagingBuffer();
		if (ME_VK_FAILED(result))
		{
			ME_ASSERT(false, TEXT("Vulkan: data update in vertex buffer \"{0}\" failed! Error: {1}"), m_DebugName.GetString(), static_cast<int32>(result));
			Shutdown();
		}
	}

	VkResult VulkanVertexBuffer::CreateBuffer()
	{
		VkBufferCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		createInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		createInfo.size = static_cast<uint64>(m_Size);
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

	VkResult VulkanVertexBuffer::CreateStagingBuffer()
	{
		VkBufferCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		createInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		createInfo.size = static_cast<uint64>(m_Size);
		createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0;
		createInfo.pQueueFamilyIndices = nullptr;

		VmaAllocationCreateInfo allocCreateInfo = {};
		allocCreateInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;
		allocCreateInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;

		return vmaCreateBuffer(Render::RenderCommand::Get()->As<VulkanRenderAPI>()->GetAllocator(), &createInfo, &allocCreateInfo, &m_StagingBuffer, &m_StagingAllocation, nullptr);
	}
}
