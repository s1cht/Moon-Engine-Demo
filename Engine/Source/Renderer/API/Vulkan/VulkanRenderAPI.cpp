#include "VulkanRenderAPI.h"
#include <set>

#include "Application/Application.h"
#include "EngineDefines.h"
#include "VulkanFunctions.h"
#include "VulkanCommandBuffer.h"
#include "VulkanFramebuffer.h"
#include "VulkanRenderPass.h"
#include "VulkanResourceHandler.h"
#include "Renderer/RenderResourcesTracker.hpp"
#include "Renderer/Base/Buffer.h"


namespace ME::Render
{
    inline ME::Core::Memory::Reference<RenderAPI> RenderAPI::CreateVulkan()
    {
        auto object = ME::Core::Memory::MakeReference<VulkanRenderAPI>();
        RenderResourcesTracker::Get().AddItem(object);
        return object;
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
            ME_ASSERT(TEXT("Vulkan device creation failed! Error: {0}"), result);
            Shutdown();
        }
        m_ResourceHandler = ME::Core::Memory::MakeReference<VulkanResourceHandler>(m_SwapChain->GetFrameCount());
        CreateFrameInfoObjects();

    }

    void VulkanRenderAPI::CreateFramebuffers(ME::Core::Memory::Reference<ME::Render::RenderPass> renderPass)
    {
        ME::Core::Containers::Array<ME::Core::Memory::Reference<ME::Render::Texture2D>> swapChainImages = m_SwapChain->GetImages();

        for (SIZE_T i = 0; i < swapChainImages.GetSize(); i++)
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
        uint32 currentFrame = m_SwapChain->GetFrameIndex();
        auto cmdBuffer = buffer->As<VulkanCommandBuffer>()->GetBuffer();

        VkPipelineStageFlags flags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &cmdBuffer;
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = &m_FrameInfos[currentFrame].ImageAvailable;
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &m_FrameInfos[currentFrame].RenderFinished;
        submitInfo.pWaitDstStageMask = &flags;

        vkQueueSubmit(m_GraphicsQueue, 1, &submitInfo, m_FrameInfos[currentFrame].InFlight);
    }

    void VulkanRenderAPI::NewFrame()
    {
        uint32 currentFrame = m_SwapChain->GetFrameIndex();
        vkWaitForFences(m_Device, 1, &m_FrameInfos[currentFrame].InFlight, true, UINT64_MAX);
        vkResetFences(m_Device, 1, &m_FrameInfos[currentFrame].InFlight);
        m_SwapChain->As<VulkanSwapChain>()->NewFrame(m_FrameInfos[currentFrame]);

        if (m_SwapChain->UpdateRequired())
        {
            vkDeviceWaitIdle(m_Device);
            auto renderPass = m_FrameInfos[0].Framebuffer->GetSpecification().RenderPass;

            for (SIZE_T i = 0; i < m_SwapChain->GetImages().GetSize(); i++)
            {
                m_FrameInfos[i].Framebuffer->Shutdown();
                m_FrameInfos[i].Framebuffer = nullptr;
            }

            m_SwapChain->Resize(0, 0);

            CreateFramebuffers(renderPass);
            m_SwapChain->Updated();
        }
    }

    void VulkanRenderAPI::Present()
    {
        m_SwapChain->As<VulkanSwapChain>()->Present(m_PresentQueue, m_FrameInfos[m_SwapChain->GetFrameIndex()]);
    }

    void VulkanRenderAPI::Clear(ME::Core::Math::Vector4D32 color)
    {
    }

    void VulkanRenderAPI::Draw(ME::Core::Memory::Reference<Render::CommandBuffer> buffer, uint32 vertexCount, uint32 instanceCount)
    {
        vkCmdDraw(buffer->As<VulkanCommandBuffer>()->GetBuffer(), vertexCount, instanceCount, 0, 0);
    }

    void VulkanRenderAPI::DrawIndexed(ME::Core::Memory::Reference<Render::CommandBuffer> buffer, uint32 indexCount, uint32 index)
    {
        vkCmdDrawIndexed(buffer->As<VulkanCommandBuffer>()->GetBuffer(), indexCount, 1, index, 0, 0);
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

    inline void VulkanRenderAPI::BindResourceSet(ME::Core::Memory::Reference<Render::CommandBuffer> commandBuffer,
	    ME::Core::Memory::Reference<Render::Pipeline> pipeline, ME::Core::Memory::Reference<ME::Render::Uniform> buffer)
    {
        m_ResourceHandler->BindResourceSet(commandBuffer, pipeline, buffer->GetSpecification().Set, buffer->GetResourceSet());
    }

    inline void VulkanRenderAPI::BindResourceSet(ME::Core::Memory::Reference<Render::CommandBuffer> commandBuffer,
	    ME::Core::Memory::Reference<Render::Pipeline> pipeline,
	    ME::Core::Memory::Reference<ME::Render::StorageBuffer> buffer)
    {
        m_ResourceHandler->BindResourceSet(commandBuffer, pipeline, buffer->GetSpecification().Set, buffer->GetResourceSet());
    }

    void VulkanRenderAPI::OnWindowEvent(int32 x, int32 y)
    {
        Resize(x, y);
    }

    void VulkanRenderAPI::BeginRenderPass(ME::Core::Memory::Reference<ME::Render::CommandBuffer> buffer, ME::Render::RenderPassBeginInfo& info)
    {
        ME::Core::Containers::Array<VkClearValue> clearValues;

        for (SIZE_T i = 0; i < info.ClearValues.GetSize(); i++)
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
        beginInfo.clearValueCount = static_cast<uint32>(clearValues.GetSize());
        beginInfo.pClearValues = clearValues.Data();
        beginInfo.framebuffer = info.Framebuffer->As<VulkanFramebuffer>()->GetFramebuffer();
        beginInfo.renderPass = info.RenderPass->As<VulkanRenderPass>()->GetRenderPass();

        vkCmdBeginRenderPass(buffer->As<ME::Render::VulkanCommandBuffer>()->GetBuffer(), &beginInfo, VK_SUBPASS_CONTENTS_INLINE);
    }

    void VulkanRenderAPI::EndRenderPass(ME::Core::Memory::Reference<ME::Render::CommandBuffer> buffer)
    {
        vkCmdEndRenderPass(buffer->As<VulkanCommandBuffer>()->GetBuffer());
    }

    ME::Core::Memory::Reference<ME::Render::CommandBuffer> VulkanRenderAPI::GetAvailableCommandBuffer()
    {
        uint32 currentFrame = m_SwapChain->GetFrameIndex();
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
        VkCommandBuffer buf = buffer->As<VulkanCommandBuffer>()->GetBuffer();

        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &buf;

        VkFence fence = nullptr;
        VkFenceCreateInfo fenceCreateInfo = {};
        fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

        vkCreateFence(m_Device, &fenceCreateInfo, nullptr, &fence);

		vkQueueSubmit(m_GraphicsQueue, 1, &submitInfo, fence);

    	vkWaitForFences(m_Device, 1, &fence, false, UINT64_MAX);
    	buffer->Shutdown();
        vkDestroyFence(m_Device, fence, nullptr);
    }

    ME::Core::Memory::Reference<ME::Render::Framebuffer> VulkanRenderAPI::GetAvailableFramebuffer() const
    {
        uint32 currentFrame = m_SwapChain->GetFrameIndex();
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
        };

        bool supported = CheckLayerSupport(unsupported);
        if (supported != true)
        {
            ME_ASSERT(false, TEXT("Vulkan: layers unsupported! Layer: {0}"), unsupported);
            return ME_VK_RETURN_V(VulkanErrors::LayersUnsupported);
        }

        supported = CheckExtensionSupport(unsupported);
        if (supported != true)
        {
            ME_ASSERT(false, TEXT("Vulkan: extensions unsupported! Extension: {0}"), unsupported);
            return ME_VK_RETURN_V(VulkanErrors::ExtensionUnsupported);
        }

        int32 result = CreateInstance(apiVersion);
        if (ME_VK_FAILED(result))
        {
            ME_ASSERT(false, TEXT("Vulkan instance creation failed! Error: {0}"), result);
            Shutdown();
            return ME_VK_RETURN_V(result);
        }

#if defined(ME_DEBUG) || defined(ME_RELEASE)
        result = CreateValidationLayer();
        if (ME_VK_FAILED(result))
        {
            ME_ASSERT(false, TEXT("Vulkan validation layer creation failed! Error: {0}"), result);
            Shutdown();
            return ME_VK_RETURN_V(result);
        }
#endif

        result = CreateWindowSurface();
        if (ME_VK_FAILED(result))
        {
            ME_ASSERT(false, TEXT("Vulkan window surface creation failed! Error: {0}"), result);
            Shutdown();
            return ME_VK_RETURN_V(result);
        }

        UpdatePhysicalDevices();
        CheckPhysicalDevices();

        if (m_PhysicalDevices.GetSize() <= 0)
        {
            ME_ASSERT(false, TEXT("Incompatible physical devices!"));
            Shutdown();
            return ME_VK_RETURN_V(VulkanErrors::IncompatiblePhysicalDevices);
        }

        PickBestPhysicalDevice();

        supported = CheckDeviceLayerSupport(unsupported);
        if (supported != true)
        {
            ME_ASSERT(false, TEXT("Vulkan: physical device doesn't support layer: {0}!"), unsupported);
            return ME_VK_RETURN_V(VulkanErrors::DeviceLayersUnsupported);
        }

        supported = CheckDeviceExtensionSupport(unsupported);
        if (supported != true)
        {
            ME_ASSERT(false, TEXT("Vulkan: physical device doesn't support extension: {0}"), unsupported);
            return ME_VK_RETURN_V(VulkanErrors::DeviceExtensionUnsupported);
        }

        result = PickQueueFamily();
        if (ME_VK_FAILED(result))
        {
            ME_ASSERT(false, TEXT("Vulkan queue family pick failed!"));
            Shutdown();
            return result;
        }

        result = CreateDevice();
        if (ME_VK_FAILED(result))
        {
            ME_ASSERT(TEXT("Vulkan device creation failed! Error: {0}"), result);
            Shutdown();
            return result;
        }

        result = CreateCommandPool();
        if (ME_VK_FAILED(result))
        {
            ME_ASSERT(TEXT("Vulkan: command pool creation failed! Error: {0}"), result);
            Shutdown();
            return result;
        }

        result = CreateAllocator(apiVersion);
        if (ME_VK_FAILED(result))
        {
            ME_ASSERT(TEXT("Vulkan: VMA creation failed! Error: {0}"), result);
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
        appInfo.pApplicationName = ME::Core::Containers::StringToAnsiString(appData.ApplicationName).GetString();
        appInfo.pEngineName = ENGINE_NAME;
        appInfo.applicationVersion = VK_MAKE_API_VERSION(0, appData.VersionMajor, appData.VersionMinor, appData.VersionPatch);
        appInfo.engineVersion = VK_MAKE_API_VERSION(0, ENGINE_VER_MAJOR, ENGINE_VER_MINOR, ENGINE_VER_PATCH);
        appInfo.apiVersion = apiVersion;

        VkInstanceCreateInfo instanceInfo = {};
        instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceInfo.pApplicationInfo = &appInfo;
        instanceInfo.ppEnabledExtensionNames = m_Extensions.Data();
        instanceInfo.enabledExtensionCount = static_cast<uint32>(m_Extensions.GetSize());
        instanceInfo.ppEnabledLayerNames = m_Layers.Data();
        instanceInfo.enabledLayerCount = static_cast<uint32>(m_Layers.GetSize());
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

        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.queueCreateInfoCount = static_cast<uint32>(queueCreateInfos.GetSize());
        createInfo.pQueueCreateInfos = queueCreateInfos.Data();
        createInfo.enabledLayerCount = static_cast<uint32>(m_DeviceLayers.GetSize());
        createInfo.ppEnabledLayerNames = m_DeviceLayers.Data();
        createInfo.enabledExtensionCount = static_cast<uint32>(m_DeviceExtensions.GetSize());
        createInfo.ppEnabledExtensionNames = m_DeviceExtensions.Data();
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
        if (m_FrameInfos.GetSize() == 0)
            return;


        ME::Core::Containers::Array<VkFence> fences(m_FrameInfos.GetSize());
        for (SIZE_T i = 0; i < m_FrameInfos.GetSize(); i++)
            fences[i] = m_FrameInfos[i].InFlight;

        vkWaitForFences(m_Device, static_cast<uint32>(m_FrameInfos.GetSize()), fences.Data(), true, UINT64_MAX);

        for (const auto& info : m_FrameInfos)
        {
            vkDestroySemaphore(m_Device, info.RenderFinished, nullptr);
            vkDestroySemaphore(m_Device, info.ImageAvailable, nullptr);
            vkDestroyFence(m_Device, info.InFlight, nullptr);
        }

        fences.Clear();
        m_FrameInfos.Clear();
    }

    bool VulkanRenderAPI::CheckLayerSupport(uint32& unsupportedLayer) const
    {
        uint32 layerCount = 0;
        ME::Core::Containers::Array<VkLayerProperties> availableLayers;

        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        availableLayers.Reserve(layerCount);
        availableLayers.SetSize(layerCount);

        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.Data());

        unsupportedLayer = 0;
        for (const ME::Core::Containers::AnsiStringView str : m_Layers)
        {
            unsupportedLayer++;
            bool layerFound = false;

            for (const auto& aLayer : availableLayers)
            {
                if (str == aLayer.layerName)
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
        availableExtensions.SetSize(extensionCount);

        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, availableExtensions.Data());

        unsupportedExtension = 0;
        for (const ME::Core::Containers::AnsiStringView str : m_Extensions)
        {
            unsupportedExtension++;
            bool extensionFound = false;

            for (const auto& aExtension : availableExtensions)
            {
                if (str == aExtension.extensionName)
                    extensionFound = true;
            }

            if (extensionFound != true)
                return false;
        }

        return true;
    }

    bool VulkanRenderAPI::CheckDeviceLayerSupport(uint32& unsupportedLayer) const
    {
        if (m_DeviceLayers.GetSize() <= 0)
            return true;

        uint32 count = 0;
        ME::Core::Containers::Array<VkLayerProperties> availableLayers;

        vkEnumerateDeviceLayerProperties(m_PhysicalDevices[m_SelectedPhysicalDevice], &count, nullptr);

        availableLayers.Reserve(count);
        availableLayers.SetSize(count);

        vkEnumerateDeviceLayerProperties(m_PhysicalDevices[m_SelectedPhysicalDevice], &count, availableLayers.Data());

        unsupportedLayer = 0;
        for (const ME::Core::Containers::AnsiStringView str : m_Layers)
        {
            unsupportedLayer++;
            bool layerFound = false;

            for (const auto& aLayer : availableLayers)
            {
                if (str == aLayer.layerName)
                    layerFound = true;
            }

            if (layerFound != true)
                return false;
        }

        return true;
    }

    bool VulkanRenderAPI::CheckDeviceExtensionSupport(uint32& unsupportedExtension) const
    {
        if (m_DeviceLayers.GetSize() <= 0)
            return true;

        uint32 extensionCount = 0;
        ME::Core::Containers::Array<VkExtensionProperties> availableExtensions;

        vkEnumerateDeviceExtensionProperties(m_PhysicalDevices[m_SelectedPhysicalDevice], nullptr, &extensionCount, nullptr);

        availableExtensions.Reserve(extensionCount);
        availableExtensions.SetSize(extensionCount);

        vkEnumerateDeviceExtensionProperties(m_PhysicalDevices[m_SelectedPhysicalDevice], nullptr, &extensionCount, availableExtensions.Data());

        unsupportedExtension = 0;
        for (const ME::Core::Containers::AnsiStringView str : m_Extensions)
        {
            unsupportedExtension++;
            bool extensionFound = false;

            for (const auto& aExtension : availableExtensions)
            {
                if (str == aExtension.extensionName)
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
        suitableDevices.Reserve(m_PhysicalDevices.GetSize());
        for (const auto& device : m_PhysicalDevices)
            if (DeviceSuitable(device))
                suitableDevices.EmplaceBack(device);

        suitableDevices.Reserve(suitableDevices.GetSize());
        suitableDevices.SetSize(suitableDevices.GetSize());

        m_PhysicalDevices = suitableDevices;
    }

    void VulkanRenderAPI::PickBestPhysicalDevice()
    {
        SIZE_T bestDevice = -1;
        SIZE_T bestScore = 0;

        for (SIZE_T device = 0; device < m_PhysicalDevices.GetSize(); device++)
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
        props.SetSize(familyCount);

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
            ME_ASSERT(false, TEXT("Vulkan: can't find queue family!"));
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

    void VulkanRenderAPI::CreateFrameInfoObjects()
	{
        for (SIZE_T i = 0; i < m_SwapChain->GetImages().GetSize(); i++)
        {
            VulkanFrameInfo frameInfo;
            VkSemaphoreCreateInfo semaphoreCreateInfo = {};
            semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
            VkFenceCreateInfo fenceCreateInfo = {};
            fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
            fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

            vkCreateFence(m_Device, &fenceCreateInfo, nullptr, &frameInfo.InFlight);
            vkCreateSemaphore(m_Device, &semaphoreCreateInfo, nullptr, &frameInfo.ImageAvailable);
            vkCreateSemaphore(m_Device, &semaphoreCreateInfo, nullptr, &frameInfo.RenderFinished);

            frameInfo.CommandBuffer = Render::CommandBuffer::CreateCommandBuffer();

            m_FrameInfos.EmplaceBack(frameInfo);
        }
    }

    void VulkanRenderAPI::UpdatePhysicalDevices()
    {
        uint32 deviceCount = 0;
        vkEnumeratePhysicalDevices(m_Instance, &deviceCount, nullptr);

        if (deviceCount != m_PhysicalDevices.GetSize())
        {
            m_PhysicalDevices.Reserve(deviceCount);
            m_PhysicalDevices.SetSize(deviceCount);
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
