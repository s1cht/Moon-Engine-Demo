#include "VulkanUniform.hpp"
#include "VulkanRenderAPI.hpp"
#include "VulkanCommandBuffer.hpp"
#include "VulkanPipeline.hpp"
#include "VulkanResourceHandler.hpp"
#include "Renderer/RenderCommand.hpp"


namespace ME::Render
{
	ME::Core::Memory::Reference<Uniform> Uniform::CreateVulkan(const UniformSpecification& specification)
	{
		auto object = ME::Core::Memory::MakeReference<VulkanUniform>(specification);
		return object;
	}

	VulkanUniform::VulkanUniform(const UniformSpecification& specification)
		: Uniform(specification),
        m_Buffer(nullptr), m_Allocation(nullptr),
        m_StagingBuffer(nullptr), m_StagingAllocation(nullptr),
        m_MappedData(nullptr),
        m_ResourceIndex(m_Specification.SetIndex)
	{
		Init();
	}

	VulkanUniform::~VulkanUniform()
	{
		Shutdown();
	}

	void VulkanUniform::SetData(void* data, SIZE_T size, SIZE_T offset)
	{
		ME_ASSERT(size == m_Specification.Size, "Trying to set data with different size in uniform \"{0}\"!", m_DebugName);
		ME::Core::Memory::Reference<CommandBuffer> commandBuffer = RenderCommand::Get()->GetSingleUseCommandBuffer();
		SetData(commandBuffer, data, size, offset);
		RenderCommand::Get()->SubmitAndFreeSingleUseCommandBuffer(commandBuffer);
	}

	void VulkanUniform::SetData(ME::Core::Memory::Reference<CommandBuffer> commandBuffer, void* data, SIZE_T size, SIZE_T offset)
	{
		ME_ASSERT(size + offset <= m_Specification.Size, "Trying to set data with different size in uniform \"{0}\"!", m_DebugName);

		switch (m_Specification.MemoryType)
		{
			case MemoryType::VRAM:
			{
				SetDataVRAM(commandBuffer, data, size, offset);
				break;
			}
			case MemoryType::RAM:
			{
				SetDataRAM(commandBuffer, data, size, offset);
				break;
			}
		}
	}

	MappedBufferData VulkanUniform::Map()
	{
		if (m_Specification.MemoryType == MemoryType::RAM) return MappedBufferData{ .Data = m_MappedData, .Size = m_Specification.Size };

		MappedBufferData data = {};
		data.Data = nullptr;
		data.Size = 0;

		VulkanRenderAPI* render = Render::RenderCommand::Get()->As<VulkanRenderAPI>();
		VkResult result = vmaMapMemory(render->GetAllocator(), m_StagingAllocation, &data.Data);
		if (ME_VK_FAILED(result))
			ME_ASSERT(false, ME_VK_LOG_OUTPUT_FORMAT("Uniform", "Failed to map data! Error code: {1}"),
				m_DebugName, static_cast<uint32>(result));
		else
			data.Size = m_Specification.Size;

		return data;
	}

	void VulkanUniform::Unmap()
	{
		if (m_Specification.MemoryType == MemoryType::RAM) return;

		ME::Core::Memory::Reference<ME::Render::CommandBuffer> commandBuffer = RenderCommand::Get()->GetSingleUseCommandBuffer();
		VulkanRenderAPI* render = Render::RenderCommand::Get()->As<VulkanRenderAPI>();
		vmaUnmapMemory(render->GetAllocator(), m_StagingAllocation);

		VkResult result = vmaFlushAllocation(render->GetAllocator(), m_StagingAllocation, 0, m_Specification.Size);
		if (ME_VK_FAILED(result))
		{
			ME_ASSERT(false, ME_VK_LOG_OUTPUT_FORMAT("Uniform", "Failed to flush allocation! Error code: {1}"),
				m_DebugName, static_cast<uint32>(result));
			Shutdown();
		}

		VkBufferCopy bufferCopy = { 0, 0, m_Specification.Size };

		vkCmdCopyBuffer(commandBuffer->As<VulkanCommandBuffer>()->GetCommandBuffer(), m_StagingBuffer, m_Buffer, 1, &bufferCopy);

		VkBufferMemoryBarrier barrier = {};
		barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
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

		Render::RenderCommand::Get()->SubmitAndFreeSingleUseCommandBuffer(commandBuffer);
	}

    void VulkanUniform::Resize(SIZE_T size)
    {
		ME::Core::Memory::Reference<CommandBuffer> commandBuffer = RenderCommand::Get()->GetSingleUseCommandBuffer();
		VkBuffer oldBuffer = m_Buffer;
		VmaAllocation oldAlloc = m_Allocation;
		SIZE_T oldSize = m_Specification.Size;
		VulkanRenderAPI* render = RenderCommand::Get()->As<VulkanRenderAPI>();
		VkBufferCopy bufferCopy;

		m_Specification.Size = size;

		CreateBuffer();

		VkBufferMemoryBarrier inBarrier = {};
		inBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
		inBarrier.srcAccessMask = VK_ACCESS_UNIFORM_READ_BIT;
		inBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		inBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		inBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		inBarrier.buffer = oldBuffer;
		inBarrier.offset = 0;
		inBarrier.size = oldSize;

		vkCmdPipelineBarrier(commandBuffer->As<VulkanCommandBuffer>()->GetCommandBuffer(),
			m_OptimalSrcPipelineStage,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			0,
			0, nullptr,
			1, &inBarrier,
			0, nullptr);

		bufferCopy = { 0, 0, oldSize };
		vkCmdCopyBuffer(commandBuffer->As<VulkanCommandBuffer>()->GetCommandBuffer(), oldBuffer, m_Buffer, 1, &bufferCopy);

		VkBufferMemoryBarrier outBarrier = {};
		outBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
		outBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		outBarrier.dstAccessMask = VK_ACCESS_UNIFORM_READ_BIT;
		outBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		outBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		outBarrier.buffer = m_Buffer;
		outBarrier.offset = 0;
		outBarrier.size = m_Specification.Size;

		vkCmdPipelineBarrier(commandBuffer->As<VulkanCommandBuffer>()->GetCommandBuffer(),
			VK_PIPELINE_STAGE_TRANSFER_BIT, m_OptimalDstPipelineStage,
			0,
			0, nullptr,
			1, &outBarrier,
			0, nullptr);

		// Submit and wait for fences
		RenderCommand::Get()->SubmitAndFreeSingleUseCommandBuffer(commandBuffer);

		// Then delete old buffer
		vmaDestroyBuffer(RenderCommand::Get()->As<VulkanRenderAPI>()->GetAllocator(), oldBuffer, oldAlloc);
		render->NameVulkanObject(m_DebugName, ME_VK_TO_UINT_HANDLE(m_Buffer), VK_OBJECT_TYPE_BUFFER);

		// If required, recreate the staging buffer
		if (m_Specification.MemoryType == MemoryType::RAM) return;
		vmaDestroyBuffer(RenderCommand::Get()->As<VulkanRenderAPI>()->GetAllocator(), m_StagingBuffer, m_StagingAllocation);
		CreateStagingBuffer();
		render->NameVulkanObject(m_DebugName + TEXT(" Staging"), ME_VK_TO_UINT_HANDLE(m_StagingBuffer), VK_OBJECT_TYPE_BUFFER);
    }

    void VulkanUniform::Clear()
    {
		ME::Core::Memory::Reference<CommandBuffer> commandBuffer = RenderCommand::Get()->GetSingleUseCommandBuffer();
		Clear(commandBuffer);
		RenderCommand::Get()->SubmitAndFreeSingleUseCommandBuffer(commandBuffer);
    }

    void VulkanUniform::Clear(ME::Core::Memory::Reference<CommandBuffer> commandBuffer)
    {
		VkBufferMemoryBarrier barrier = {};
		barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
		barrier.srcAccessMask = VK_ACCESS_UNIFORM_READ_BIT;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.buffer = m_Buffer;
		barrier.offset = 0;
		barrier.size = m_Specification.Size;

		vkCmdPipelineBarrier(commandBuffer->As<VulkanCommandBuffer>()->GetCommandBuffer(), 
			m_OptimalSrcPipelineStage, VK_PIPELINE_STAGE_TRANSFER_BIT,
			0,
			0, nullptr,
			1, &barrier,
			0, nullptr);

		vkCmdFillBuffer(commandBuffer->As<VulkanCommandBuffer>()->GetCommandBuffer(), m_Buffer, 0, m_Specification.Size, 0);
    }

    void VulkanUniform::Bind(ME::Core::Memory::Reference<CommandBuffer> commandBuffer,
        ME::Core::Memory::Reference<Pipeline> pipeline)
    {
		RenderCommand::GetResourceHandler()->As<VulkanResourceHandler>()->BindResourceSet(commandBuffer, pipeline, m_Specification.Set, m_ResourceIndex);
    }

	void VulkanUniform::Write()
	{
		Write(m_Specification.Size, 0, m_Specification.Binding);
	}

	void VulkanUniform::Write(SIZE_T offset)
	{
		Write(m_Specification.Size, offset, m_Specification.Binding);
	}

	void VulkanUniform::Write(SIZE_T offset, uint32 binding)
	{
		Write(m_Specification.Size, offset, binding);
	}

	void VulkanUniform::Write(SIZE_T size, SIZE_T offset)
	{
		Write(size, offset, m_Specification.Binding);
	}

	void VulkanUniform::Write(SIZE_T size, SIZE_T offset, uint32 binding)
	{
		RenderCommand::GetResourceHandler()->As<VulkanResourceHandler>()->WriteResource(this, size, offset, binding);
	}

    void VulkanUniform::Barrier(ME::Core::Memory::Reference<CommandBuffer> commandBuffer, BarrierInfo src,
        BarrierInfo dst)
    {
		RenderCommand::GetResourceHandler()->As<VulkanResourceHandler>()->BufferBarrier(commandBuffer, m_Buffer, src, dst);
    }

    void VulkanUniform::Shutdown()
	{
		if (m_StagingBuffer)
		{
			vmaDestroyBuffer(RenderCommand::Get()->As<VulkanRenderAPI>()->GetAllocator(), m_StagingBuffer, m_StagingAllocation);
			m_StagingBuffer = nullptr;
			m_StagingAllocation = nullptr;
		}
		if (m_Buffer)
		{
			vmaDestroyBuffer(RenderCommand::Get()->As<VulkanRenderAPI>()->GetAllocator(), m_Buffer, m_Allocation);
			m_Buffer = nullptr;
			m_Allocation = nullptr;
		}
	}

	void VulkanUniform::Init()
	{
		m_DebugName = m_Specification.DebugName;

		ChooseOptimalDstStage();
		ChooseOptimalSrcStage();

		VulkanRenderAPI* render = Render::RenderCommand::Get()->As<VulkanRenderAPI>();
		VkResult result = CreateBuffer();
		if (ME_VK_FAILED(result))
		{
			ME_ASSERT(false, ME_VK_LOG_OUTPUT_FORMAT("Uniform", "Failed to create buffer! Error code: {1}"),
				m_DebugName, static_cast<uint32>(result));
			Shutdown();
		}
		render->NameVulkanObject(m_DebugName, ME_VK_TO_UINT_HANDLE(m_Buffer), VK_OBJECT_TYPE_BUFFER);

		if (m_Specification.MemoryType == MemoryType::RAM) return;

		result = CreateStagingBuffer();
		if (ME_VK_FAILED(result))
		{
			ME_ASSERT(false, ME_VK_LOG_OUTPUT_FORMAT("Uniform", "Failed to create staging buffer! Error code: {1}"),
				m_DebugName, static_cast<uint32>(result));
			Shutdown();
		}
		render->NameVulkanObject(m_DebugName + TEXT(" Staging"), ME_VK_TO_UINT_HANDLE(m_StagingBuffer), VK_OBJECT_TYPE_BUFFER);
	}

	VkResult VulkanUniform::CreateBuffer()
	{
		VkBufferCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		createInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | (m_Specification.MemoryType == MemoryType::RAM ? 0 : VK_BUFFER_USAGE_TRANSFER_DST_BIT);
		createInfo.size = static_cast<uint64>(m_Specification.Size);
		createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0;
		createInfo.pQueueFamilyIndices = nullptr;
		
		VmaAllocationCreateInfo allocCreateInfo = {};
		allocCreateInfo.flags = m_Specification.MemoryType == MemoryType::RAM ? VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT : 0;
		allocCreateInfo.usage = m_Specification.MemoryType == MemoryType::RAM ? VMA_MEMORY_USAGE_AUTO_PREFER_HOST : VMA_MEMORY_USAGE_GPU_ONLY;

		VmaAllocationInfo allocInfo = {};

		VkResult result = vmaCreateBuffer(RenderCommand::Get()->As<VulkanRenderAPI>()->GetAllocator(), &createInfo, &allocCreateInfo, &m_Buffer, &m_Allocation, &allocInfo);
		m_MappedData = allocInfo.pMappedData;
		return result;
	}

	VkResult VulkanUniform::CreateStagingBuffer()
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

		return vmaCreateBuffer(RenderCommand::Get()->As<VulkanRenderAPI>()->GetAllocator(), &createInfo, &allocCreateInfo, &m_StagingBuffer, &m_StagingAllocation, nullptr);
	}

    void VulkanUniform::ChooseOptimalDstStage()
    {
	    m_OptimalDstPipelineStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		if (static_cast<uint32>(m_Specification.ResourceBinding.Stage & ShaderStage::Geometry) != 0)
			m_OptimalDstPipelineStage = VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT;
		if (static_cast<uint32>(m_Specification.ResourceBinding.Stage & ShaderStage::Domain) != 0)
			m_OptimalDstPipelineStage = VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT;
		if (static_cast<uint32>(m_Specification.ResourceBinding.Stage & ShaderStage::Hull) != 0)
			m_OptimalDstPipelineStage = VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT;
		if (static_cast<uint32>(m_Specification.ResourceBinding.Stage & ShaderStage::Vertex) != 0)
			m_OptimalDstPipelineStage = VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;
    }

    void VulkanUniform::ChooseOptimalSrcStage()
    {
	    m_OptimalSrcPipelineStage = VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;
		if (static_cast<uint32>(m_Specification.ResourceBinding.Stage & ShaderStage::Hull) != 0)
			m_OptimalSrcPipelineStage = VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT;
		if (static_cast<uint32>(m_Specification.ResourceBinding.Stage & ShaderStage::Domain) != 0)
			m_OptimalSrcPipelineStage = VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT;
		if (static_cast<uint32>(m_Specification.ResourceBinding.Stage & ShaderStage::Geometry) != 0)
			m_OptimalSrcPipelineStage = VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT;
		if (static_cast<uint32>(m_Specification.ResourceBinding.Stage & ShaderStage::Pixel) != 0)
			m_OptimalSrcPipelineStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }

    void VulkanUniform::SetDataVRAM(const ME::Core::Memory::Reference<CommandBuffer>& commandBuffer, const void* data, SIZE_T size, SIZE_T offset) const
    {
		void* bufferData;
		VulkanRenderAPI* render = RenderCommand::Get()->As<VulkanRenderAPI>();
		VkBufferCopy bufferCopy;

		VkResult result = vmaMapMemory(render->GetAllocator(), m_StagingAllocation, &bufferData);
		if (ME_VK_FAILED(result))
			ME_ASSERT(false, ME_VK_LOG_OUTPUT_FORMAT("Uniform", "Failed to map buffer while updating data! Error code: {1}"),
				m_DebugName, static_cast<uint32>(result));

		memcpy(static_cast<char8*>(bufferData) + offset, data, size);

		vmaUnmapMemory(render->GetAllocator(), m_StagingAllocation);

		result = vmaFlushAllocation(render->GetAllocator(), m_StagingAllocation, offset, size);
		if (ME_VK_FAILED(result))
			ME_ASSERT(false, ME_VK_LOG_OUTPUT_FORMAT("Uniform", "Failed to flush allocation while updating data! Error code: {1}"),
				m_DebugName, static_cast<uint32>(result));

        bufferCopy = {offset, offset, size };

        vkCmdCopyBuffer(commandBuffer->As<VulkanCommandBuffer>()->GetCommandBuffer(), m_StagingBuffer, m_Buffer, 1, &bufferCopy);

		VkBufferMemoryBarrier barrier = {};
		barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_UNIFORM_READ_BIT;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.buffer = m_Buffer;
		barrier.offset = 0;
		barrier.size = m_Specification.Size;

		vkCmdPipelineBarrier(commandBuffer->As<VulkanCommandBuffer>()->GetCommandBuffer(), 
			VK_PIPELINE_STAGE_TRANSFER_BIT, m_OptimalDstPipelineStage, 
			0, 
			0, nullptr, 
			1, &barrier,
			0, nullptr);
	}

	void VulkanUniform::SetDataRAM(const ME::Core::Memory::Reference<CommandBuffer>& commandBuffer, const void* data, SIZE_T size, SIZE_T offset) const
    {
		memcpy(static_cast<char8*>(m_MappedData) + offset, data, size);

		VkBufferMemoryBarrier barrier = {};
		barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
		barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_UNIFORM_READ_BIT;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.buffer = m_Buffer;
		barrier.offset = 0;
		barrier.size = m_Specification.Size;

		vkCmdPipelineBarrier(commandBuffer->As<VulkanCommandBuffer>()->GetCommandBuffer(), 
			VK_PIPELINE_STAGE_HOST_BIT, m_OptimalDstPipelineStage, 
			0, 
			0, nullptr, 
			1, &barrier,
			0, nullptr);
	}
}
