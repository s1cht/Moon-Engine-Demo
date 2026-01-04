#include "VulkanRenderAPI.hpp"
#include <Core/Containers/Set.hpp>

#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"

#include "Application/Application.hpp"
#include "EngineDefines.hpp"
#include "VulkanFunctions.hpp"
#include "VulkanCommandBuffer.hpp"
#include "VulkanSwapChain.hpp"
#include "VulkanFramebuffer.hpp"
#include "VulkanIndexBuffer.hpp"
#include "VulkanIndirectBuffer.hpp"
#include "VulkanPipeline.hpp"
#include "VulkanRenderPass.hpp"
#include "VulkanResourceHandler.hpp"
#include "VulkanShader.hpp"
#include "VulkanStorageBuffer.hpp"
#include "VulkanTexture1D.hpp"
#include "VulkanTexture2D.hpp"
#include "VulkanTexture3D.hpp"
#include "VulkanUniform.hpp"
#include "VulkanVertexBuffer.hpp"
#include "Renderer/Base/Buffer.hpp"


#if defined(ME_DEBUG) || defined(ME_RELEASE)
    #define DEBUG_NAME_DEF  VkDebugUtilsObjectNameInfoEXT nameInfo = {};                            \
                            nameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;    \
                            nameInfo.pNext = nullptr;
    
    #define DEBUG_NAME_OBJ(type, name, ptr, resultVar)                                      \
                            nameInfo.objectType = type;                                     \
                            nameInfo.objectHandle = uint64(ptr);                            \
                            nameInfo.pObjectName = name;                                    \
                            resultVar = f_vkSetDebugUtilsObjectNameEXT(m_Device, &nameInfo);
#else
    #define DEBUG_NAME_DEF
    #define DEBUG_NAME_OBJ(type, name, ptr, resultVar)
#endif

namespace ME::Render
{
    inline ME::Core::Memory::Reference<RenderAPI> RenderAPI::CreateVulkan()
    {
        return ME::Core::Memory::MakeReference<VulkanRenderAPI>();
    }

    VulkanRenderAPI::VulkanRenderAPI()
        : m_Instance(nullptr),
        m_Surface(nullptr), m_Device(nullptr), m_SelectedPhysicalDevice(static_cast<uint32>(-1)),
        m_PhysicalDevices(ME::Core::Array<VkPhysicalDevice>()), m_GraphicsQueueFamily(static_cast<uint32>(-1)), m_PresentQueueFamily(static_cast<uint32>(-1)),
        m_GraphicsQueue(nullptr), m_PresentQueue(nullptr), m_SwapChain(nullptr),
        m_ValidationLayer(nullptr)
    {
        Init();
    }

    VulkanRenderAPI::~VulkanRenderAPI()
    {
        Shutdown();
    }

    void VulkanRenderAPI::PostInit()
    {
        int32 result = 0;

        m_SwapChain = SwapChain::Create(result);
        if (ME_VK_FAILED(result))
        {
            ME_ASSERT("Vulkan device creation failed! Error: {0}", result);
            Shutdown();
        }
        m_ResourceHandler = ME::Core::Memory::MakeReference<VulkanResourceHandler>(m_SwapChain->GetFrameCount());
        CreateFrameInfoObjects();
    }

    void VulkanRenderAPI::CreateFramebuffers(ME::Core::Memory::Reference<ME::Render::RenderPass> renderPass,
        const ME::Core::Array<ME::Core::Memory::Reference<Render::RTexture2D>>& attachments)
    {
        ME::Core::Array<ME::Core::Memory::Reference<ME::Render::Texture2D>> swapChainImages = m_SwapChain->GetImages();
        m_FramebufferAttachments = attachments;
        for (SIZE_T i = 0; i < swapChainImages.Size(); i++)
        {
            Render::FramebufferSpecification framebufferSpecification = {};
            framebufferSpecification.Attachments = { swapChainImages[i] };
            for (auto& attachment : m_FramebufferAttachments)
                framebufferSpecification.Attachments.EmplaceBack(attachment->GetTextures()[i]);
            framebufferSpecification.Layers = 1;
            framebufferSpecification.RenderPass = renderPass;
            framebufferSpecification.Resolution.x = swapChainImages[i]->GetResolution().x;
            framebufferSpecification.Resolution.y = swapChainImages[i]->GetResolution().y;

            m_FrameInfos[i].Framebuffer = ME::Render::Framebuffer::Create(framebufferSpecification);
        }
    }

    void VulkanRenderAPI::Submit(ME::Core::Memory::Reference<Render::CommandBuffer> buffer)
    {
        uint32_t imageIndex = m_SwapChain->GetImageIndex();
        uint32_t frameIndex = m_SwapChain->GetFrameIndex();
        auto cmdBuffer = buffer->As<VulkanCommandBuffer>()->GetCommandBuffer();

        VkPipelineStageFlags flags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &cmdBuffer;
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = &m_FrameInfos[frameIndex].ImageAvailable;
        submitInfo.pWaitDstStageMask = &flags;
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &m_SubmitSemaphores[imageIndex];

        vkQueueSubmit(m_GraphicsQueue, 1, &submitInfo, m_FrameInfos[frameIndex].InFlight);
    }

    void VulkanRenderAPI::NewFrame()
    {
        uint32 frameIndex = m_SwapChain->GetFrameIndex();
        vkWaitForFences(m_Device, 1, &m_FrameInfos[frameIndex].InFlight, true, UINT64_MAX);
        m_SwapChain->As<VulkanSwapChain>()->NewFrame(m_FrameInfos[frameIndex].ImageAvailable);
        vkResetFences(m_Device, 1, &m_FrameInfos[frameIndex].InFlight);

        if (m_SwapChain->UpdateRequired())
        {
            vkDeviceWaitIdle(m_Device);
            auto renderPass = m_FrameInfos[0].Framebuffer->GetSpecification().RenderPass;
            auto attachments = m_FrameInfos[0].Framebuffer->GetSpecification().Attachments;
            attachments.Erase(--attachments.End());

            for (SIZE_T i = 0; i < m_SwapChain->GetImages().Size(); i++)
            {
                m_FrameInfos[i].Framebuffer->Shutdown();
                m_FrameInfos[i].Framebuffer = nullptr;
            }

            m_SwapChain->Resize(0, 0);

            CreateFramebuffers(renderPass, m_FramebufferAttachments);
            m_SwapChain->Updated();
        }

        ME::Core::Memory::Reference<ME::Render::CommandBuffer> buffer = m_FrameInfos[frameIndex].CommandBuffer;

        buffer->Reset();
        buffer->Record();

        VkImageMemoryBarrier barrier = {};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        barrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        barrier.image = m_SwapChain->GetImages()[frameIndex]->As<VulkanTexture2D>()->GetImage();
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        vkCmdPipelineBarrier(m_FrameInfos[frameIndex].CommandBuffer->As<VulkanCommandBuffer>()->GetCommandBuffer(),
            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 
            0,
            0, nullptr, 
            0, nullptr, 
            1, &barrier);
    }

    void VulkanRenderAPI::EndFrame()
    {
        uint32 currentImage = m_SwapChain->GetFrameIndex();

        VkImageMemoryBarrier barrier = {};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        barrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        barrier.image = m_SwapChain->GetImages()[currentImage]->As<VulkanTexture2D>()->GetImage();
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;
        barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        barrier.dstAccessMask = 0;

        vkCmdPipelineBarrier(m_FrameInfos[currentImage].CommandBuffer->As<VulkanCommandBuffer>()->GetCommandBuffer(),
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
            0,
            0, nullptr,
            0, nullptr,
            1, &barrier);

        m_FrameInfos[currentImage].CommandBuffer->Finish();
        Submit(m_FrameInfos[currentImage].CommandBuffer);
    }

    void VulkanRenderAPI::Present()
    {
        m_SwapChain->As<VulkanSwapChain>()->Present(m_PresentQueue, m_SubmitSemaphores[m_SwapChain->GetImageIndex()]);
    }

    void VulkanRenderAPI::Clear(ME::Core::Math::Vector4D32 color)
    {
    }

    void VulkanRenderAPI::Draw(ME::Core::Memory::Reference<Render::CommandBuffer> buffer, uint32 vertexCount, uint32 instanceCount, uint32 firstVertex, uint32 firstInstance)
    {
        vkCmdDraw(buffer->As<VulkanCommandBuffer>()->GetCommandBuffer(), vertexCount, instanceCount, firstVertex, firstInstance);
    }

    void VulkanRenderAPI::DrawIndexed(ME::Core::Memory::Reference<Render::CommandBuffer> buffer, uint32 indexCount, uint32 index)
    {
        vkCmdDrawIndexed(buffer->As<VulkanCommandBuffer>()->GetCommandBuffer(), indexCount, 1, index, 0, 0);
    }

    void VulkanRenderAPI::DrawIndexedIndirect(ME::Core::Memory::Reference<Render::CommandBuffer> commandBuffer,
        const ME::Core::Memory::Reference<Render::IndirectBuffer>& buffer, SIZE_T offset, uint32 drawCount,
        uint32 stride)
    {
        VkBufferMemoryBarrier barrier = {};
        barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
        barrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_INDIRECT_COMMAND_READ_BIT;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.buffer = buffer->As<VulkanIndirectBuffer>()->GetBuffer();
        barrier.offset = 0;
        barrier.size = VK_WHOLE_SIZE;

        vkCmdPipelineBarrier(commandBuffer->As<VulkanCommandBuffer>()->GetCommandBuffer(),
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT, 0, 0,
            nullptr, 1, &barrier, 0, nullptr);

        vkCmdDrawIndexedIndirect(commandBuffer->As<VulkanCommandBuffer>()->GetCommandBuffer(), buffer->As<VulkanIndirectBuffer>()->GetBuffer(), offset, drawCount, stride);
    }

    void VulkanRenderAPI::DrawIndexedIndirectCount(ME::Core::Memory::Reference<Render::CommandBuffer> commandBuffer,
        PipelineStageFlags bufferSrc,
        const ME::Core::Memory::Reference<Render::IndirectBuffer>& buffer, SIZE_T offset,
        const ME::Core::Memory::Reference<Render::IndirectBuffer>& drawCount, uint32 drawCountOffset,
        uint32 maxDrawCount, uint32 stride)
    {
        vkCmdDrawIndexedIndirectCount(commandBuffer->As<VulkanCommandBuffer>()->GetCommandBuffer(),
            buffer->As<VulkanIndirectBuffer>()->GetBuffer(), offset,
            drawCount->As<VulkanIndirectBuffer>()->GetBuffer(), drawCountOffset,
            maxDrawCount, stride);
    }

    void VulkanRenderAPI::DispatchMesh(ME::Core::Memory::Reference<Render::CommandBuffer> commandBuffer,
        uint32 groupCountX, uint32 groupCountY, uint32 groupCountZ)
    {
        f_vkCmdDrawMeshTasksEXT(commandBuffer->As<VulkanCommandBuffer>()->GetCommandBuffer(), groupCountX, groupCountY, groupCountZ);
    }

    void VulkanRenderAPI::Dispatch(ME::Core::Memory::Reference<Render::CommandBuffer> buffer, uint32 groupCountX, uint32 groupCountY, uint32 groupCountZ)
    {
        vkCmdDispatch(buffer->As<VulkanCommandBuffer>()->GetCommandBuffer(), groupCountX, groupCountY, groupCountZ);
    }

    void VulkanRenderAPI::Shutdown()
    {
        DestroySynchronizationObjects();

        if (m_ResourceHandler != nullptr)
        {
            m_ResourceHandler->Shutdown();
            m_ResourceHandler = nullptr;
        }
        if (m_SwapChain != nullptr)
        {
            m_SwapChain->Shutdown();
            m_SwapChain = nullptr;
        }
        if (m_GraphicsCommandPool != nullptr)
        {
            vkDestroyCommandPool(m_Device, m_GraphicsCommandPool, nullptr);
            m_GraphicsCommandPool = nullptr;
        }
        if (m_PresentQueue != nullptr)
        {
            m_PresentQueue = nullptr;
        }
        if (m_GraphicsQueue != nullptr)
        {
            m_GraphicsQueue = nullptr;
        }
        if (m_Allocator != nullptr)
        {
            vmaDestroyAllocator(m_Allocator);
            m_Allocator = nullptr;
        }
        if (m_Device != nullptr)
        {
            vkDestroyDevice(m_Device, nullptr);
            m_Device = nullptr;
        }
        if (m_Surface)
        {
            vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
            m_Surface = nullptr;
        }
#if defined(ME_DEBUG) || defined(ME_RELEASE)
        if (m_ValidationLayer != nullptr)
        {
            DestroyValidationLayer();
            m_ValidationLayer = nullptr;
        }
#endif
        if (m_Instance != nullptr)
        {
            vkDestroyInstance(m_Instance, nullptr);
            m_Instance = nullptr;
        }
    }

    inline void VulkanRenderAPI::WriteResource(ME::Core::Memory::Reference<ME::Render::Uniform> buffer)
    {
        m_ResourceHandler->WriteResource(buffer);
    }

    inline void VulkanRenderAPI::WriteResource(ME::Core::Memory::Reference<ME::Render::StorageBuffer> buffer)
    {
        m_ResourceHandler->WriteResource(buffer);
    }

    inline void VulkanRenderAPI::WriteResource(ME::Core::Memory::Reference<ME::Render::VertexBuffer> buffer)
    {
        m_ResourceHandler->WriteResource(buffer);
    }

    inline void VulkanRenderAPI::WriteResource(ME::Core::Memory::Reference<ME::Render::IndexBuffer> buffer)
    {
        m_ResourceHandler->WriteResource(buffer);
    }

    inline void VulkanRenderAPI::WriteResource(ME::Core::Memory::Reference<ME::Render::IndirectBuffer> buffer)
    {
        m_ResourceHandler->WriteResource(buffer);
    }

    void VulkanRenderAPI::BindResourceSet(ME::Core::Memory::Reference<Render::CommandBuffer> commandBuffer,
        ME::Core::Memory::Reference<Render::Pipeline> pipeline, 
        ME::Core::Memory::Reference<ME::Render::Buffer> buffer)
    {
        m_ResourceHandler->BindResourceSet(commandBuffer, pipeline, buffer->GetBaseSpecification().Set, buffer->GetResourceSet());
    }

    void VulkanRenderAPI::BindIndexBuffer(ME::Core::Memory::Reference<Render::CommandBuffer> commandBuffer,
        const ME::Core::Memory::Reference<Render::IndexBuffer>& buffer, uint32 offset)
    {
        vkCmdBindIndexBuffer(commandBuffer->As<VulkanCommandBuffer>()->GetCommandBuffer(), buffer->As<VulkanIndexBuffer>()->GetBuffer(), offset, VK_INDEX_TYPE_UINT32);
    }

    void VulkanRenderAPI::BindVertexBuffer(ME::Core::Memory::Reference<Render::CommandBuffer> commandBuffer,
        const ME::Core::Memory::Reference<Render::VertexBuffer>& buffer, uint32 offset)
    {
        VkDeviceSize vkOffset = offset;
        VkBuffer vkBuffer = buffer->As<VulkanVertexBuffer>()->GetBuffer();
        vkCmdBindVertexBuffers(commandBuffer->As<VulkanCommandBuffer>()->GetCommandBuffer(), 0, 1, &vkBuffer, &vkOffset);
    }

    void VulkanRenderAPI::BufferBarrier(ME::Core::Memory::Reference<Render::CommandBuffer> commandBuffer,
        const ME::Core::Memory::Reference<Render::VertexBuffer>& buffer, BarrierInfo src, BarrierInfo dst)
    {
        m_ResourceHandler->BufferBarrier(commandBuffer, buffer, src, dst);
    }

    void VulkanRenderAPI::BufferBarrier(ME::Core::Memory::Reference<Render::CommandBuffer> commandBuffer,
        const ME::Core::Memory::Reference<Render::IndexBuffer>& buffer, BarrierInfo src, BarrierInfo dst)
    {
        m_ResourceHandler->BufferBarrier(commandBuffer, buffer, src, dst);
    }

    void VulkanRenderAPI::BufferBarrier(ME::Core::Memory::Reference<Render::CommandBuffer> commandBuffer,
        const ME::Core::Memory::Reference<Render::IndirectBuffer>& buffer, BarrierInfo src, BarrierInfo dst)
    {
        m_ResourceHandler->BufferBarrier(commandBuffer, buffer, src, dst);
    }

    void VulkanRenderAPI::BufferBarrier(ME::Core::Memory::Reference<Render::CommandBuffer> commandBuffer,
        const ME::Core::Memory::Reference<Render::StorageBuffer>& buffer, BarrierInfo src, BarrierInfo dst)
    {
        m_ResourceHandler->BufferBarrier(commandBuffer, buffer, src, dst);
    }

    void VulkanRenderAPI::BufferBarrier(ME::Core::Memory::Reference<Render::CommandBuffer> commandBuffer,
        const ME::Core::Memory::Reference<Render::Uniform>& buffer, BarrierInfo src, BarrierInfo dst)
    {
        m_ResourceHandler->BufferBarrier(commandBuffer, buffer, src, dst);
    }

    void VulkanRenderAPI::TextureBarrier(ME::Core::Memory::Reference<Render::CommandBuffer> commandBuffer,
        const ME::Core::Memory::Reference<Render::Texture1D>& texture, BarrierInfo src, BarrierInfo dst)
    {
        m_ResourceHandler->TextureBarrier(commandBuffer, texture, src, dst);
    }

    void VulkanRenderAPI::TextureBarrier(ME::Core::Memory::Reference<Render::CommandBuffer> commandBuffer,
        const ME::Core::Memory::Reference<Render::Texture2D>& texture, BarrierInfo src, BarrierInfo dst)
    {
        m_ResourceHandler->TextureBarrier(commandBuffer, texture, src, dst);
    }

    void VulkanRenderAPI::TextureBarrier(ME::Core::Memory::Reference<Render::CommandBuffer> commandBuffer,
        const ME::Core::Memory::Reference<Render::Texture3D>& texture, BarrierInfo src, BarrierInfo dst)
    {
        m_ResourceHandler->TextureBarrier(commandBuffer, texture, src, dst);
    }

    void VulkanRenderAPI::BindTexture(ME::Core::Memory::Reference<Render::CommandBuffer> commandBuffer,
                                      ME::Core::Memory::Reference<Render::Pipeline> pipeline,
                                      ME::Core::Memory::Reference<ME::Render::Texture2D> texture, uint32 set)
    {
        m_ResourceHandler->BindResourceSet(commandBuffer, pipeline, set, texture->GetSpecification().SetIndex);
    }

    void VulkanRenderAPI::OnWindowEvent(int32 x, int32 y)
    {
        Resize(x, y);
    }

    ME::Core::Memory::Reference<ME::Render::CommandBuffer> VulkanRenderAPI::GetAvailableCommandBuffer()
    {
        return m_FrameInfos[m_SwapChain->GetFrameIndex()].CommandBuffer;
    }

    ME::Core::Memory::Reference<ME::Render::CommandBuffer> VulkanRenderAPI::GetSingleUseCommandBuffer()
    {
        ME::Core::Memory::Reference<ME::Render::CommandBuffer> cmdBuffer = Render::CommandBuffer::CreateCommandBuffer();
        cmdBuffer->Record();
        return cmdBuffer;
    }

    void VulkanRenderAPI::SubmitAndFreeSingleUseCommandBuffer(
	    ME::Core::Memory::Reference<ME::Render::CommandBuffer> buffer)
    {
        buffer->Finish();
        VkCommandBuffer buf = buffer->As<VulkanCommandBuffer>()->GetCommandBuffer();

        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &buf;

        VkFence fence = nullptr;
        VkFenceCreateInfo fenceCreateInfo = {};
        fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

        vkCreateFence(m_Device, &fenceCreateInfo, nullptr, &fence);

		vkQueueSubmit(m_GraphicsQueue, 1, &submitInfo, fence);

    	vkWaitForFences(m_Device, 1, &fence, true, UINT64_MAX);
    	buffer->Shutdown();
        vkDestroyFence(m_Device, fence, nullptr);
    }

    ME::Core::Memory::Reference<ME::Render::Framebuffer> VulkanRenderAPI::GetAvailableFramebuffer() const
    {
        return m_FrameInfos[m_SwapChain->GetFrameIndex()].Framebuffer;
    }

    void VulkanRenderAPI::YieldUntilIdle()
    {
        vkDeviceWaitIdle(m_Device);
    }

    void VulkanRenderAPI::Resize(uint32 x, uint32 y)
    {
        vkDeviceWaitIdle(m_Device);

        auto renderPass = m_FrameInfos[0].Framebuffer ? m_FrameInfos[0].Framebuffer->GetSpecification().RenderPass : nullptr;
        auto attachments = m_FrameInfos[0].Framebuffer->GetSpecification().Attachments;
        attachments.Erase(--attachments.End());

        for (auto& frameInfo : m_FrameInfos)
        {
            if (frameInfo.Framebuffer)
            {
				frameInfo.Framebuffer->Shutdown();
            	frameInfo.Framebuffer = nullptr;
            }
        }

        m_SwapChain->Resize(x, y);

        if (!m_SwapChain->UpdateRequired() && renderPass)
            CreateFramebuffers(renderPass, m_FramebufferAttachments);
    }

    int32 VulkanRenderAPI::Init()
    {
        m_DebugName = "RenderAPI";
	    uint32 unsupported = 0;
        int32 apiVersion = VK_API_VERSION_1_4;
        DEBUG_NAME_DEF;

        m_Layers =
        {
            // Layers
#if defined(ME_DEBUG) || defined(ME_RELEASE)
            "VK_LAYER_KHRONOS_validation"
#endif
        };

        m_Extensions =
        {
        #if defined(ME_DEBUG) || defined(ME_RELEASE)
            VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
        #endif
        #ifdef PLATFORM_WINDOWS
            VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
        #endif
            VK_KHR_SURFACE_EXTENSION_NAME
        };

        m_DeviceLayers = {};
        m_DeviceExtensions =
        {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
            VK_GOOGLE_HLSL_FUNCTIONALITY_1_EXTENSION_NAME,
            VK_GOOGLE_USER_TYPE_EXTENSION_NAME,
            VK_EXT_MESH_SHADER_EXTENSION_NAME,
            VK_EXT_SCALAR_BLOCK_LAYOUT_EXTENSION_NAME,
        };

        bool supported = CheckLayerSupport(unsupported);
        if (supported != true)
        {
            ME_ASSERT(false, ME_VK_LOG_OUTPUT_FORMAT("API", "Initialization failed! Layer unsupported. Layer: {1}"),
                "", unsupported);
            return ME_VK_RETURN_V(VulkanErrors::LayersUnsupported);
        }

        supported = CheckExtensionSupport(unsupported);
        if (supported != true)
        {
            ME_ASSERT(false, ME_VK_LOG_OUTPUT_FORMAT("API", "Initialization failed! Extension unsupported. Extension: {1}"),
                "", unsupported);
            return ME_VK_RETURN_V(VulkanErrors::ExtensionUnsupported);
        }

        int32 result = CreateInstance(apiVersion);
        if (ME_VK_FAILED(result))
        {
            ME_ASSERT(false, ME_VK_LOG_OUTPUT_FORMAT("API", "Initialization failed! Failed to create instance. Error code: {1}"),
                "", result);
            Shutdown();
            return ME_VK_RETURN_V(result);
        }

#if defined(ME_DEBUG) || defined(ME_RELEASE)
        result = CreateValidationLayer();
        if (ME_VK_FAILED(result))
        {
            ME_ASSERT(false, ME_VK_LOG_OUTPUT_FORMAT("API", "Initialization failed! Failed to create validation layer. Error code: {1}"),
                "", result);
            Shutdown();
            return ME_VK_RETURN_V(result);
        }
#endif

        result = CreateWindowSurface();
        if (ME_VK_FAILED(result))
        {
            ME_ASSERT(false, ME_VK_LOG_OUTPUT_FORMAT("API", "Initialization failed! Failed to create window surface. Error code: {1}"),
                "", result);
            Shutdown();
            return ME_VK_RETURN_V(result);
        }

        UpdatePhysicalDevices();
        CheckPhysicalDevices();

        if (m_PhysicalDevices.Size() <= 0)
        {
            ME_ASSERT(false, ME_VK_LOG_OUTPUT_FORMAT("API", "Initialization failed! Physical devices are incompatible."),
                "");
            Shutdown();
            return ME_VK_RETURN_V(VulkanErrors::IncompatiblePhysicalDevices);
        }

        PickBestPhysicalDevice();

        supported = CheckDeviceLayerSupport(unsupported);
        if (supported != true)
        {
            ME_ASSERT(false, ME_VK_LOG_OUTPUT_FORMAT("API", "Initialization failed! Physical device layer unsupported. Layer: {1}"),
                "", unsupported);
            return ME_VK_RETURN_V(VulkanErrors::DeviceLayersUnsupported);
        }

        supported = CheckDeviceExtensionSupport(unsupported);
        if (supported != true)
        {
            ME_ASSERT(false, ME_VK_LOG_OUTPUT_FORMAT("API", "Initialization failed! Physical device extension unsupported. Extension: {1}"),
                "", unsupported);
            return ME_VK_RETURN_V(VulkanErrors::DeviceExtensionUnsupported);
        }

        result = PickQueueFamily();
        if (ME_VK_FAILED(result))
        {
            ME_ASSERT(false, ME_VK_LOG_OUTPUT_FORMAT("API", "Initialization failed! Queue family selection failed. Error code: {1}"),
                "", unsupported);
            Shutdown();
            return result;
        }

        result = AcquireAllRequiredFunctions();
        if (AcquireAllRequiredFunctions())
        {
            ME_ASSERT(false, ME_VK_LOG_OUTPUT_FORMAT("API", "Initialization failed! Required function isn't listed. Error code: {1}"),
                "", result);
            Shutdown();
            return result;
        }

        result = CreateDevice();
        if (ME_VK_FAILED(result))
        {
            ME_ASSERT(false, ME_VK_LOG_OUTPUT_FORMAT("API", "Initialization failed! Failed to create logical device. Error code: {1}"),
                "", result);
            Shutdown();
            return result;
        }
        DEBUG_NAME_OBJ(VK_OBJECT_TYPE_INSTANCE, "Instance", m_Instance, result);
        DEBUG_NAME_OBJ(VK_OBJECT_TYPE_DEVICE, "Device", m_Device, result);

        result = CreateCommandPool();
        if (ME_VK_FAILED(result))
        {
            ME_ASSERT(false, ME_VK_LOG_OUTPUT_FORMAT("API", "Initialization failed! Failed to create command pool. Error code: {1}"),
                "", result);
            Shutdown();
            return result;
        }
        DEBUG_NAME_OBJ(VK_OBJECT_TYPE_COMMAND_POOL, "Command pool", m_GraphicsCommandPool, result);

        result = CreateAllocator(apiVersion);
        if (ME_VK_FAILED(result))
        {
            ME_ASSERT(false, ME_VK_LOG_OUTPUT_FORMAT("API", "Initialization failed! Failed to create VMA. Error code: {1}"),
                "", result);
            Shutdown();
            return result;
        }

        return result;
    }

    int32 VulkanRenderAPI::CreateAllocator(const uint32& apiVersion)
    {
        VmaVulkanFunctions functions = {};
        functions.vkGetInstanceProcAddr = vkGetInstanceProcAddr;
        functions.vkGetDeviceProcAddr = vkGetDeviceProcAddr;
        functions.vkGetPhysicalDeviceProperties = vkGetPhysicalDeviceProperties;
        functions.vkGetPhysicalDeviceMemoryProperties = vkGetPhysicalDeviceMemoryProperties;
        functions.vkAllocateMemory = vkAllocateMemory;
        functions.vkFreeMemory = vkFreeMemory;
        functions.vkMapMemory = vkMapMemory;
        functions.vkUnmapMemory = vkUnmapMemory;
        functions.vkFlushMappedMemoryRanges = vkFlushMappedMemoryRanges;
        functions.vkInvalidateMappedMemoryRanges = vkInvalidateMappedMemoryRanges;
        functions.vkBindBufferMemory = vkBindBufferMemory;
        functions.vkBindImageMemory = vkBindImageMemory;
        functions.vkGetBufferMemoryRequirements = vkGetBufferMemoryRequirements;
        functions.vkGetImageMemoryRequirements = vkGetImageMemoryRequirements;
        functions.vkCreateBuffer = vkCreateBuffer;
        functions.vkDestroyBuffer = vkDestroyBuffer;
        functions.vkCreateImage = vkCreateImage;
        functions.vkDestroyImage = vkDestroyImage;
        functions.vkCmdCopyBuffer = vkCmdCopyBuffer;
        functions.vkGetDeviceBufferMemoryRequirements = vkGetDeviceBufferMemoryRequirements;
        functions.vkGetDeviceImageMemoryRequirements = vkGetDeviceImageMemoryRequirements;

        VmaAllocatorCreateInfo allocInfo = {};
        allocInfo.vulkanApiVersion = apiVersion;
        allocInfo.device = m_Device;
        allocInfo.instance = m_Instance;
        allocInfo.physicalDevice = m_PhysicalDevices[m_SelectedPhysicalDevice];
        allocInfo.pVulkanFunctions = &functions;
        allocInfo.flags = 0;

        return ME_VK_RETURN_V(vmaCreateAllocator(&allocInfo, &m_Allocator));
    }

    int32 VulkanRenderAPI::CreateInstance(const uint32& apiVersion)
    {
        uint32 extensionCount = 0;
        ME::Core::Array<VkExtensionProperties> extensions;
        const ME::ApplicationData& appData = ME::Application::Get().GetAppData();

        VkApplicationInfo appInfo = {};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = CONVERT_TEXT(appData.ApplicationName.String());
        appInfo.pEngineName = ENGINE_NAME;
        appInfo.applicationVersion = VK_MAKE_API_VERSION(0, appData.VersionMajor, appData.VersionMinor, appData.VersionPatch);
        appInfo.engineVersion = VK_MAKE_API_VERSION(0, ENGINE_VER_MAJOR, ENGINE_VER_MINOR, ENGINE_VER_PATCH);
        appInfo.apiVersion = apiVersion;

        ME::Core::Array<const char*> layers = {};
        for (const ME::Core::StringView& layer : m_Layers)
            layers.EmplaceBack(CONVERT_TEXT(layer.String()));
        ME::Core::Array<const char*> extensionsC = {};
        for (const ME::Core::StringView& extension : m_Extensions)
            extensionsC.EmplaceBack(CONVERT_TEXT(extension.String()));

        VkInstanceCreateInfo instanceInfo = {};
        instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceInfo.pApplicationInfo = &appInfo;
        instanceInfo.ppEnabledExtensionNames = extensionsC.Data();
        instanceInfo.enabledExtensionCount = static_cast<uint32>(extensionsC.Size());
        instanceInfo.ppEnabledLayerNames = layers.Data();
        instanceInfo.enabledLayerCount = static_cast<uint32>(layers.Size());
        instanceInfo.flags = 0;

        return ME_VK_RETURN_V(vkCreateInstance(&instanceInfo, nullptr, &m_Instance));
    }

#if defined(ME_DEBUG) || defined(ME_RELEASE)

#endif

    int32 VulkanRenderAPI::CreateDevice()
    {
        VkResult result;

        ME::Core::Set<uint32> families;
        families.Insert(m_GraphicsQueueFamily);
        families.Insert(m_PresentQueueFamily);
        ME::Core::Array<VkDeviceQueueCreateInfo> queueCreateInfos;
        queueCreateInfos.Reserve(families.Size());
        float32 queuePriority = 1.f;

        for (const auto& family : families)
        {
            VkDeviceQueueCreateInfo queueCreateInfo{};

            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfo.queueFamilyIndex = family;
            queueCreateInfos.EmplaceBack(queueCreateInfo);
        }

        VkPhysicalDeviceFeatures features = {};
        features.logicOp = true;
        features.wideLines = true;
        features.fillModeNonSolid = true;
        features.depthClamp = true;
    	features.depthBiasClamp = true;
        features.depthBounds = true;
        features.sampleRateShading = true;

        VkPhysicalDeviceMeshShaderFeaturesEXT meshShaderFeatures = {};
        meshShaderFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_FEATURES_EXT;
        meshShaderFeatures.meshShader = true;
        meshShaderFeatures.taskShader = true;
        meshShaderFeatures.meshShaderQueries = true;
        meshShaderFeatures.pNext = nullptr;

        VkPhysicalDeviceVulkan14Features vk14Features = {};
        vk14Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_4_FEATURES;
        vk14Features.pNext = &meshShaderFeatures;

        VkPhysicalDeviceVulkan13Features vk13Features = {};
        vk13Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
        vk13Features.pNext = &vk14Features;

        VkPhysicalDeviceVulkan12Features vk12Features = {};
        vk12Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
        vk12Features.separateDepthStencilLayouts = true;
        vk12Features.scalarBlockLayout = true;
        vk12Features.descriptorBindingUpdateUnusedWhilePending = true;
        vk12Features.descriptorBindingPartiallyBound = true;
        vk12Features.runtimeDescriptorArray = true;
        vk12Features.drawIndirectCount = true;
        vk12Features.pNext = &vk13Features;

        VkPhysicalDeviceVulkan11Features vk11Features = {};
        vk11Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;
        vk11Features.pNext = &vk12Features;

        ME::Core::Array<const char*> layers = {};
        for (const ME::Core::StringView& layer : m_DeviceLayers)
            layers.EmplaceBack(CONVERT_TEXT(layer.String()));
        ME::Core::Array<const char*> extensions = {};
        for (const ME::Core::StringView& extension : m_DeviceExtensions)
            extensions.EmplaceBack(CONVERT_TEXT(extension.String()));

        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.queueCreateInfoCount = static_cast<uint32>(queueCreateInfos.Size());
        createInfo.pQueueCreateInfos = queueCreateInfos.Data();
        createInfo.enabledLayerCount = static_cast<uint32>(layers.Size());
        createInfo.ppEnabledLayerNames = layers.Data();
        createInfo.enabledExtensionCount = static_cast<uint32>(extensions.Size());
        createInfo.ppEnabledExtensionNames = extensions.Data();
        createInfo.pEnabledFeatures = &features;
        createInfo.pNext = &vk12Features;

        result = vkCreateDevice(m_PhysicalDevices[m_SelectedPhysicalDevice], &createInfo, nullptr, &m_Device);
        if (ME_VK_FAILED(result))
            return ME_VK_RETURN_V(result);

        vkGetDeviceQueue(m_Device, m_GraphicsQueueFamily, 0, &m_GraphicsQueue);
        vkGetDeviceQueue(m_Device, m_PresentQueueFamily, 0, &m_PresentQueue);

        return ME_VK_RETURN_V(result);
    }

    int32 VulkanRenderAPI::CreateWindowSurface()
    {
        VkResult result;
        ME::Application& application = ME::Application::Get();

#ifdef PLATFORM_WINDOWS
        ME::Win32Window* window = static_cast<ME::Win32Window*>(application.GetWindow());

        VkWin32SurfaceCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        createInfo.hwnd = window->GetWindowHandle();
        createInfo.hinstance = GetModuleHandle(nullptr);

        result = vkCreateWin32SurfaceKHR(m_Instance, &createInfo, nullptr, &  m_Surface);
#else
        return ME_VK_RETURN_V(VulkanRenderAPIErrors::UnsupportedPlatform);
#endif

        return ME_VK_RETURN_V(result);
    }

    int32 VulkanRenderAPI::CreateCommandPool()
    {
	    VkCommandPoolCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        createInfo.queueFamilyIndex = m_GraphicsQueueFamily;
        createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

        VkResult result = vkCreateCommandPool(m_Device, &createInfo, nullptr, &m_GraphicsCommandPool);

        return ME_VK_RETURN_V(result);
    }

    void VulkanRenderAPI::DestroySynchronizationObjects()
    {
        if (m_FrameInfos.Size() == 0)
            return;

        ME::Core::Array<VkFence> fences(m_FrameInfos.Size());
        for (SIZE_T i = 0; i < m_FrameInfos.Size(); i++)
            fences[i] = m_FrameInfos[i].InFlight;

        vkWaitForFences(m_Device, static_cast<uint32>(m_FrameInfos.Size()), fences.Data(), true, UINT64_MAX);

        for (const auto& info : m_FrameInfos)
        {
            vkDestroySemaphore(m_Device, info.ImageAvailable, nullptr);
            vkDestroyFence(m_Device, info.InFlight, nullptr);
        }

        for (const auto& semaphore : m_SubmitSemaphores)
            vkDestroySemaphore(m_Device, semaphore, nullptr);

        fences.Clear();
        m_FrameInfos.Clear();
    }

    bool VulkanRenderAPI::CheckLayerSupport(uint32& unsupportedLayer) const
    {
        uint32 layerCount = 0;
        ME::Core::Array<VkLayerProperties> availableLayers;

        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        availableLayers.Reserve(layerCount);
        availableLayers.Resize(layerCount);

        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.Data());

        unsupportedLayer = 0;
        for (const ME::Core::StringView str : m_Layers)
        {
            unsupportedLayer++;
            bool layerFound = false;

            for (const auto& aLayer : availableLayers)
            {
                if (str == CONVERT_TEXT_UTF8(aLayer.layerName))
                    layerFound = true;
            }

            if (layerFound != true)
                return false;
        }

        return true;
    }

    bool VulkanRenderAPI::CheckExtensionSupport(uint32& unsupportedExtension) const
    {
        uint32 extensionCount = 0;
        ME::Core::Array<VkExtensionProperties> availableExtensions;

        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

        availableExtensions.Reserve(extensionCount);
        availableExtensions.Resize(extensionCount);

        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, availableExtensions.Data());

        unsupportedExtension = 0;
        for (const ME::Core::StringView str : m_Extensions)
        {
            unsupportedExtension++;
            bool extensionFound = false;

            for (const auto& aExtension : availableExtensions)
            {
                if (str == CONVERT_TEXT_UTF8(aExtension.extensionName))
                {
                    extensionFound = true;
                    break;
                }
            }

            if (extensionFound != true)
                return false;
        }

        return true;
    }

    bool VulkanRenderAPI::CheckDeviceLayerSupport(uint32& unsupportedLayer) const
    {
        if (m_DeviceLayers.Size() <= 0)
            return true;

        uint32 count = 0;
        ME::Core::Array<VkLayerProperties> availableLayers;

        vkEnumerateDeviceLayerProperties(m_PhysicalDevices[m_SelectedPhysicalDevice], &count, nullptr);

        availableLayers.Reserve(count);
        availableLayers.Resize(count);

        vkEnumerateDeviceLayerProperties(m_PhysicalDevices[m_SelectedPhysicalDevice], &count, availableLayers.Data());

        unsupportedLayer = 0;
        for (const ME::Core::StringView str : m_Layers)
        {
            unsupportedLayer++;
            bool layerFound = false;

            for (const auto& aLayer : availableLayers)
            {
                if (str == CONVERT_TEXT_UTF8(aLayer.layerName))
                    layerFound = true;
            }

            if (layerFound != true)
                return false;
        }

        return true;
    }

    bool VulkanRenderAPI::CheckDeviceExtensionSupport(uint32& unsupportedExtension) const
    {
        if (m_DeviceExtensions.Size() <= 0)
            return true;

        uint32 extensionCount = 0;
        ME::Core::Array<VkExtensionProperties> availableExtensions;

        vkEnumerateDeviceExtensionProperties(m_PhysicalDevices[m_SelectedPhysicalDevice], nullptr, &extensionCount, nullptr);

        availableExtensions.Reserve(extensionCount);
        availableExtensions.Resize(extensionCount);

        vkEnumerateDeviceExtensionProperties(m_PhysicalDevices[m_SelectedPhysicalDevice], nullptr, &extensionCount, availableExtensions.Data());

        unsupportedExtension = 0;
        for (const ME::Core::StringView str : m_DeviceExtensions)
        {
            unsupportedExtension++;
            bool extensionFound = false;

            for (const auto& aExtension : availableExtensions)
            {
                if (str == CONVERT_TEXT_UTF8(aExtension.extensionName))
                    extensionFound = true;
            }

            if (extensionFound != true)
                return false;
        }

        return true;
    }

    void VulkanRenderAPI::CheckPhysicalDevices()
    {
        ME::Core::Array<VkPhysicalDevice> suitableDevices;
        suitableDevices.Reserve(m_PhysicalDevices.Size());
        for (const auto& device : m_PhysicalDevices)
            if (DeviceSuitable(device))
                suitableDevices.EmplaceBack(device);

        suitableDevices.Reserve(suitableDevices.Size());
        suitableDevices.Resize(suitableDevices.Size());

        m_PhysicalDevices = suitableDevices;
    }

    void VulkanRenderAPI::PickBestPhysicalDevice()
    {
        SIZE_T bestDevice = -1;
        SIZE_T bestScore = 0;

        for (SIZE_T device = 0; device < m_PhysicalDevices.Size(); device++)
        {
            uint32 score = CalculatePhysicalDevicePower(m_PhysicalDevices[device]);
            if (score >= bestScore)
            {
                bestDevice = device;
                bestScore = score;
            }
        }

        m_SelectedPhysicalDevice = static_cast<uint32>(bestDevice);
    }

    int32 VulkanRenderAPI::CalculatePhysicalDevicePower(VkPhysicalDevice device)
    {
        uint32 score = 0;
        VkPhysicalDeviceProperties deviceProperties = {};

        vkGetPhysicalDeviceProperties(device, &deviceProperties);

        if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            score += 1000;
        else if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU)
            score += 500;
        else if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
            score += 0;

        score += deviceProperties.limits.maxImageDimension2D;

        return score;
    }

    int32 VulkanRenderAPI::PickQueueFamily()
    {
        uint32 familyCount = 0;
        ME::Core::Array<VkQueueFamilyProperties> props;

        vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevices[m_SelectedPhysicalDevice], &familyCount, nullptr);

        props.Reserve(familyCount);
        props.Resize(familyCount);

        vkGetPhysicalDeviceQueueFamilyProperties(m_PhysicalDevices[m_SelectedPhysicalDevice], &familyCount, props.Data());

        for (uint32 i = 0; i < familyCount; i++)
        {
            VkBool32 supportsSChain = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(m_PhysicalDevices[m_SelectedPhysicalDevice], i, m_Surface, &supportsSChain);

            bool supportsGraphics = (props[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0;
            bool supportsCompute = (props[i].queueFlags & VK_QUEUE_COMPUTE_BIT) != 0;

            if (supportsGraphics && supportsCompute && supportsSChain)
            {
                m_GraphicsQueueFamily = i;
                m_PresentQueueFamily = i;
                break;
            }

            if (supportsGraphics && supportsCompute)
                m_GraphicsQueueFamily = i;
            if (supportsSChain)
                m_PresentQueueFamily = i;
        }

        if (m_GraphicsQueueFamily == static_cast<uint32>(-1) || m_PresentQueueFamily == static_cast<uint32>(-1))
        {
            ME_ASSERT(false, ME_VK_LOG_OUTPUT_FORMAT("API", "Requested queue family is not present!"),
                "");
            return ME_VK_RETURN_V(VulkanErrors::QueueFamilyNotFound);
        }

        return ME_VK_RETURN_V(VulkanErrors::Success);
    }

    inline bool VulkanRenderAPI::CheckPhysicalDeviceFeatures(const VkPhysicalDeviceFeatures& features)
    {
        return features.geometryShader && features.logicOp && features.wideLines && features.fillModeNonSolid && features.depthClamp && features.depthBiasClamp;
    }

    inline bool VulkanRenderAPI::CheckPhysicalDeviceProperties(const VkPhysicalDeviceProperties& properties)
    {
        return properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU
            || properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU
            || properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU;
    }

    inline bool VulkanRenderAPI::DeviceSuitable(VkPhysicalDevice device)
    {
        VkPhysicalDeviceProperties deviceProperties;
        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceProperties(device, &deviceProperties);
        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

        return CheckPhysicalDeviceProperties(deviceProperties)
            && CheckPhysicalDeviceFeatures(deviceFeatures);
    }

    int32 VulkanRenderAPI::AcquireAllRequiredFunctions()
    {
#if defined(ME_DEBUG) || defined(ME_RELEASE)
        f_vkSetDebugUtilsObjectNameEXT = reinterpret_cast<PFN_vkSetDebugUtilsObjectNameEXT>(vkGetInstanceProcAddr(
            m_Instance, "vkSetDebugUtilsObjectNameEXT"));
        if (f_vkSetDebugUtilsObjectNameEXT == nullptr)
            return ME_VK_RETURN_V(VulkanErrors::FunctionNotPresent);
#endif
        f_vkCmdDrawMeshTasksEXT = reinterpret_cast<PFN_vkCmdDrawMeshTasksEXT>(vkGetInstanceProcAddr(
            m_Instance, "vkCmdDrawMeshTasksEXT"));
        if (f_vkCmdDrawMeshTasksEXT == nullptr)
            return ME_VK_RETURN_V(VulkanErrors::FunctionNotPresent);

        return ME_VK_RETURN_V(VulkanErrors::Success);
    }

    void VulkanRenderAPI::CreateFrameInfoObjects()
	{
        for (SIZE_T i = 0; i < m_SwapChain->GetImages().Size(); i++)
        {
            VulkanImageInfo frameInfo = {};

            VkFenceCreateInfo fenceCreateInfo = {};
            fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
            fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

            VkSemaphoreCreateInfo semaphoreCreateInfo = {};
            semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

            vkCreateFence(m_Device, &fenceCreateInfo, nullptr, &frameInfo.InFlight);

            vkCreateSemaphore(m_Device, &semaphoreCreateInfo, nullptr, &frameInfo.ImageAvailable);

            frameInfo.CommandBuffer = Render::CommandBuffer::CreateCommandBuffer();
            frameInfo.Framebuffer = nullptr;

            m_FrameInfos.EmplaceBack(frameInfo);

            VkSemaphore semaphore;
            vkCreateSemaphore(m_Device, &semaphoreCreateInfo, nullptr, &semaphore);
            m_SubmitSemaphores.EmplaceBack(semaphore);
        }
    }

    void VulkanRenderAPI::CreateFramebuffers(ME::Core::Memory::Reference<ME::Render::RenderPass> renderPass)
    {
        ME::Core::Array<ME::Core::Memory::Reference<ME::Render::Texture2D>> swapChainImages = m_SwapChain->GetImages();
        for (SIZE_T i = 0; i < swapChainImages.Size(); i++)
        {
            Render::FramebufferSpecification framebufferSpecification = {};
            framebufferSpecification.Attachments = { swapChainImages[i] };
            for (auto& attachment : m_FramebufferAttachments)
                framebufferSpecification.Attachments.EmplaceBack(attachment->GetTextures()[i]);
            framebufferSpecification.Layers = 1;
            framebufferSpecification.RenderPass = renderPass;
            framebufferSpecification.Resolution.x = swapChainImages[i]->GetResolution().x;
            framebufferSpecification.Resolution.y = swapChainImages[i]->GetResolution().y;

            m_FrameInfos[i].Framebuffer = ME::Render::Framebuffer::Create(framebufferSpecification);
        }
    }

    void VulkanRenderAPI::UpdatePhysicalDevices()
    {
        uint32 deviceCount = 0;
        vkEnumeratePhysicalDevices(m_Instance, &deviceCount, nullptr);

        if (deviceCount != m_PhysicalDevices.Size())
        {
            m_PhysicalDevices.Reserve(deviceCount);
            m_PhysicalDevices.Resize(deviceCount);
            vkEnumeratePhysicalDevices(m_Instance, &deviceCount, m_PhysicalDevices.Data());
        }
    }

    Render::VulkanResourceHandler* VulkanRenderAPI::GetVulkanResourceHandler()
    {
        return m_ResourceHandler->As<VulkanResourceHandler>();
    }

    Render::VulkanSwapChain* VulkanRenderAPI::GetVulkanSwapChain()
    {
        return m_SwapChain->As<VulkanSwapChain>();
    }

    void VulkanRenderAPI::BindIndexBuffer(ME::Core::Memory::Reference<Render::CommandBuffer> commandBuffer,
        VkBuffer buffer, uint32 offset)
    {
        vkCmdBindIndexBuffer(commandBuffer->As<VulkanCommandBuffer>()->GetCommandBuffer(), buffer, offset, VK_INDEX_TYPE_UINT32);
    }

    void VulkanRenderAPI::BindVertexBuffer(ME::Core::Memory::Reference<Render::CommandBuffer> commandBuffer,
        VkBuffer buffer, uint32 offset)
    {
        VkDeviceSize vkOffset = offset;
        vkCmdBindVertexBuffers(commandBuffer->As<VulkanCommandBuffer>()->GetCommandBuffer(), 0, 1, &buffer, &vkOffset);
    }

#if defined(ME_DEBUG) || defined(ME_RELEASE)
    void VulkanRenderAPI::NameVulkanObject(ME::Core::String name, uint64 handle, VkObjectType objectType)
    {
        VkDebugUtilsObjectNameInfoEXT nameInfo = {};
        nameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
        nameInfo.pNext = nullptr;
        nameInfo.pObjectName = reinterpret_cast<const char*>(name.String());
        nameInfo.objectType = objectType;
        nameInfo.objectHandle = handle;
        f_vkSetDebugUtilsObjectNameEXT(m_Device, &nameInfo);
    }

    VkBool32 VKAPI_CALL VulkanRenderAPI::DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
	{
        switch (messageSeverity)
        {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            ME_RENDER_TRACE("[Vulkan {0}{1}] {2:<64}",
                VulkanMessageTypeToText(messageType), "",
                pCallbackData->pMessage); break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            ME_RENDER_INFO("[Vulkan {0}{1}] {2:<64}",
                VulkanMessageTypeToText(messageType), " info",
                pCallbackData->pMessage); break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            ME_RENDER_WARN("[Vulkan {0}{1}] {2:<64}",
                VulkanMessageTypeToText(messageType), " warning",
                pCallbackData->pMessage); break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            ME_RENDER_ERROR("[Vulkan {0}{1}] {2:<64}",
                    VulkanMessageTypeToText(messageType), " error", 
                    pCallbackData->pMessage); break;
            default:
		    	break;
        }

        return VK_FALSE;
	}

    ME::Core::String VulkanRenderAPI::FormatObjectString(const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData)
    {
        return "";
    }

    int32 VulkanRenderAPI::CreateValidationLayer()
    {
        auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
            vkGetInstanceProcAddr(m_Instance, "vkCreateDebugUtilsMessengerEXT"));
        if (func == nullptr)
            return VK_ERROR_EXTENSION_NOT_PRESENT;

        VkDebugUtilsMessengerCreateInfoEXT createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = DebugCallback;
        createInfo.pUserData = nullptr;

        return ME_VK_RETURN_V(func(m_Instance, &createInfo, nullptr, &m_ValidationLayer));
    }

    void VulkanRenderAPI::DestroyValidationLayer()
    {
        auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(
            m_Instance, "vkDestroyDebugUtilsMessengerEXT"));
        if (func != nullptr)
            func(m_Instance, m_ValidationLayer, nullptr);
    }
#else
    void VulkanRenderAPI::NameVulkanObject(ME::Core::String name, uint64 handle, VkObjectType objectType) {}
#endif 
}