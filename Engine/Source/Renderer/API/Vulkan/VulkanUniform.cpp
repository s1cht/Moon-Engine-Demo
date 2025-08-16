#include "VulkanUniform.h"

#include "VulkanRenderAPI.h"
#include "VulkanCommandBuffer.h"
#include "VulkanPipeline.h"
#include "Renderer/RenderCommand.h"
#include "Renderer/RenderResourcesTracker.hpp"

namespace ME::Render
{
	ME::Core::Memory::Reference<ME::Render::Uniform> Uniform::CreateVulkan(const UniformSpecification& specification)
	{
		auto object = ME::Core::Memory::Reference<Render::VulkanUniform>(new VulkanUniform(specification));
		RenderResourcesTracker::Get().AddItem(object);
		return object;
	}

	VulkanUniform::VulkanUniform(const UniformSpecification& specification)
		: m_Buffer(nullptr), m_Allocation(nullptr), m_Specification(specification), m_DescriptorIndex(specification.SetIndex)
	{
		Init();
	}

	VulkanUniform::~VulkanUniform()
	{
		Shutdown();
	}

	void VulkanUniform::SetData(void* data, SIZE_T size)
	{
		ME_ASSERT(size == m_Specification.Size, TEXT("Trying to set data with different size in vertex buffer \"{0}\"!"));

		ME::Core::Memory::Reference<ME::Render::CommandBuffer> commandBuffer = RenderCommand::Get()->GetSingleUseCommandBuffer();

		switch (m_Specification.MemoryType)
		{
			case MemoryType::VRAM:
			{
				SetDataVRAM(commandBuffer, data, size);
				break;
			}
			case MemoryType::RAM:
			{
				SetDataRAM(commandBuffer, data, size);
				break;
			}
		}

		RenderCommand::Get()->SubmitAndFreeSingleUseCommandBuffer(commandBuffer);
	}

	void VulkanUniform::SetData(ME::Core::Memory::Reference<ME::Render::CommandBuffer> commandBuffer, void* data, SIZE_T size)
	{
		ME_ASSERT(size == m_Specification.Size, TEXT("Trying to set data with different size in vertex buffer \"{0}\"!"));

		switch (m_Specification.MemoryType)
		{
		case MemoryType::VRAM:
		{
			SetDataVRAM(commandBuffer, data, size);
			break;
		}
		case MemoryType::RAM:
		{
			SetDataRAM(commandBuffer, data, size);
			break;
		}
		}
	}

	void VulkanUniform::Shutdown()
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

	void VulkanUniform::Init()
	{
		VkResult result = CreateBuffer();
		if (ME_VK_FAILED(result))
		{
			ME_ASSERT(false, TEXT("Vulkan: uniform buffer's \"{0}\" creation failed! Error: {1}"), m_Specification.DebugName.GetString(), static_cast<int32>(result));
			Shutdown();
		}

		if (m_Specification.MemoryType == Render::MemoryType::RAM) return;

		result = CreateStagingBuffer();
		if (ME_VK_FAILED(result))
		{
			ME_ASSERT(false, TEXT("Vulkan: data update in uniform buffer \"{0}\" failed! Error: {1}"), m_Specification.DebugName.GetString(), static_cast<int32>(result));
			Shutdown();
		}
	}

	VkResult VulkanUniform::CreateBuffer()
	{
		VkBufferCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		createInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | (m_Specification.MemoryType == Render::MemoryType::RAM ? 0 : VK_BUFFER_USAGE_TRANSFER_DST_BIT);
		createInfo.size = static_cast<uint64>(m_Specification.Size);
		createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0;
		createInfo.pQueueFamilyIndices = nullptr;
		
		VmaAllocationCreateInfo allocCreateInfo = {};
		allocCreateInfo.flags = m_Specification.MemoryType == Render::MemoryType::RAM ? VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT : 0;
		allocCreateInfo.usage = m_Specification.MemoryType == Render::MemoryType::RAM ? VMA_MEMORY_USAGE_AUTO_PREFER_HOST : VMA_MEMORY_USAGE_GPU_ONLY;

		VmaAllocationInfo allocInfo = {};

		VkResult result = vmaCreateBuffer(Render::RenderCommand::Get()->As<VulkanRenderAPI>()->GetAllocator(), &createInfo, &allocCreateInfo, &m_Buffer, &m_Allocation, &allocInfo);
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

		return vmaCreateBuffer(Render::RenderCommand::Get()->As<VulkanRenderAPI>()->GetAllocator(), &createInfo, &allocCreateInfo, &m_StagingBuffer, &m_StagingAllocation, nullptr);
	}

	void VulkanUniform::SetDataVRAM(const ME::Core::Memory::Reference<ME::Render::CommandBuffer>& commandBuffer, const void* data,
	                                SIZE_T size)
	{
		VkResult result;
		void* bufferData;
		VkBufferCopy bufferCopy;
		VulkanRenderAPI* render = Render::RenderCommand::Get()->As<VulkanRenderAPI>();

		result = vmaMapMemory(render->GetAllocator(), m_StagingAllocation, &bufferData);
		if (ME_VK_FAILED(result))
		{
			ME_ASSERT(false, TEXT("Vulkan: data mapping in vertex buffer \"{0}\" failed! Error: {1}"), m_Specification.DebugName.GetString(), static_cast<int32>(result));
		}

		memcpy(bufferData, data, m_Specification.Size);

		vmaUnmapMemory(render->GetAllocator(), m_StagingAllocation);

		result = vmaFlushAllocation(render->GetAllocator(), m_StagingAllocation, 0, m_Specification.Size);
		if (ME_VK_FAILED(result))
		{
			ME_ASSERT(false, TEXT("Vulkan: allocation flushing in vertex buffer \"{0}\" failed! Error: {1}"), m_Specification.DebugName.GetString(), static_cast<int32>(result));
		}

		bufferCopy = { 0, 0, m_Specification.Size };

		vkCmdCopyBuffer(commandBuffer->As<VulkanCommandBuffer>()->GetBuffer(), m_StagingBuffer, m_Buffer, 1, &bufferCopy);

		VkMemoryBarrier barrier = {};
		barrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_UNIFORM_READ_BIT;

		VkPipelineStageFlagBits dstPipelineStage;
		switch (m_Specification.Stage)
		{
			case ShaderStage::Vertex: dstPipelineStage = VK_PIPELINE_STAGE_VERTEX_SHADER_BIT; break;
			case ShaderStage::Pixel: dstPipelineStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT; break;
			case ShaderStage::Compute: dstPipelineStage = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT; break;
			case ShaderStage::Geometry: dstPipelineStage = VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT; break;
			case ShaderStage::Hull: dstPipelineStage = VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT; break;
			case ShaderStage::Domain: dstPipelineStage = VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT; break;
			default: ME_ASSERT(false, TEXT("Invalid shader stage"));
		}

		vkCmdPipelineBarrier(commandBuffer->As<VulkanCommandBuffer>()->GetBuffer(), VK_PIPELINE_STAGE_TRANSFER_BIT, dstPipelineStage, 0, 1, &barrier, 0, nullptr, 0, nullptr);
	}

	void VulkanUniform::SetDataRAM(ME::Core::Memory::Reference<ME::Render::CommandBuffer> commandBuffer, void* data,
		SIZE_T size)
	{
		memcpy(m_MappedData, data, m_Specification.Size);

		VkMemoryBarrier barrier = {};
		barrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_UNIFORM_READ_BIT;

		VkPipelineStageFlagBits dstPipelineStage;
		switch (m_Specification.Stage)
		{
			case ShaderStage::Vertex: dstPipelineStage = VK_PIPELINE_STAGE_VERTEX_SHADER_BIT; break;
			case ShaderStage::Pixel: dstPipelineStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT; break;
			case ShaderStage::Compute: dstPipelineStage = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT; break;
			case ShaderStage::Geometry: dstPipelineStage = VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT; break;
			case ShaderStage::Hull: dstPipelineStage = VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT; break;
			case ShaderStage::Domain: dstPipelineStage = VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT; break;
			default: ME_ASSERT(false, TEXT("Invalid shader stage"));
		}

		vkCmdPipelineBarrier(commandBuffer->As<VulkanCommandBuffer>()->GetBuffer(), VK_PIPELINE_STAGE_TRANSFER_BIT, dstPipelineStage, 0, 1, &barrier, 0, nullptr, 0, nullptr);
	}
}
