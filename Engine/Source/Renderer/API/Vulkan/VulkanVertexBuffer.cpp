#include "VulkanVertexBuffer.hpp"
#include "VulkanRenderAPI.hpp"
#include "VulkanCommandBuffer.hpp"
#include "Renderer/RenderCommand.hpp"
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
		: VertexBuffer(specification), m_Buffer(nullptr), m_Allocation(nullptr),
		m_ResourceIndex(m_Specification.SetIndex)
	{
		Init(specification);
	}

	VulkanVertexBuffer::~VulkanVertexBuffer()
	{
		Shutdown();
	}

	//void VulkanVertexBuffer::Bind(ME::Core::Memory::Reference<ME::Render::CommandBuffer> commandBuffer)
	//{
	//	VkDeviceSize offset = 0;
	//	vkCmdBindVertexBuffers(commandBuffer->As<VulkanCommandBuffer>()->GetCommandBuffer(), 0, 1, &m_Buffer, &offset);
	//}

	void VulkanVertexBuffer::SetData(void* data, SIZE_T size, SIZE_T offset)
	{
		ME::Core::Memory::Reference<ME::Render::CommandBuffer> commandBuffer = RenderCommand::Get()->GetSingleUseCommandBuffer();
		SetData(commandBuffer, data, size, offset);
		RenderCommand::Get()->SubmitAndFreeSingleUseCommandBuffer(commandBuffer);
	}

	void VulkanVertexBuffer::SetData(ME::Core::Memory::Reference<ME::Render::CommandBuffer> commandBuffer, void* data, SIZE_T size, SIZE_T offset)
	{
		ME_ASSERT(size + offset <= m_Specification.Size, "Trying to set data with different size in vertex buffer \"{0}\"!", m_DebugName);
		void* bufferData;
		VkBufferCopy bufferCopy;
		VulkanRenderAPI* render = Render::RenderCommand::Get()->As<VulkanRenderAPI>();

		VkResult result = vmaMapMemory(render->GetAllocator(), m_StagingAllocation, &bufferData);
		if (ME_VK_FAILED(result))
		{
			ME_ASSERT(false, "Vulkan: data mapping in vertex buffer \"{0}\" failed! Error: {1}", m_DebugName, static_cast<int32>(result));
			Shutdown();
		}

		memcpy(static_cast<char8*>(bufferData) + offset, data, size);

		vmaUnmapMemory(render->GetAllocator(), m_StagingAllocation);

		result = vmaFlushAllocation(render->GetAllocator(), m_StagingAllocation, offset, size);
		if (ME_VK_FAILED(result))
		{
			ME_ASSERT(false, "Vulkan: allocation flushing in vertex buffer \"{0}\" failed! Error: {1}", m_DebugName, static_cast<int32>(result));
			Shutdown();
		}

		bufferCopy = {offset, offset, size};

		vkCmdCopyBuffer(commandBuffer->As<VulkanCommandBuffer>()->GetCommandBuffer(), m_StagingBuffer, m_Buffer, 1, &bufferCopy);

		VkBufferMemoryBarrier barrier = {};
		barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
		barrier.srcAccessMask = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.buffer = m_Buffer;
		barrier.offset = 0;
		barrier.size = m_Specification.Size;

		vkCmdPipelineBarrier(commandBuffer->As<VulkanCommandBuffer>()->GetCommandBuffer(),
			VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,
			0,
			0, nullptr,
			1, &barrier,
			0, nullptr);
	}

	void VulkanVertexBuffer::Resize(SIZE_T size)
	{
		ME::Core::Memory::Reference<ME::Render::CommandBuffer> commandBuffer = RenderCommand::Get()->GetSingleUseCommandBuffer();
		VmaAllocation oldAlloc = m_Allocation;
		VkBuffer oldBuffer = m_Buffer;
		SIZE_T oldSize = m_Specification.Size;
		VulkanRenderAPI* render = Render::RenderCommand::Get()->As<VulkanRenderAPI>();
		VkBufferCopy bufferCopy;

		m_Specification.Size = size;
		CreateBuffer();

		VkBufferMemoryBarrier inBarrier = {};
		inBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
		inBarrier.srcAccessMask = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
		inBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		inBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		inBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		inBarrier.buffer = oldBuffer;
		inBarrier.offset = 0;
		inBarrier.size = oldSize;

		vkCmdPipelineBarrier(commandBuffer->As<VulkanCommandBuffer>()->GetCommandBuffer(),
			VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			0,
			0, nullptr,
			1, &inBarrier,
			0, nullptr);

		bufferCopy = {0, 0, oldSize};
		vkCmdCopyBuffer(commandBuffer->As<VulkanCommandBuffer>()->GetCommandBuffer(), oldBuffer, m_Buffer, 1, &bufferCopy);

		VkBufferMemoryBarrier outBarrier = {};
		outBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
		outBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		outBarrier.dstAccessMask = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
		outBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		outBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		outBarrier.buffer = m_Buffer;
		outBarrier.offset = 0;
		outBarrier.size = m_Specification.Size;

		vkCmdPipelineBarrier(commandBuffer->As<VulkanCommandBuffer>()->GetCommandBuffer(),
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,
			0,
			0, nullptr,
			1, &outBarrier,
			0, nullptr);

		// Submit and wait for fences
		Render::RenderCommand::Get()->SubmitAndFreeSingleUseCommandBuffer(commandBuffer);

		// Then delete old buffer
		vmaDestroyBuffer(Render::RenderCommand::Get()->As<VulkanRenderAPI>()->GetAllocator(), oldBuffer, oldAlloc);

		// Recreate staging buffer
		vmaDestroyBuffer(RenderCommand::Get()->As<VulkanRenderAPI>()->GetAllocator(), m_StagingBuffer, m_StagingAllocation);
		CreateStagingBuffer();
	}

	void VulkanVertexBuffer::Clear()
	{
		ME::Core::Memory::Reference<ME::Render::CommandBuffer> commandBuffer = RenderCommand::Get()->GetSingleUseCommandBuffer();
		Clear(commandBuffer);
		RenderCommand::Get()->SubmitAndFreeSingleUseCommandBuffer(commandBuffer);
	}

	void VulkanVertexBuffer::Clear(ME::Core::Memory::Reference<ME::Render::CommandBuffer> commandBuffer)
	{
		VkBufferMemoryBarrier barrier = {};
		barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
		barrier.srcAccessMask = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.buffer = m_Buffer;
		barrier.offset = 0;
		barrier.size = m_Specification.Size;

		vkCmdPipelineBarrier(commandBuffer->As<VulkanCommandBuffer>()->GetCommandBuffer(),
			VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
			0,
			0, nullptr,
			1, &barrier,
			0, nullptr);

		vkCmdFillBuffer(commandBuffer->As<VulkanCommandBuffer>()->GetCommandBuffer(), m_Buffer, 0, m_Specification.Size, 0);
	}

	void VulkanVertexBuffer::Shutdown()
	{
		if (m_StagingBuffer)
		{
			vmaDestroyBuffer(Render::RenderCommand::Get()->As<VulkanRenderAPI>()->GetAllocator(), m_StagingBuffer, m_StagingAllocation);
			m_StagingBuffer = nullptr;
			m_StagingAllocation = nullptr;
		}
		if (m_Buffer)
		{
			vmaDestroyBuffer(Render::RenderCommand::Get()->As<VulkanRenderAPI>()->GetAllocator(), m_Buffer, m_Allocation);
			m_Buffer = nullptr;
			m_Allocation = nullptr;
		}
	}

	void VulkanVertexBuffer::Init(const VertexBufferSpecification& specification)
	{
		VkResult result = CreateBuffer();
		if (ME_VK_FAILED(result))
		{
			ME_ASSERT(false, "Vulkan: vertex buffer \"{0}\" creation failed! Error: {1}", m_DebugName, static_cast<int32>(result));
			Shutdown();
		}

		result = CreateStagingBuffer();
		if (ME_VK_FAILED(result))
		{
			ME_ASSERT(false, "Vulkan: data update in vertex buffer \"{0}\" failed! Error: {1}", m_DebugName, static_cast<int32>(result));
			Shutdown();
		}
	}

	VkResult VulkanVertexBuffer::CreateBuffer()
	{
		VkBufferCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		createInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		createInfo.size = static_cast<uint64>(m_Specification.Size);
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
		createInfo.size = static_cast<uint64>(m_Specification.Size);
		createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0;
		createInfo.pQueueFamilyIndices = nullptr;

		VmaAllocationCreateInfo allocCreateInfo = {};
		allocCreateInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;
		allocCreateInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;

		return vmaCreateBuffer(Render::RenderCommand::Get()->As<VulkanRenderAPI>()->GetAllocator(), &createInfo, &allocCreateInfo, &m_StagingBuffer, &m_StagingAllocation, nullptr);
	}
}
