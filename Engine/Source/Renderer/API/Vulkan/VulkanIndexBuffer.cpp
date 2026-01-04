#include "VulkanIndexBuffer.hpp"

#include "VulkanRenderAPI.hpp"
#include "VulkanCommandBuffer.hpp"
#include "VulkanResourceHandler.hpp"
#include "Renderer/RenderCommand.hpp"


namespace ME::Render
{
	ME::Core::Memory::Reference<ME::Render::IndexBuffer> IndexBuffer::CreateVulkan(const IndexBufferSpecification& specification)
	{
		auto object = ME::Core::Memory::MakeReference<VulkanIndexBuffer>(specification);
		return object;
	}

	VulkanIndexBuffer::VulkanIndexBuffer(const IndexBufferSpecification& specification)
		: IndexBuffer(specification),
        m_Buffer(nullptr), m_Allocation(nullptr),
        m_StagingBuffer(nullptr), m_StagingAllocation(nullptr),
		m_ResourceIndex(m_Specification.SetIndex)
	{
		Init();
	}

	VulkanIndexBuffer::~VulkanIndexBuffer()
	{
		Shutdown();
	}

	void VulkanIndexBuffer::SetData(uint32* indices, SIZE_T indexCount, SIZE_T offset)
	{
		ME::Core::Memory::Reference<ME::Render::CommandBuffer> commandBuffer = RenderCommand::Get()->GetSingleUseCommandBuffer();
		SetData(commandBuffer, indices, indexCount, offset);
		RenderCommand::Get()->SubmitAndFreeSingleUseCommandBuffer(commandBuffer);
	}

	void VulkanIndexBuffer::SetData(ME::Core::Memory::Reference<ME::Render::CommandBuffer> commandBuffer, uint32* indices, SIZE_T indexCount, SIZE_T offset)
	{
		ME_ASSERT(indexCount <= m_Specification.IndexCount, "Trying to set data with different size in index buffer \"{0}\"!", m_DebugName);
        void* bufferData;
		VkBufferCopy bufferCopy;
		VulkanRenderAPI* render = Render::RenderCommand::Get()->As<VulkanRenderAPI>();

		VkResult result = vmaMapMemory(render->GetAllocator(), m_StagingAllocation, &bufferData);
		if (ME_VK_FAILED(result))
		{
			ME_ASSERT(false, ME_VK_LOG_OUTPUT_FORMAT("IndexBuffer", "Failed to map data! Error code: {1}"),
				m_DebugName, static_cast<uint32>(result));
			Shutdown();
		}

		memcpy(static_cast<char8*>(bufferData) + (sizeof(uint32) * offset), indices, sizeof(uint32) * indexCount);

		vmaUnmapMemory(render->GetAllocator(), m_StagingAllocation);

		result = vmaFlushAllocation(render->GetAllocator(), m_StagingAllocation, sizeof(uint32) * offset, sizeof(uint32) * indexCount);
		if (ME_VK_FAILED(result))
		{
			ME_ASSERT(false, ME_VK_LOG_OUTPUT_FORMAT("IndexBuffer", "Failed to flush allocation! Error code: {1}"),
				m_DebugName, static_cast<uint32>(result));
		    Shutdown();
		}

		bufferCopy = { sizeof(uint32) * offset, sizeof(uint32) * offset, sizeof(uint32) * m_Specification.IndexCount };

		vkCmdCopyBuffer(commandBuffer->As<VulkanCommandBuffer>()->GetCommandBuffer(), m_StagingBuffer, m_Buffer, 1, &bufferCopy);

		VkBufferMemoryBarrier barrier = {};
		barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_INDEX_READ_BIT;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.buffer = m_Buffer;
		barrier.offset = 0;
		barrier.size = m_Specification.IndexCount * sizeof(uint32);

		vkCmdPipelineBarrier(commandBuffer->As<VulkanCommandBuffer>()->GetCommandBuffer(),
			VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, 
			0, 
			0, nullptr, 
			1, &barrier,
			0, nullptr);
	}

	MappedBufferData VulkanIndexBuffer::Map()
	{
		MappedBufferData data = {};
		data.Data = nullptr;
		data.Size = 0;

		VulkanRenderAPI* render = Render::RenderCommand::Get()->As<VulkanRenderAPI>();
		VkResult result = vmaMapMemory(render->GetAllocator(), m_StagingAllocation, &data.Data);
		if (ME_VK_FAILED(result))
		    ME_ASSERT(false, ME_VK_LOG_OUTPUT_FORMAT("IndexBuffer", "Failed to map! Error code: {1}"),
			    m_DebugName, static_cast<uint32>(result));
	    else
			data.Size = sizeof(uint32) * m_Specification.IndexCount;

		return data;
	}

	void VulkanIndexBuffer::Unmap()
	{
		ME::Core::Memory::Reference<ME::Render::CommandBuffer> commandBuffer = RenderCommand::Get()->GetSingleUseCommandBuffer();
		VulkanRenderAPI* render = Render::RenderCommand::Get()->As<VulkanRenderAPI>();
		vmaUnmapMemory(render->GetAllocator(), m_StagingAllocation);

		VkResult result = vmaFlushAllocation(render->GetAllocator(), m_StagingAllocation, 0, sizeof(uint32) * m_Specification.IndexCount);
		if (ME_VK_FAILED(result))
		{
			ME_ASSERT(false, ME_VK_LOG_OUTPUT_FORMAT("IndexBuffer", "Failed to flush allocation! Error code: {1}"),
				m_DebugName, static_cast<uint32>(result));
			Shutdown();
		}

		VkBufferCopy bufferCopy = { 0, 0, sizeof(uint32) * m_Specification.IndexCount };

		vkCmdCopyBuffer(commandBuffer->As<VulkanCommandBuffer>()->GetCommandBuffer(), m_StagingBuffer, m_Buffer, 1, &bufferCopy);

		VkBufferMemoryBarrier barrier = {};
		barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.buffer = m_Buffer;
		barrier.offset = 0;
		barrier.size = m_Specification.IndexCount * sizeof(uint32);

		vkCmdPipelineBarrier(commandBuffer->As<VulkanCommandBuffer>()->GetCommandBuffer(),
			VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,
			0,
			0, nullptr,
			1, &barrier,
			0, nullptr);

		Render::RenderCommand::Get()->SubmitAndFreeSingleUseCommandBuffer(commandBuffer);
	}

	void VulkanIndexBuffer::Resize(SIZE_T indexCount)
	{
		ME::Core::Memory::Reference<ME::Render::CommandBuffer> commandBuffer = RenderCommand::Get()->GetSingleUseCommandBuffer();

		VmaAllocation oldAlloc = m_Allocation;
		VkBuffer oldBuffer = m_Buffer;
		SIZE_T oldCount = m_Specification.IndexCount;
		VulkanRenderAPI* render = Render::RenderCommand::Get()->As<VulkanRenderAPI>();
		VkBufferCopy bufferCopy;

		m_Specification.IndexCount = indexCount;

		CreateBuffer();

		VkBufferMemoryBarrier inBarrier = {};
		inBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
		inBarrier.srcAccessMask = VK_ACCESS_INDEX_READ_BIT;
		inBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		inBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		inBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		inBarrier.buffer = m_Buffer;
		inBarrier.offset = 0;
		inBarrier.size = m_Specification.IndexCount * sizeof(uint32);

		vkCmdPipelineBarrier(commandBuffer->As<VulkanCommandBuffer>()->GetCommandBuffer(),
			VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			0,
			0, nullptr,
			0, &inBarrier,
			0, nullptr);

		bufferCopy = { 0, 0, sizeof(uint32) * oldCount };
		vkCmdCopyBuffer(commandBuffer->As<VulkanCommandBuffer>()->GetCommandBuffer(), oldBuffer, m_Buffer, 1, &bufferCopy);

		VkBufferMemoryBarrier outBarrier = {};
		outBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
		outBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		outBarrier.dstAccessMask = VK_ACCESS_INDEX_READ_BIT;
		outBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		outBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		outBarrier.buffer = m_Buffer;
		outBarrier.offset = 0;
		outBarrier.size = m_Specification.IndexCount * sizeof(uint32);

		vkCmdPipelineBarrier(commandBuffer->As<VulkanCommandBuffer>()->GetCommandBuffer(),
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,
			0,
			0, nullptr,
			0, &outBarrier,
			0, nullptr);

		// Submit and wait for fences
		Render::RenderCommand::Get()->SubmitAndFreeSingleUseCommandBuffer(commandBuffer);

		// Then delete old buffer
		vmaDestroyBuffer(Render::RenderCommand::Get()->As<VulkanRenderAPI>()->GetAllocator(), oldBuffer, oldAlloc);
		render->NameVulkanObject(m_DebugName, ME_VK_TO_UINT_HANDLE(m_Buffer), VK_OBJECT_TYPE_BUFFER);

		// Recreate staging buffer
		vmaDestroyBuffer(RenderCommand::Get()->As<VulkanRenderAPI>()->GetAllocator(), m_StagingBuffer, m_StagingAllocation);
		CreateStagingBuffer();
		render->NameVulkanObject(m_DebugName + TEXT(" Staging"), ME_VK_TO_UINT_HANDLE(m_StagingBuffer), VK_OBJECT_TYPE_BUFFER);
	}

	void VulkanIndexBuffer::Clear()
	{
		ME::Core::Memory::Reference<ME::Render::CommandBuffer> commandBuffer = RenderCommand::Get()->GetSingleUseCommandBuffer();
		Clear(commandBuffer);
		RenderCommand::Get()->SubmitAndFreeSingleUseCommandBuffer(commandBuffer);
	}

	void VulkanIndexBuffer::Clear(ME::Core::Memory::Reference<ME::Render::CommandBuffer> commandBuffer)
	{
		VkBufferMemoryBarrier barrier = {};
		barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
		barrier.srcAccessMask = VK_ACCESS_INDEX_READ_BIT;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.buffer = m_Buffer;
		barrier.offset = 0;
		barrier.size = m_Specification.IndexCount * sizeof(uint32);

		vkCmdPipelineBarrier(commandBuffer->As<VulkanCommandBuffer>()->GetCommandBuffer(), 
			VK_PIPELINE_STAGE_VERTEX_INPUT_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
			0,
			0, nullptr,
			1, &barrier,
			0, nullptr);

		vkCmdFillBuffer(commandBuffer->As<VulkanCommandBuffer>()->GetCommandBuffer(), m_Buffer, 0, m_Specification.IndexCount * sizeof(uint32), 0);
	}

	void VulkanIndexBuffer::Shutdown()
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

    void VulkanIndexBuffer::Bind(ME::Core::Memory::Reference<CommandBuffer> commandBuffer, uint32 offset)
	{
		RenderCommand::Get()->As<VulkanRenderAPI>()->BindIndexBuffer(commandBuffer, m_Buffer, offset);
	}

    void VulkanIndexBuffer::Bind(ME::Core::Memory::Reference<CommandBuffer> commandBuffer,
                                 ME::Core::Memory::Reference<Pipeline> pipeline)
    {
		RenderCommand::GetResourceHandler()->BindResourceSet(commandBuffer, pipeline, m_Specification.Set, m_ResourceIndex);
    }

	void VulkanIndexBuffer::Write()
	{
		Write(m_Specification.IndexCount, 0, m_Specification.Binding);
	}

	void VulkanIndexBuffer::Write(SIZE_T offset)
	{
		Write(m_Specification.IndexCount, offset, m_Specification.Binding);
	}

	void VulkanIndexBuffer::Write(SIZE_T offset, uint32 binding)
	{
		Write(m_Specification.IndexCount, offset, binding);
	}

	void VulkanIndexBuffer::Write(SIZE_T size, SIZE_T offset)
	{
		Write(size, offset, m_Specification.Binding);
	}

	void VulkanIndexBuffer::Write(SIZE_T size, SIZE_T offset, uint32 binding)
	{
		RenderCommand::GetResourceHandler()->As<VulkanResourceHandler>()->WriteResource(this, size, offset, binding);
	}

    void VulkanIndexBuffer::Barrier(ME::Core::Memory::Reference<CommandBuffer> commandBuffer, BarrierInfo src,
        BarrierInfo dst)
    {
		RenderCommand::GetResourceHandler()->As<VulkanResourceHandler>()->BufferBarrier(commandBuffer, m_Buffer, src, dst);
    }

    void VulkanIndexBuffer::Init()
	{
		m_DebugName = m_Specification.DebugName;
		VulkanRenderAPI* render = Render::RenderCommand::Get()->As<VulkanRenderAPI>();
		VkResult result = CreateBuffer();
		if (ME_VK_FAILED(result))
		{
			ME_ASSERT(false, ME_VK_LOG_OUTPUT_FORMAT("IndexBuffer", "Failed to create buffer! Error code: {1}"),
				m_DebugName, static_cast<uint32>(result));
			Shutdown();
		}
		render->NameVulkanObject(m_DebugName, ME_VK_TO_UINT_HANDLE(m_Buffer), VK_OBJECT_TYPE_BUFFER);

		result = CreateStagingBuffer();
		if (ME_VK_FAILED(result))
		{
			ME_ASSERT(false, ME_VK_LOG_OUTPUT_FORMAT("IndexBuffer", "Failed to create staging buffer! Error code: {1}"),
				m_DebugName, static_cast<uint32>(result));
			Shutdown();
		}
		render->NameVulkanObject(m_DebugName + TEXT(" Staging"), ME_VK_TO_UINT_HANDLE(m_StagingBuffer), VK_OBJECT_TYPE_BUFFER);
	}

	VkResult VulkanIndexBuffer::CreateBuffer()
	{
		VkBufferCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		createInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		createInfo.size = static_cast<uint64>(m_Specification.IndexCount * sizeof(int32));
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
		createInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		createInfo.size = static_cast<uint64>(m_Specification.IndexCount * sizeof(int32));
		createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0;
		createInfo.pQueueFamilyIndices = nullptr;

		VmaAllocationCreateInfo allocCreateInfo = {};
		allocCreateInfo.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;
		allocCreateInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;

		return vmaCreateBuffer(Render::RenderCommand::Get()->As<VulkanRenderAPI>()->GetAllocator(), &createInfo, &allocCreateInfo, &m_StagingBuffer, &m_StagingAllocation, nullptr);
	}
}