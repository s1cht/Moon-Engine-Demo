#include "VulkanSwapChain.h"
#include "Application/Application.h"
#include "Renderer/RenderCommand.h"
#include "VulkanRenderer.h"
#include "VulkanMacros.hpp"
#include <limits>
#include <algorithm>

#include "VulkanFunctions.h"

namespace Pawn::Render
{
	SwapChain* SwapChain::CreateVulkanSwapChain(int32& result)
	{
		return new VulkanSwapChain(result);
	}

	VulkanSwapChain::VulkanSwapChain(int32& result)
		: m_SwapChain(nullptr)
	{
		result = Init();
	}

	VulkanSwapChain::~VulkanSwapChain()
	{
		Shutdown();
	}

	void VulkanSwapChain::Shutdown()
	{
		VulkanRenderer* renderer = static_cast<VulkanRenderer*>(RenderCommand::Get());

		m_Images.Clear();
		if (m_SwapChain != nullptr)
		{
			vkDestroySwapchainKHR(renderer->GetDevice(), m_SwapChain, nullptr);
			m_SwapChain = nullptr;
		}
	}

	void VulkanSwapChain::Present() {}

	void VulkanSwapChain::Resize(uint32 x, uint32 y)
	{
		int32 result;
		VulkanRenderer* renderer = static_cast<VulkanRenderer*>(RenderCommand::Get());
		vkDeviceWaitIdle(renderer->GetDevice());
		VkSwapchainKHR old = m_SwapChain;
		m_SwapChain = nullptr;

		m_Images.Clear();

		result = CreateSwapChain(renderer, old);
		if (PE_VK_FAILED(result))
		{
			PE_ASSERT(false, TEXT("Vulkan swap chain: recreation failed! Error: {0}"), result);
			Shutdown();
			return;
		}

		m_ImageFormat = m_Format.format;

		result = CreateImages(renderer);
		if (PE_VK_FAILED(result))
		{
			PE_ASSERT(false, TEXT("Vulkan swap chain: Image recreation failed! Error: {0}"), result);
			Shutdown();
			return;
		}

	}

	void VulkanSwapChain::SetFullscreen(bool fullscreen)
	{
	}

	inline void VulkanSwapChain::SetVSync(bool enabled)
	{
		m_VSYNCEnabled = enabled;
	}

	int32 VulkanSwapChain::CreateSwapChain(VulkanRenderer* renderer, VkSwapchainKHR oldSwapChain)
	{
		uint32 imageCount;
		Pawn::Core::Containers::Array<uint32> queueFamilies;

		imageCount = m_Capabilities.minImageCount + 1;
		queueFamilies = { (uint32)renderer->GetGraphicsQueueFamily(), (uint32)renderer->GetPresentQueueFamily() };

		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = renderer->GetSurface();
		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = m_Format.format;
		createInfo.imageColorSpace = m_Format.colorSpace;
		createInfo.imageExtent = m_Extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		createInfo.imageSharingMode = renderer->GetGraphicsQueueFamily() == renderer->GetPresentQueueFamily() ? VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = renderer->GetGraphicsQueueFamily() == renderer->GetPresentQueueFamily() ? 0 : (uint32)queueFamilies.GetSize();
		createInfo.pQueueFamilyIndices = queueFamilies.Data();
		createInfo.preTransform = m_Capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = m_PresentMode;
		createInfo.clipped = VK_TRUE;
		createInfo.oldSwapchain = oldSwapChain;

		return PE_VK_RETURN_V(vkCreateSwapchainKHR(renderer->GetDevice(), &createInfo, nullptr, &m_SwapChain));
	}

	int32 VulkanSwapChain::CreateImages(VulkanRenderer* renderer)
	{
		Core::Containers::Array<VkImage> swapChainImages;
		Texture2DSpecification specs = {};
		specs.bOwnsImage = true;
		specs.Data = nullptr;
		specs.DataSize = -1;
		specs.Format = Pawn::Render::ConvertFormatEngine(m_ImageFormat);
		specs.Usage = ImageUsageFlags::None;
		specs.Resolution.x = m_Extent.width;
		specs.Resolution.y = m_Extent.height;
		

		uint32 count = 0;
		vkGetSwapchainImagesKHR(renderer->GetDevice(), m_SwapChain, &count, nullptr);
		swapChainImages.Reserve(count);
		swapChainImages.SetSize(count);
		vkGetSwapchainImagesKHR(renderer->GetDevice(), m_SwapChain, &count, swapChainImages.Data());

		for (uint32 i = 0; i < count; i++)
		{
			specs.DebugName = "SwapChain image" + Pawn::Core::Containers::ToAnsiString(1);

			m_Images.EmplaceBack(VulkanTexture2D(swapChainImages[i], specs));
		}

		return 0;
	}

	bool VulkanSwapChain::SetDetails(VkPhysicalDevice device, VkSurfaceKHR surface,
		Pawn::Core::Containers::Array<VkSurfaceFormatKHR>& formats,
		Pawn::Core::Containers::Array<VkPresentModeKHR>& presentModes)
	{
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &m_Capabilities);

		uint32 formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

		if (formatCount != 0) {
			formats.Reserve(formatCount);
			formats.SetSize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, formats.Data());
		}

		uint32 presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

		if (presentModeCount != 0) {
			presentModes.Reserve(formatCount);
			presentModes.SetSize(formatCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, presentModes.Data());
		}

		return presentModes.Empty() && formats.Empty();
	}

	void VulkanSwapChain::SelectFormat(Pawn::Core::Containers::Array<VkSurfaceFormatKHR>& formats)
	{
		for (const auto& availableFormat : formats) 
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) 
				m_Format = availableFormat;

		m_Format = formats[0];
	}

	void VulkanSwapChain::SelectPresentMode(Pawn::Core::Containers::Array<VkPresentModeKHR>& presentModes)
	{
		for (const auto& availablePresentMode : presentModes)
		{
			if (m_VSYNCEnabled)
			{
				if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR)
				{
					m_PresentMode = availablePresentMode;
					return;
				}
			}
			else
			{
				if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
				{
					m_PresentMode = availablePresentMode;
					return;
				}
			}
		}
		m_PresentMode = VK_PRESENT_MODE_FIFO_KHR;
	}

	void VulkanSwapChain::SelectSwapExtent()
	{
		Pawn::Application& app = Pawn::Application::Get();
		Pawn::Window* window = app.GetWindow();

		if (m_Capabilities.currentExtent.width != UINT32_MAX)
		{
			m_Extent = m_Capabilities.currentExtent;
		}
		else 
		{
			uint32 width, height;
			width = (uint32)window->GetWidth();
			height = (uint32)window->GetHeight();

			VkExtent2D actualExtent = { width, height };

			actualExtent.width = std::clamp(actualExtent.width, m_Capabilities.minImageExtent.width, m_Capabilities.maxImageExtent.width);
			actualExtent.height = std::clamp(actualExtent.height, m_Capabilities.minImageExtent.height, m_Capabilities.maxImageExtent.height);

			m_Extent = actualExtent;
		}
	}

	int32 VulkanSwapChain::Init()
	{
		int32 result;
		VulkanRenderer* renderer = static_cast<VulkanRenderer*>(RenderCommand::Get());
		Pawn::Core::Containers::Array<VkSurfaceFormatKHR> formats;
		Pawn::Core::Containers::Array<VkPresentModeKHR> presentModes;

		result = uint32(SetDetails(renderer->GetPhysicalDevice(), renderer->GetSurface(), formats, presentModes));
		if (PE_VK_FAILED(result))
		{
			PE_ASSERT(false, TEXT("Vulkan swap chain: Present modes or formats are empty! Error: {0}"), result);
			return PE_VK_RETURN_V(VulkanErrors::PresentModesOrFormatsEmpty);
		}

		SelectFormat(formats);
		SelectPresentMode(presentModes);
		SelectSwapExtent();

		result = CreateSwapChain(renderer, nullptr);
		if (PE_VK_FAILED(result))
		{
			PE_ASSERT(false, TEXT("Vulkan swap chain: creation failed! Error: {0}"), result);
			Shutdown();
			return PE_VK_RETURN_V(result);
		}

		m_ImageFormat = m_Format.format;

		result = CreateImages(renderer);
		if (PE_VK_FAILED(result))
		{
			PE_ASSERT(false, TEXT("Vulkan swap chain: Image recreation failed! Error: {0}"), result);
			Shutdown();
			return PE_VK_RETURN_V(result);

		}

		return result;
	}
}
