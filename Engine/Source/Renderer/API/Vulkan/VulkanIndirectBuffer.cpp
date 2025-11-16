#include "VulkanIndirectBuffer.hpp"

#include "VulkanCommandBuffer.hpp"
#include "VulkanRenderAPI.hpp"
#include "Renderer/RenderCommand.hpp"
#include "Renderer/RenderResourcesTracker.hpp"

namespace ME::Render
{
    ME::Core::Memory::Reference<ME::Render::IndirectBuffer> IndirectBuffer::CreateVulkan(const IndirectBufferSpecification& specification)
    {
        auto object = ME::Core::Memory::Reference<Render::VulkanIndirectBuffer>(new VulkanIndirectBuffer(specification));
        RenderResourcesTracker::Get().AddItem(object);
        return object;
    }

    VulkanIndirectBuffer::VulkanIndirectBuffer(const IndirectBufferSpecification& specification)
        : IndirectBuffer(specification),
        m_Buffer(nullptr), m_Allocation(nullptr),
        m_ResourceIndex(specification.SetIndex)
    {
        Init();
    }

    VulkanIndirectBuffer::~VulkanIndirectBuffer()
    {
        Shutdown();
    }

    void VulkanIndirectBuffer::Shutdown()
    {
        if (m_Buffer != nullptr)
        {
            vmaDestroyBuffer(Render::RenderCommand::Get()->As<VulkanRenderAPI>()->GetAllocator(), m_Buffer, m_Allocation);
            m_Buffer = nullptr;
            m_Allocation = nullptr;
        }
    }

    void VulkanIndirectBuffer::Resize(SIZE_T size)
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
        inBarrier.srcAccessMask = VK_ACCESS_INDIRECT_COMMAND_READ_BIT;
        inBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        inBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        inBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        inBarrier.buffer = oldBuffer;
        inBarrier.offset = 0;
        inBarrier.size = oldSize;

        vkCmdPipelineBarrier(commandBuffer->As<VulkanCommandBuffer>()->GetCommandBuffer(),
            VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT,
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
        outBarrier.dstAccessMask = VK_ACCESS_INDIRECT_COMMAND_READ_BIT;
        outBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        outBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        outBarrier.buffer = m_Buffer;
        outBarrier.offset = 0;
        outBarrier.size = m_Specification.Size;

        vkCmdPipelineBarrier(commandBuffer->As<VulkanCommandBuffer>()->GetCommandBuffer(),
            VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT,
            0,
            0, nullptr,
            1, &outBarrier,
            0, nullptr);

        // Submit and wait for fences
        RenderCommand::Get()->SubmitAndFreeSingleUseCommandBuffer(commandBuffer);

        // Then delete old buffer
        vmaDestroyBuffer(RenderCommand::Get()->As<VulkanRenderAPI>()->GetAllocator(), oldBuffer, oldAlloc);
    }

    void VulkanIndirectBuffer::Clear()
    {
        ME::Core::Memory::Reference<CommandBuffer> commandBuffer = RenderCommand::Get()->GetSingleUseCommandBuffer();
        Clear(commandBuffer);
        RenderCommand::Get()->SubmitAndFreeSingleUseCommandBuffer(commandBuffer);
    }

    void VulkanIndirectBuffer::Clear(ME::Core::Memory::Reference<ME::Render::CommandBuffer> commandBuffer)
    {
        VkBufferMemoryBarrier barrier = {};
        barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
        barrier.srcAccessMask = VK_ACCESS_INDIRECT_COMMAND_READ_BIT;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.buffer = m_Buffer;
        barrier.offset = 0;
        barrier.size = m_Specification.Size;

        vkCmdPipelineBarrier(commandBuffer->As<VulkanCommandBuffer>()->GetCommandBuffer(), 
            VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 
            0, 
            0, nullptr, 
            1, &barrier, 
            0, nullptr);

        vkCmdFillBuffer(commandBuffer->As<VulkanCommandBuffer>()->GetCommandBuffer(), m_Buffer, 0, m_Specification.Size, 0);
    }

    void VulkanIndirectBuffer::Init()
    {
        VkResult result = CreateBuffer();
        if (ME_VK_FAILED(result))
        {
            ME_ASSERT(false, "Vulkan: indirect buffer's \"{0}\" creation failed! Error: {1}", m_DebugName, static_cast<int32>(result));
            Shutdown();
        }
    }

    VkResult VulkanIndirectBuffer::CreateBuffer()
    {
        VkBufferCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        createInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
        createInfo.size = static_cast<uint64>(m_Specification.Size);
        createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices = nullptr;

        VmaAllocationCreateInfo allocCreateInfo = {};
        allocCreateInfo.flags = 0;
        allocCreateInfo.usage = static_cast<VmaMemoryUsage>(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        return vmaCreateBuffer(Render::RenderCommand::Get()->As<VulkanRenderAPI>()->GetAllocator(), &createInfo, &allocCreateInfo, &m_Buffer, &m_Allocation, nullptr);
    }
}
