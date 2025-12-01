#include "VulkanStorageBuffer.hpp"
#include "VulkanRenderAPI.hpp"
#include "VulkanCommandBuffer.hpp"
#include "Renderer/RenderCommand.hpp"
#include "Renderer/RenderResourcesTracker.hpp"
#include "VulkanResourceHandler.hpp"

namespace ME::Render
{
    ME::Core::Memory::Reference<StorageBuffer> StorageBuffer::CreateVulkan(
        const StorageBufferSpecification& specification)
    {
        auto object = ME::Core::Memory::MakeReference<VulkanStorageBuffer>(specification);
        RenderResourcesTracker::Get().AddItem(object);
        return object;
    }

    VulkanStorageBuffer::VulkanStorageBuffer(const StorageBufferSpecification& specification)
        : StorageBuffer(specification),
        m_Buffer(nullptr), m_Allocation(nullptr),
        m_StagingBuffer(nullptr), m_StagingAllocation(nullptr),
        m_MappedData(nullptr),
        m_ResourceIndex(m_Specification.SetIndex)
    {
        Init();
    }

    VulkanStorageBuffer::~VulkanStorageBuffer()
    {
        Shutdown();
    }

    void VulkanStorageBuffer::Shutdown()
    {
        if (m_StagingBuffer != nullptr)
        {
            vmaDestroyBuffer(RenderCommand::Get()->As<VulkanRenderAPI>()->GetAllocator(), m_StagingBuffer, m_StagingAllocation);
            m_StagingBuffer = nullptr;
            m_StagingAllocation = nullptr;
        }

        if (m_Buffer != nullptr)
        {
            vmaDestroyBuffer(RenderCommand::Get()->As<VulkanRenderAPI>()->GetAllocator(), m_Buffer, m_Allocation);
            m_Buffer = nullptr;
            m_Allocation = nullptr;
        }
    }

    void VulkanStorageBuffer::SetData(void* data, SIZE_T size, SIZE_T offset)
    {
        ME_ASSERT(size + offset <= m_Specification.Size, "Trying to set data with different size in storage buffer \"{0}\"!", m_DebugName);
        ME::Core::Memory::Reference<CommandBuffer> commandBuffer = RenderCommand::Get()->GetSingleUseCommandBuffer();
        SetData(commandBuffer, data, size, offset);
        RenderCommand::Get()->SubmitAndFreeSingleUseCommandBuffer(commandBuffer);
    }

    void VulkanStorageBuffer::SetData(ME::Core::Memory::Reference<CommandBuffer> commandBuffer, void* data,
                                      SIZE_T size, SIZE_T offset)
    {
        ME_ASSERT(size + offset <= m_Specification.Size, "Trying to set data with different size in storage buffer \"{0}\"!", m_DebugName);
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

    void VulkanStorageBuffer::Resize(SIZE_T size)
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
        inBarrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
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
        outBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
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

        // If required, recreate the staging buffer
        if (m_Specification.MemoryType == MemoryType::RAM) return;
        vmaDestroyBuffer(RenderCommand::Get()->As<VulkanRenderAPI>()->GetAllocator(), m_StagingBuffer, m_StagingAllocation);
        CreateStagingBuffer();
    }

    void VulkanStorageBuffer::Clear()
    {
        ME::Core::Memory::Reference<CommandBuffer> commandBuffer = RenderCommand::Get()->GetSingleUseCommandBuffer();
        Clear(commandBuffer);
        RenderCommand::Get()->SubmitAndFreeSingleUseCommandBuffer(commandBuffer);
    }

    void VulkanStorageBuffer::Clear(ME::Core::Memory::Reference<CommandBuffer> commandBuffer)
    {
        VkBufferMemoryBarrier barrier = {};
        barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
        barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
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

    void VulkanStorageBuffer::Bind(ME::Core::Memory::Reference<CommandBuffer> commandBuffer,
        ME::Core::Memory::Reference<Pipeline> pipeline)
    {
        RenderCommand::GetResourceHandler()->As<VulkanResourceHandler>()->BindResourceSet(commandBuffer, pipeline, m_Specification.Set, m_ResourceIndex);
    }

    void VulkanStorageBuffer::Write()
    {
        RenderCommand::GetResourceHandler()->As<VulkanResourceHandler>()->WriteResource(this);
    }

    void VulkanStorageBuffer::Barrier(ME::Core::Memory::Reference<CommandBuffer> commandBuffer, BarrierInfo src,
        BarrierInfo dst)
    {
        RenderCommand::GetResourceHandler()->As<VulkanResourceHandler>()->BufferBarrier(commandBuffer, m_Buffer, src, dst);
    }

    void VulkanStorageBuffer::Init()
    {
        ChooseOptimalDstStage();
        ChooseOptimalSrcStage();

        VkResult result = CreateBuffer();
        if (ME_VK_FAILED(result))
        {
            ME_ASSERT(false, "Vulkan: uniform buffer's \"{0}\" creation failed! Error: {1}", m_DebugName, static_cast<int32>(result));
            Shutdown();
        }

        if (m_Specification.MemoryType == MemoryType::RAM) return;

        result = CreateStagingBuffer();
        if (ME_VK_FAILED(result))
        {
            ME_ASSERT(false, "Vulkan: data update in uniform buffer \"{0}\" failed! Error: {1}", m_DebugName, static_cast<int32>(result));
            Shutdown();
        }
    }

    VkResult VulkanStorageBuffer::CreateBuffer()
    {
        VkBufferCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        createInfo.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | (m_Specification.MemoryType == MemoryType::RAM ? 0 : VK_BUFFER_USAGE_TRANSFER_DST_BIT);
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

    VkResult VulkanStorageBuffer::CreateStagingBuffer()
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

    void VulkanStorageBuffer::ChooseOptimalDstStage()
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
        if (static_cast<uint32>(m_Specification.ResourceBinding.Stage & ShaderStage::Mesh) != 0)
            m_OptimalDstPipelineStage = VK_PIPELINE_STAGE_MESH_SHADER_BIT_EXT;
        if (static_cast<uint32>(m_Specification.ResourceBinding.Stage & ShaderStage::Task) != 0)
            m_OptimalDstPipelineStage = VK_PIPELINE_STAGE_TASK_SHADER_BIT_EXT;
    }

    void VulkanStorageBuffer::ChooseOptimalSrcStage()
    {
        m_OptimalDstPipelineStage = VK_PIPELINE_STAGE_TASK_SHADER_BIT_EXT;
        if (static_cast<uint32>(m_Specification.ResourceBinding.Stage & ShaderStage::Mesh) != 0)
            m_OptimalDstPipelineStage = VK_PIPELINE_STAGE_MESH_SHADER_BIT_EXT;
        if (static_cast<uint32>(m_Specification.ResourceBinding.Stage & ShaderStage::Vertex) != 0)
            m_OptimalDstPipelineStage = VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;
        if (static_cast<uint32>(m_Specification.ResourceBinding.Stage & ShaderStage::Hull) != 0)
            m_OptimalSrcPipelineStage = VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT;
        if (static_cast<uint32>(m_Specification.ResourceBinding.Stage & ShaderStage::Domain) != 0)
            m_OptimalSrcPipelineStage = VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT;
        if (static_cast<uint32>(m_Specification.ResourceBinding.Stage & ShaderStage::Geometry) != 0)
            m_OptimalSrcPipelineStage = VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT;
        if (static_cast<uint32>(m_Specification.ResourceBinding.Stage & ShaderStage::Pixel) != 0)
            m_OptimalSrcPipelineStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }

    void VulkanStorageBuffer::SetDataVRAM(const ME::Core::Memory::Reference<CommandBuffer>& commandBuffer, const void* data, SIZE_T size, SIZE_T offset) const
    {
        void* bufferData;
        VkBufferCopy bufferCopy;
        VulkanRenderAPI* render = RenderCommand::Get()->As<VulkanRenderAPI>();

        VkResult result = vmaMapMemory(render->GetAllocator(), m_StagingAllocation, &bufferData);
        if (ME_VK_FAILED(result))
            ME_ASSERT(false, "Vulkan: data mapping in storage buffer \"{0}\" failed! Error: {1}", m_DebugName, static_cast<int32>(result));

        memcpy(static_cast<char8*>(bufferData) + offset, data, size);

        vmaUnmapMemory(render->GetAllocator(), m_StagingAllocation);

        result = vmaFlushAllocation(render->GetAllocator(), m_StagingAllocation, offset, size);
        if (ME_VK_FAILED(result))
            ME_ASSERT(false, "Vulkan: allocation flushing in storage buffer \"{0}\" failed! Error: {1}", m_DebugName, static_cast<int32>(result));

        bufferCopy = { offset, offset, size };

        vkCmdCopyBuffer(commandBuffer->As<VulkanCommandBuffer>()->GetCommandBuffer(), m_StagingBuffer, m_Buffer, 1, &bufferCopy);

        VkBufferMemoryBarrier barrier = {};
        barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.buffer = m_Buffer;
        barrier.offset = offset;
        barrier.size = size;

        vkCmdPipelineBarrier(commandBuffer->As<VulkanCommandBuffer>()->GetCommandBuffer(),
            VK_PIPELINE_STAGE_TRANSFER_BIT, m_OptimalDstPipelineStage,
            0, 
            0, nullptr,
            1, &barrier,
            0, nullptr);
    }
    

    void VulkanStorageBuffer::SetDataRAM(const ME::Core::Memory::Reference<CommandBuffer>& commandBuffer, const void* data, SIZE_T size, SIZE_T offset) const
    {
        memcpy(static_cast<char8*>(m_MappedData) + offset, data, size);

        VkBufferMemoryBarrier barrier = {};
        barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.buffer = m_Buffer;
        barrier.offset = offset;
        barrier.size = size;

        vkCmdPipelineBarrier(commandBuffer->As<VulkanCommandBuffer>()->GetCommandBuffer(),
            VK_PIPELINE_STAGE_TRANSFER_BIT, m_OptimalDstPipelineStage,
            0,
            0, nullptr,
            1, &barrier, 
            0, nullptr);
    }
}
