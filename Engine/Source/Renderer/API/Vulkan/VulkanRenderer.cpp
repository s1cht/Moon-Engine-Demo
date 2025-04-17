#include "VulkanRenderer.h"
#include "Application/Application.h"
#include <set>

import Pawn.Core.Container.Array;
import Pawn.Core.Container.StringView;
import Pawn.Core.Memory;

namespace Pawn::Render
{
    inline RendererAPI* RendererAPI::CreateVulkan()
    {
        return new VulkanRenderer();
    }

    VulkanRenderer::VulkanRenderer()
        : m_PhysicalDevices(Pawn::Core::Containers::Array<VkPhysicalDevice>()),
        m_GraphicsQueueFamily((uint32)-1), m_PresentQueueFamily((uint32)-1), m_SelectedPhysicalDevice((uint32)-1),
        m_Instance(nullptr), m_Device(nullptr), m_Surface(nullptr),
        m_GraphicsQueue(nullptr), m_PresentQueue(nullptr), m_ValidationLayer(nullptr),
        m_SwapChain(nullptr)
    {
        Init();
    }

    VulkanRenderer::~VulkanRenderer()
    {
        Shutdown();
    }

    void VulkanRenderer::PostInit()
    {
        int32 result = 0;

		m_SwapChain = new VulkanSwapChain(result);
		if (PE_VK_FAILED(result))
		{
			PE_ASSERT(TEXT("Vulkan device creation failed! Error: {0}"), result)
			Shutdown();
		}
    }

    void VulkanRenderer::Present()
    {
    }

    void VulkanRenderer::Clear(Pawn::Core::Math::Vector4D32 color)
    {
    }

    void VulkanRenderer::DrawIndexed(uint32 indexCount, uint32 index)
    {
    }

    void VulkanRenderer::Shutdown()
    {
        if (m_SwapChain != nullptr)
        {
            delete m_SwapChain;
            m_SwapChain = nullptr;
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

    void VulkanRenderer::OnWindowEvent(int32 x, int32 y)
    {
        m_SwapChain->Resize((uint32)x, (uint32)y);
    }

    void VulkanRenderer::BindBackBuffer()
    {
    }

    void VulkanRenderer::UnbindBackBuffer()
    {
    }

    int32 VulkanRenderer::Init()
    {
        int32 result;
        bool supported;
        uint32 unsupported = 0;

        m_Layers =
        {
            // Layers
#if defined(PE_DEBUG) || defined(PE_RELEASE)
            "VK_LAYER_KHRONOS_validation"
#endif
        };

        m_Extensions =
        {
        #if defined(PE_DEBUG) || defined(PE_RELEASE)
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
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };

        supported = CheckLayerSupport(unsupported);
        if (supported != true)
        {
            PE_ASSERT(false, TEXT("Vulkan: layers unsupported! Layer: {0}"), unsupported);
            return PE_VK_RETURN_V(VulkanErrors::LayersUnsupported);
        }

		supported = CheckExtensionSupport(unsupported);
		if (supported != true)
		{
			PE_ASSERT(false, TEXT("Vulkan: extensions unsupported! Extension: {0}"), unsupported);
            return PE_VK_RETURN_V(VulkanErrors::ExtensionUnsupported);
		}

        result = CreateInstance();
        if (PE_VK_FAILED(result))
        {
            PE_ASSERT(false, TEXT("Vulkan instance creation failed! Error: {0}"), result);
            Shutdown();
            return PE_VK_RETURN_V(result);
        }

#if defined(PE_DEBUG) || defined(PE_RELEASE)
        result = CreateValidationLayer();
        if (PE_VK_FAILED(result))
        {
            PE_ASSERT(false, TEXT("Vulkan validation layer creation failed! Error: {0}"), result)
            Shutdown();
            return PE_VK_RETURN_V(result);
        }
#endif

        result = CreateWindowSurface();
        if (PE_VK_FAILED(result))
        {
            PE_ASSERT(false, TEXT("Vulkan window surface creation failed! Error: {0}"), result);
            Shutdown();
            return PE_VK_RETURN_V(result);
        }

        UpdatePhysicalDevices();
        CheckPhysicalDevices();

        if (m_PhysicalDevices.GetSize() <= 0)
        {
            PE_ASSERT(false, TEXT("Incompatible physical devices!"));
            Shutdown();
            return PE_VK_RETURN_V(VulkanErrors::IncompatiblePhysicalDevices);
        }

        PickBestPhysicalDevice();
        
		supported = CheckDeviceLayerSupport(unsupported);
		if (supported != true)
		{
			PE_ASSERT(false, TEXT("Vulkan: physical device unsupports layer: {0}!"), unsupported);
			return PE_VK_RETURN_V(VulkanErrors::DeviceLayersUnsupported);
		}

		supported = CheckDeviceExtensionSupport(unsupported);
		if (supported != true)
		{
			PE_ASSERT(false, TEXT("Vulkan: physical device unsupports extension: {0}"), unsupported);
			return PE_VK_RETURN_V(VulkanErrors::DeviceExtensionUnsupported);
		}

        result = PickQueueFamily();
        if (PE_VK_FAILED(result))
        {
            PE_ASSERT(false, TEXT("Vulkan queue family pick failed!"));
            Shutdown();
            return result;
        }

        result = CreateDevice();
		if (PE_VK_FAILED(result))
		{
			PE_ASSERT(TEXT("Vulkan device creation failed! Error: {0}"), result)
			Shutdown();
            return result;
		}

        return result;
    }

    int32 VulkanRenderer::CreateInstance()
    {
		uint32 extensionCount = 0;
        Pawn::Core::Containers::Array<VkExtensionProperties> extensions;
        const Pawn::ApplicationData& appData = Pawn::Application::Get().GetAppData();

        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = Pawn::Core::Containers::StringToAnsiString(appData.ApplicationName).GetString();
        appInfo.pEngineName = ENGINE_NAME;
        appInfo.applicationVersion = VK_MAKE_API_VERSION(0, appData.VersionMajor, appData.VersionMinor, appData.VersionPatch);
        appInfo.engineVersion = VK_MAKE_API_VERSION(0, ENGINE_VER_MAJOR, ENGINE_VER_MINOR, ENGINE_VER_PATCH);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo instanceInfo{};
        instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceInfo.pApplicationInfo = &appInfo;
        instanceInfo.ppEnabledExtensionNames = m_Extensions.Data();
        instanceInfo.enabledExtensionCount = (uint32)m_Extensions.GetSize();
        instanceInfo.ppEnabledLayerNames = m_Layers.Data();
        instanceInfo.enabledLayerCount = (uint32)m_Layers.GetSize();
        instanceInfo.flags = 0;

        return PE_VK_RETURN_V(vkCreateInstance(&instanceInfo, nullptr, &m_Instance));
    }

    int32 VulkanRenderer::CreateValidationLayer()
    {
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_Instance, "vkCreateDebugUtilsMessengerEXT");
		if (func == nullptr) 
			return VK_ERROR_EXTENSION_NOT_PRESENT;

		VkDebugUtilsMessengerCreateInfoEXT createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo.pfnUserCallback = DebugCallback;
		createInfo.pUserData = nullptr;

        return PE_VK_RETURN_V(func(m_Instance, &createInfo, nullptr, &m_ValidationLayer));
    }

    int32 VulkanRenderer::CreateDevice()
    {
        VkResult result;

        std::set<uint32> families = { m_GraphicsQueueFamily, m_PresentQueueFamily };
        Pawn::Core::Containers::Array<VkDeviceQueueCreateInfo> queueCreateInfos;
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


        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.queueCreateInfoCount = (uint32)queueCreateInfos.GetSize();
        createInfo.pQueueCreateInfos = queueCreateInfos.Data();
        createInfo.enabledLayerCount = (uint32)m_DeviceLayers.GetSize();
        createInfo.ppEnabledLayerNames = m_DeviceLayers.Data();
        createInfo.enabledExtensionCount = (uint32)m_DeviceExtensions.GetSize();
        createInfo.ppEnabledExtensionNames = m_DeviceExtensions.Data();

        result = vkCreateDevice(m_PhysicalDevices[m_SelectedPhysicalDevice], &createInfo, nullptr, &m_Device);
        if (PE_VK_FAILED(result))
            return PE_VK_RETURN_V(result);

        vkGetDeviceQueue(m_Device, m_GraphicsQueueFamily, 0, &m_GraphicsQueue);
        vkGetDeviceQueue(m_Device, m_PresentQueueFamily, 0, &m_PresentQueue);
        return PE_VK_RETURN_V(result);
    }

    int32 VulkanRenderer::CreateWindowSurface()
    {
        VkResult result;
        Pawn::Application& application = Pawn::Application::Get();

#ifdef PLATFORM_WINDOWS
        Pawn::Win32Window* window = static_cast<Pawn::Win32Window*>(application.GetWindow());

        VkWin32SurfaceCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        createInfo.hwnd = window->GetWindowHandle();
        createInfo.hinstance = GetModuleHandle(nullptr);

        result = vkCreateWin32SurfaceKHR(m_Instance, &createInfo, nullptr, &m_Surface);
#else
        return PE_VK_RETURN_V(VulkanRendererErrors::UnsupportedPlatform);
#endif

        return PE_VK_RETURN_V(result);
    }

    void VulkanRenderer::DestroyValidationLayer()
    {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_Instance, "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr) 
            func(m_Instance, m_ValidationLayer, nullptr);
    }

    bool VulkanRenderer::CheckLayerSupport(uint32& unsupportedLayer) const
    {
		uint32 layerCount = 0;
		Pawn::Core::Containers::Array<VkLayerProperties> availableLayers;

		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        availableLayers.Reserve(layerCount);
        availableLayers.SetSize(layerCount);

		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.Data());

        unsupportedLayer = 0;
		for (const Pawn::Core::Containers::AnsiStringView str : m_Layers)
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

	bool VulkanRenderer::CheckExtensionSupport(uint32& unsupportedExtension) const
	{
		uint32 extensionCount = 0;
		Pawn::Core::Containers::Array<VkExtensionProperties> availableExtensions;

		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

		availableExtensions.Reserve(extensionCount);
		availableExtensions.SetSize(extensionCount);

        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, availableExtensions.Data());

        unsupportedExtension = 0;
		for (const Pawn::Core::Containers::AnsiStringView str : m_Extensions)
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

    bool VulkanRenderer::CheckDeviceLayerSupport(uint32& unsupportedLayer) const
    {
        if (m_DeviceLayers.GetSize() <= 0)
            return true;

        uint32 count = 0;
        Pawn::Core::Containers::Array<VkLayerProperties> availableLayers;

        vkEnumerateDeviceLayerProperties(m_PhysicalDevices[m_SelectedPhysicalDevice], &count, nullptr);

        availableLayers.Reserve(count);
        availableLayers.SetSize(count);

        vkEnumerateDeviceLayerProperties(m_PhysicalDevices[m_SelectedPhysicalDevice], &count, availableLayers.Data());

		unsupportedLayer = 0;
		for (const Pawn::Core::Containers::AnsiStringView str : m_Layers)
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

    bool VulkanRenderer::CheckDeviceExtensionSupport(uint32& unsupportedExtension) const
    {
        if (m_DeviceLayers.GetSize() <= 0)
            return true;

		uint32 extensionCount = 0;
		Pawn::Core::Containers::Array<VkExtensionProperties> availableExtensions;

        vkEnumerateDeviceExtensionProperties(m_PhysicalDevices[m_SelectedPhysicalDevice], nullptr, &extensionCount, nullptr);

		availableExtensions.Reserve(extensionCount);
		availableExtensions.SetSize(extensionCount);

        vkEnumerateDeviceExtensionProperties(m_PhysicalDevices[m_SelectedPhysicalDevice], nullptr, &extensionCount, availableExtensions.Data());

		unsupportedExtension = 0;
		for (const Pawn::Core::Containers::AnsiStringView str : m_Extensions)
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

    void VulkanRenderer::CheckPhysicalDevices()
    {
        Pawn::Core::Containers::Array<VkPhysicalDevice> suitableDevices;
        suitableDevices.Reserve(m_PhysicalDevices.GetSize());
        for (const auto& device : m_PhysicalDevices)
            if (DeviceSuitable(device))
                suitableDevices.EmplaceBack(device);

        suitableDevices.Reserve(suitableDevices.GetSize());
        suitableDevices.SetSize(suitableDevices.GetSize());

        m_PhysicalDevices = suitableDevices;
    }

    void VulkanRenderer::PickBestPhysicalDevice()
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

        m_SelectedPhysicalDevice = (uint32)bestDevice;
    }

    int32 VulkanRenderer::CalculatePhysicalDevicePower(VkPhysicalDevice device)
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

    int32 VulkanRenderer::PickQueueFamily()
    {
        uint32 familyCount = 0;
        Pawn::Core::Containers::Array<VkQueueFamilyProperties> props;

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

        if (m_GraphicsQueueFamily == (uint32)-1 || m_PresentQueueFamily == (uint32)-1)
        {
            PE_ASSERT(false, TEXT("Vulkan: can't find queue family!"));
            return PE_VK_RETURN_V(VulkanErrors::QueueFamilyNotFound);
        }

        return PE_VK_RETURN_V(VulkanErrors::Success);
    }

    inline bool VulkanRenderer::DeviceSuitable(VkPhysicalDevice device) const
    {
		VkPhysicalDeviceProperties deviceProperties;
		VkPhysicalDeviceFeatures deviceFeatures;
		vkGetPhysicalDeviceProperties(device, &deviceProperties);
		vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

		return (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU
            || deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU
            || deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU)
            && deviceFeatures.geometryShader;
    }

    void VulkanRenderer::UpdatePhysicalDevices()
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

	VKAPI_ATTR VkBool32 VKAPI_CALL VulkanRenderer::DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
	{
        switch (messageSeverity)
        {
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            {
                PE_TRACE("Message type: {1} Vulkan: \n\t{0}", pCallbackData->pMessage, (int32)messageType);
                break;
            }
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
		    {
                PE_INFO("Message type: {1} Vulkan info: \n\t{0}", pCallbackData->pMessage, (int32)messageType);
		    	break;
		    }
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
		    {
                PE_WARN("Message type: {1} Vulkan warning: \n\t{0}", pCallbackData->pMessage, (int32)messageType);
		    	break;
		    }
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
		    {
                PE_ERROR("Message type: {1} Vulkan error: \n\t{0}", pCallbackData->pMessage, (int32)messageType);
		    	break;
		    }
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT:
		    {
                PE_INFO("Message type: {1} Vulkan flag: \n\t{0}", pCallbackData->pMessage, (int32)messageType);
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