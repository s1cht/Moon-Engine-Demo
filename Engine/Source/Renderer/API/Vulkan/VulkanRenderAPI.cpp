#include "VulkanRenderAPI.hpp"
#include <set>

#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"

#include "Application/Application.h"
#include "EngineDefines.h"
#include "VulkanFunctions.hpp"
#include "VulkanCommandBuffer.hpp"
#include "VulkanFramebuffer.hpp"
#include "VulkanIndirectBuffer.hpp"
#include "VulkanRenderPass.hpp"
#include "VulkanResourceHandler.hpp"
#include "VulkanTexture.hpp"
#include "Renderer/RenderResourcesTracker.hpp"
#include "Renderer/Base/Buffer.hpp"


namespace ME::Render
{
    inline ME::Core::Memory::Reference<RenderAPI> RenderAPI::CreateVulkan()
    {
        return ME::Core::Memory::MakeReference<VulkanRenderAPI>();
    }

    VulkanRenderAPI::VulkanRenderAPI()
        : m_Instance(nullptr),
        m_Surface(nullptr), m_Device(nullptr), m_SelectedPhysicalDevice(static_cast<uint32>(-1)),
        m_PhysicalDevices(ME::Core::Containers::Array<VkPhysicalDevice>()), m_GraphicsQueueFamily(static_cast<uint32>(-1)), m_PresentQueueFamily(static_cast<uint32>(-1)),
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

        m_SwapChain = ME::Core::Memory::Reference<VulkanSwapChain>(new VulkanSwapChain(result));
        if (ME_VK_FAILED(result))
        {
            ME_ASSERT("Vulkan device creation failed! Error: {0}", result);
            Shutdown();
        }
        m_ResourceHandler = ME::Core::Memory::MakeReference<VulkanResourceHandler>(m_SwapChain->GetFrameCount());
        CreateFrameInfoObjects();

    }

    void VulkanRenderAPI::CreateFramebuffers(ME::Core::Memory::Reference<ME::Render::RenderPass> renderPass)
    {
        ME::Core::Containers::Array<ME::Core::Memory::Reference<ME::Render::Texture2D>> swapChainImages = m_SwapChain->GetImages();

        for (SIZE_T i = 0; i < swapChainImages.Size(); i++)
        {
            Render::FramebufferSpecification framebufferSpecification = {};
            framebufferSpecification.Attachments = { swapChainImages[i] };
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

            for (SIZE_T i = 0; i < m_SwapChain->GetImages().Size(); i++)
            {
                m_FrameInfos[i].Framebuffer->Shutdown();
                m_FrameInfos[i].Framebuffer = nullptr;
            }

            m_SwapChain->Resize(0, 0);

            CreateFramebuffers(renderPass);
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
        barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
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
    }

    void VulkanRenderAPI::Present()
    {
        m_SwapChain->As<VulkanSwapChain>()->Present(m_PresentQueue, m_SubmitSemaphores[m_SwapChain->GetImageIndex()]);
    }

    void VulkanRenderAPI::Clear(ME::Core::Math::Vector4D32 color)
    {
    }

    void VulkanRenderAPI::Draw(ME::Core::Memory::Reference<Render::CommandBuffer> buffer, uint32 vertexCount, uint32 instanceCount)
    {
        vkCmdDraw(buffer->As<VulkanCommandBuffer>()->GetCommandBuffer(), vertexCount, instanceCount, 0, 0);
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
        const ME::Core::Memory::Reference<Render::IndirectBuffer>& buffer, SIZE_T offset,
        const ME::Core::Memory::Reference<Render::IndirectBuffer>& drawCount, uint32 drawCountOffset,
        uint32 maxDrawCount, uint32 stride)
    {
        VkBufferMemoryBarrier bufferBarrier = {};
        bufferBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
        bufferBarrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
        bufferBarrier.dstAccessMask = VK_ACCESS_INDIRECT_COMMAND_READ_BIT;
        bufferBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        bufferBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        bufferBarrier.buffer = buffer->As<VulkanIndirectBuffer>()->GetBuffer();
        bufferBarrier.offset = 0;
        bufferBarrier.size = VK_WHOLE_SIZE;

        VkBufferMemoryBarrier drawCountBarrier = {};
        drawCountBarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
        drawCountBarrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
        drawCountBarrier.dstAccessMask = VK_ACCESS_INDIRECT_COMMAND_READ_BIT;
        drawCountBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        drawCountBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        drawCountBarrier.buffer = drawCount->As<VulkanIndirectBuffer>()->GetBuffer();
        drawCountBarrier.offset = 0;
        drawCountBarrier.size = VK_WHOLE_SIZE;

        vkCmdPipelineBarrier(commandBuffer->As<VulkanCommandBuffer>()->GetCommandBuffer(),
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT,
            0,
            0, nullptr,
            1, &bufferBarrier,
            0, nullptr);

        vkCmdPipelineBarrier(commandBuffer->As<VulkanCommandBuffer>()->GetCommandBuffer(),
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT,
            0,
            0, nullptr,
            1, &drawCountBarrier,
            0, nullptr);

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
        if (m_ValidationLayer != nullptr)
        {
            DestroyValidationLayer();
            m_ValidationLayer = nullptr;
        }
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

    void VulkanRenderAPI::BindResourceSet(ME::Core::Memory::Reference<Render::CommandBuffer> commandBuffer,
        ME::Core::Memory::Reference<Render::Pipeline> pipeline, 
        ME::Core::Memory::Reference<ME::Render::Buffer> buffer)
    {
        m_ResourceHandler->BindResourceSet(commandBuffer, pipeline, buffer->GetBaseSpecification().Set, buffer->GetResourceSet());
    }

    void VulkanRenderAPI::BindTexture(ME::Core::Memory::Reference<Render::CommandBuffer> commandBuffer,
                                      ME::Core::Memory::Reference<Render::Pipeline> pipeline,
                                      ME::Core::Memory::Reference<ME::Render::Texture2D> texture, uint32 set)
    {
        m_ResourceHandler->BindTexture(commandBuffer, pipeline, set, texture->GetSet());
    }

    void VulkanRenderAPI::OnWindowEvent(int32 x, int32 y)
    {
        Resize(x, y);
    }

    void VulkanRenderAPI::BeginRenderPass(ME::Core::Memory::Reference<ME::Render::CommandBuffer> buffer, ME::Render::RenderPassBeginInfo& info)
    {
        ME::Core::Containers::Array<VkClearValue> clearValues;

        for (SIZE_T i = 0; i < info.ClearValues.Size(); i++)
        {
            VkClearValue clearValue = {};
            if (info.ClearValues[i].UsingDepth)
            {
                clearValue.depthStencil.depth = info.ClearValues[i].DepthClearValue.Depth;
                clearValue.depthStencil.stencil = info.ClearValues[i].DepthClearValue.Stencil;
                clearValues.EmplaceBack(clearValue);
                continue;
            }

            clearValue.color.float32[0] = info.ClearValues[i].ColorClearValue.XYZW[0];
            clearValue.color.float32[1] = info.ClearValues[i].ColorClearValue.XYZW[1];
            clearValue.color.float32[2] = info.ClearValues[i].ColorClearValue.XYZW[2];
            clearValue.color.float32[3] = info.ClearValues[i].ColorClearValue.XYZW[3];

            clearValues.EmplaceBack(clearValue);
        }

        VkRenderPassBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        beginInfo.renderArea = VkRect2D{
            {
                info.RenderArea.Offset.x,
                info.RenderArea.Offset.y
            },
            {
                info.RenderArea.Extent.x,
                info.RenderArea.Extent.y
            }
        };
        beginInfo.clearValueCount = static_cast<uint32>(clearValues.Size());
        beginInfo.pClearValues = clearValues.Data();
        beginInfo.framebuffer = info.Framebuffer->As<VulkanFramebuffer>()->GetFramebuffer();
        beginInfo.renderPass = info.RenderPass->As<VulkanRenderPass>()->GetRenderPass();

        vkCmdBeginRenderPass(buffer->As<ME::Render::VulkanCommandBuffer>()->GetCommandBuffer(), &beginInfo, VK_SUBPASS_CONTENTS_INLINE);
    }

    void VulkanRenderAPI::EndRenderPass(ME::Core::Memory::Reference<ME::Render::CommandBuffer> buffer)
    {
        vkCmdEndRenderPass(buffer->As<VulkanCommandBuffer>()->GetCommandBuffer());
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

    void VulkanRenderAPI::Resize(uint32 x, uint32 y)
    {
        vkDeviceWaitIdle(m_Device);

        auto renderPass = m_FrameInfos[0].Framebuffer ? m_FrameInfos[0].Framebuffer->GetSpecification().RenderPass : nullptr;

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
        {
            CreateFramebuffers(renderPass);
        }
    }

    int32 VulkanRenderAPI::Init()
    {
	    uint32 unsupported = 0;
        int32 apiVersion = VK_API_VERSION_1_4;

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
        };

        bool supported = CheckLayerSupport(unsupported);
        if (supported != true)
        {
            ME_ASSERT(false, "Vulkan: layers unsupported! Layer: {0}", unsupported);
            return ME_VK_RETURN_V(VulkanErrors::LayersUnsupported);
        }

        supported = CheckExtensionSupport(unsupported);
        if (supported != true)
        {
            ME_ASSERT(false, "Vulkan: extensions unsupported! Extension: {0}", unsupported);
            return ME_VK_RETURN_V(VulkanErrors::ExtensionUnsupported);
        }

        int32 result = CreateInstance(apiVersion);
        if (ME_VK_FAILED(result))
        {
            ME_ASSERT(false, "Vulkan instance creation failed! Error: {0}", result);
            Shutdown();
            return ME_VK_RETURN_V(result);
        }

#if defined(ME_DEBUG) || defined(ME_RELEASE)
        result = CreateValidationLayer();
        if (ME_VK_FAILED(result))
        {
            ME_ASSERT(false, "Vulkan validation layer creation failed! Error: {0}", result);
            Shutdown();
            return ME_VK_RETURN_V(result);
        }
#endif

        result = CreateWindowSurface();
        if (ME_VK_FAILED(result))
        {
            ME_ASSERT(false, "Vulkan window surface creation failed! Error: {0}", result);
            Shutdown();
            return ME_VK_RETURN_V(result);
        }

        UpdatePhysicalDevices();
        CheckPhysicalDevices();

        if (m_PhysicalDevices.Size() <= 0)
        {
            ME_ASSERT(false, "Incompatible physical devices!");
            Shutdown();
            return ME_VK_RETURN_V(VulkanErrors::IncompatiblePhysicalDevices);
        }

        PickBestPhysicalDevice();

        supported = CheckDeviceLayerSupport(unsupported);
        if (supported != true)
        {
            ME_ASSERT(false, "Vulkan: physical device doesn't support layer: {0}!", unsupported);
            return ME_VK_RETURN_V(VulkanErrors::DeviceLayersUnsupported);
        }

        supported = CheckDeviceExtensionSupport(unsupported);
        if (supported != true)
        {
            ME_ASSERT(false, "Vulkan: physical device doesn't support extension: {0}", unsupported);
            return ME_VK_RETURN_V(VulkanErrors::DeviceExtensionUnsupported);
        }

        result = PickQueueFamily();
        if (ME_VK_FAILED(result))
        {
            ME_ASSERT(false, "Vulkan queue family pick failed!");
            Shutdown();
            return result;
        }

        result = AcquireAllRequiredFunctions();
        if (AcquireAllRequiredFunctions())
        {
            ME_ASSERT(false, "Vulkan required function is not present! Error: {0}", result);
            Shutdown();
            return result;
        }

        result = CreateDevice();
        if (ME_VK_FAILED(result))
        {
            ME_ASSERT(false, "Vulkan device creation failed! Error: {0}", result);
            Shutdown();
            return result;
        }

        result = CreateCommandPool();
        if (ME_VK_FAILED(result))
        {
            ME_ASSERT(false, "Vulkan: command pool creation failed! Error: {0}", result);
            Shutdown();
            return result;
        }

        result = CreateAllocator(apiVersion);
        if (ME_VK_FAILED(result))
        {
            ME_ASSERT(false, "Vulkan: VMA creation failed! Error: {0}", result);
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
        ME::Core::Containers::Array<VkExtensionProperties> extensions;
        const ME::ApplicationData& appData = ME::Application::Get().GetAppData();

        VkApplicationInfo appInfo = {};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = CONVERT_TEXT(appData.ApplicationName.String());
        appInfo.pEngineName = ENGINE_NAME;
        appInfo.applicationVersion = VK_MAKE_API_VERSION(0, appData.VersionMajor, appData.VersionMinor, appData.VersionPatch);
        appInfo.engineVersion = VK_MAKE_API_VERSION(0, ENGINE_VER_MAJOR, ENGINE_VER_MINOR, ENGINE_VER_PATCH);
        appInfo.apiVersion = apiVersion;

        ME::Core::Containers::Array<const char*> layers = {};
        for (const ME::Core::StringView& layer : m_DeviceLayers)
            layers.EmplaceBack(CONVERT_TEXT(layer.String()));
        ME::Core::Containers::Array<const char*> extensionsC = {};
        for (const ME::Core::StringView& extension : m_DeviceExtensions)
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

    int32 VulkanRenderAPI::CreateDevice()
    {
        VkResult result;

        std::set<uint32> families = { m_GraphicsQueueFamily, m_PresentQueueFamily };
        ME::Core::Containers::Array<VkDeviceQueueCreateInfo> queueCreateInfos;
        queueCreateInfos.Reserve(families.size());
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
        features.sampleRateShading = true;

        VkPhysicalDeviceDescriptorIndexingFeatures descFeatures = {};
        descFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES;
        descFeatures.descriptorBindingUpdateUnusedWhilePending = true;
        descFeatures.descriptorBindingPartiallyBound = true;
        descFeatures.runtimeDescriptorArray = true;
        descFeatures.pNext = nullptr;

        ME::Core::Containers::Array<const char*> layers = {};
        for (const ME::Core::StringView& layer : m_DeviceLayers)
            layers.EmplaceBack(CONVERT_TEXT(layer.String()));
        ME::Core::Containers::Array<const char*> extensions = {};
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
        createInfo.pNext = &descFeatures;

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

    void VulkanRenderAPI::DestroyValidationLayer()
    {
        auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(
	        m_Instance, "vkDestroyDebugUtilsMessengerEXT"));
        if (func != nullptr)
            func(m_Instance, m_ValidationLayer, nullptr);
    }

    void VulkanRenderAPI::DestroySynchronizationObjects()
    {
        if (m_FrameInfos.Size() == 0)
            return;

        ME::Core::Containers::Array<VkFence> fences(m_FrameInfos.Size());
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
        ME::Core::Containers::Array<VkLayerProperties> availableLayers;

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
        ME::Core::Containers::Array<VkExtensionProperties> availableExtensions;

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
                    extensionFound = true;
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
        ME::Core::Containers::Array<VkLayerProperties> availableLayers;

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
        if (m_DeviceLayers.Size() <= 0)
            return true;

        uint32 extensionCount = 0;
        ME::Core::Containers::Array<VkExtensionProperties> availableExtensions;

        vkEnumerateDeviceExtensionProperties(m_PhysicalDevices[m_SelectedPhysicalDevice], nullptr, &extensionCount, nullptr);

        availableExtensions.Reserve(extensionCount);
        availableExtensions.Resize(extensionCount);

        vkEnumerateDeviceExtensionProperties(m_PhysicalDevices[m_SelectedPhysicalDevice], nullptr, &extensionCount, availableExtensions.Data());

        unsupportedExtension = 0;
        for (const ME::Core::StringView str : m_Extensions)
        {
            unsupportedExtension++;
            bool extensionFound = false;

            for (const auto& aExtension : availableExtensions)
            {
                if (str.String() == CONVERT_TEXT_UTF8(aExtension.extensionName))
                    extensionFound = true;
            }

            if (extensionFound != true)
                return false;
        }

        return true;
    }

    void VulkanRenderAPI::CheckPhysicalDevices()
    {
        ME::Core::Containers::Array<VkPhysicalDevice> suitableDevices;
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
        ME::Core::Containers::Array<VkQueueFamilyProperties> props;

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
            ME_ASSERT(false, "Vulkan: can't find queue family!");
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

	VKAPI_ATTR VkBool32 VKAPI_CALL VulkanRenderAPI::DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
	{
        switch (messageSeverity)
        {
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            {
                ME_RENDER_TRACE("Message type: {1} Vulkan: \n\t{0}", pCallbackData->pMessage, VulkanMessageTypeToText(messageType));
                break;
            }
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
		    {
                ME_RENDER_INFO("Message type: {1} Vulkan info: \n\t{0}", pCallbackData->pMessage, VulkanMessageTypeToText(messageType));
		    	break;
		    }
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
		    {
                ME_RENDER_WARN("Message type: {1} Vulkan warning: \n\t{0}", pCallbackData->pMessage, VulkanMessageTypeToText(messageType));
		    	break;
		    }
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
		    {
                ME_RENDER_ERROR("Message type: {1} Vulkan error: \n\t{0}", pCallbackData->pMessage, VulkanMessageTypeToText(messageType));
		    	break;
		    }
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT:
		    {
                ME_RENDER_INFO("Message type: {1} Vulkan flag: \n\t{0}", pCallbackData->pMessage, (int32)messageType);
		    	break;
		    }
            default:
		    {
		    	break;
		    }
        }

        return VK_FALSE;
	}

}
