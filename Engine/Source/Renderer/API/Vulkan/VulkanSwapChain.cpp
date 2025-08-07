#include "VulkanSwapChain.h"
#include "Application/Application.h"
#include "Renderer/RenderCommand.h"
#include "VulkanRenderer.h"
#include "VulkanMacros.hpp"
#include <limits>
#include <algorithm>
#include <complex>

#include "VulkanFunctions.h"
#include "Renderer/RenderResourcesTracker.hpp"

namespace ME::Render
{
	ME::Core::Memory::Reference<SwapChain> SwapChain::CreateVulkanSwapChain(int32& result)
	{
		auto object = ME::Core::Memory::Reference<SwapChain>(new VulkanSwapChain(result));
		RenderResourcesTracker::Get().AddItem(object);
		return object;
	}

	VulkanSwapChain::VulkanSwapChain(int32& result)
		: m_SwapChain(nullptr), m_VSYNCEnabled(false), m_CurrentFrame(0)
	{
		result = Init();
	}

	VulkanSwapChain::~VulkanSwapChain()
	{
		Shutdown();
	}

	void VulkanSwapChain::Shutdown()
	{
		for (auto image : m_Images)
		{
			delete image;
			image = nullptr;
		}
		m_Images.Clear();
		if (m_SwapChain != nullptr)
		{
			vkDestroySwapchainKHR(Render::RenderCommand::Get()->As<VulkanRenderer>()->GetDevice(), m_SwapChain, nullptr);
			m_SwapChain = nullptr;
		}
	}

	void VulkanSwapChain::Present(VkQueue queue, VulkanFrameInfo& info)
	{
		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &info.RenderFinished;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &m_SwapChain;
		presentInfo.pImageIndices = &m_NextImageIndex;

		vkQueuePresentKHR(queue, &presentInfo);

		m_CurrentFrame = (m_NextImageIndex + 1) % m_Images.GetSize();
	}

	void VulkanSwapChain::NewFrame(VulkanFrameInfo& info)
	{
		vkAcquireNextImageKHR(RenderCommand::Get()->As<VulkanRenderer>()->GetDevice(), m_SwapChain, UINT64_MAX, info.ImageAvailable, nullptr, &m_NextImageIndex);
	}

	void VulkanSwapChain::Resize(uint32 x, uint32 y)
	{
		int32 result;
		VulkanRenderer* render = Render::RenderCommand::Get()->As<VulkanRenderer>();
		vkDeviceWaitIdle(render->GetDevice());
		VkSwapchainKHR old = m_SwapChain;
		m_SwapChain = nullptr;

		m_Images.Clear();

		result = CreateSwapChain(render, old);
		if (ME_VK_FAILED(result))
		{
			ME_ASSERT(false, TEXT("Vulkan swap chain: recreation failed! Error: {0}"), result);
			Shutdown();
			return;
		}

		m_ImageFormat = m_Format.format;

		result = CreateImages(render);
		if (ME_VK_FAILED(result))
		{
			ME_ASSERT(false, TEXT("Vulkan swap chain: Image recreation failed! Error: {0}"), result);
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
		ME::Core::Containers::Array<uint32> queueFamilies;

		imageCount = m_Capabilities.minImageCount + 1;
		queueFamilies = { (uint32)renderer->GetGraphicsQueueFamily(), (uint32)renderer->GetPresentQueueFamily() };

		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = renderer->GetSurface();
		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = m_Format.format;
		createInfo.imageColorSpace = m_Format.colorSpace;
		createInfo.imageExtent = { m_Extent.x, m_Extent.y };
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

		return ME_VK_RETURN_V(vkCreateSwapchainKHR(renderer->GetDevice(), &createInfo, nullptr, &m_SwapChain));
	}

	int32 VulkanSwapChain::CreateImages(VulkanRenderer* renderer)
	{
		Core::Containers::Array<VkImage> swapChainImages;
		Texture2DSpecification specs = {};
		specs.bOwnsImage = true;
		specs.Data = nullptr;
		specs.DataSize = -1;
		specs.Format = ME::Render::ConvertFormatEngine(m_ImageFormat);
		specs.Usage = ImageUsageFlags::ColorAttachment;
		specs.IsDepth = false;
		specs.IsStencil = false;
		specs.Resolution.x = m_Extent.x;
		specs.Resolution.y = m_Extent.y;

		uint32 count = 0;
		vkGetSwapchainImagesKHR(renderer->GetDevice(), m_SwapChain, &count, nullptr);
		swapChainImages.Reserve(count);
		swapChainImages.SetSize(count);
		vkGetSwapchainImagesKHR(renderer->GetDevice(), m_SwapChain, &count, swapChainImages.Data());

		for (uint32 i = 0; i < count; i++)
		{
			specs.DebugName = "SwapChain image" + ME::Core::Containers::ToAnsiString(1);

			m_Images.EmplaceBack(new VulkanTexture2D(swapChainImages[i], specs));
		}

		return 0;
	}

	bool VulkanSwapChain::SetDetails(VkPhysicalDevice device, VkSurfaceKHR surface,
		ME::Core::Containers::Array<VkSurfaceFormatKHR>& formats,
		ME::Core::Containers::Array<VkPresentModeKHR>& presentModes)
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

	void VulkanSwapChain::SelectFormat(ME::Core::Containers::Array<VkSurfaceFormatKHR>& formats)
	{
		for (const auto& availableFormat : formats) 
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) 
				m_Format = availableFormat;

		m_Format = formats[0];
	}

	void VulkanSwapChain::SelectPresentMode(ME::Core::Containers::Array<VkPresentModeKHR>& presentModes)
	{
		for (const auto& availablePresentMode : presentModes)
		{
			if (!m_VSYNCEnabled)
			{
				if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR)
				{
					m_PresentMode = availablePresentMode;
					return;
				}
			}
			else
			{
				if (availablePresentMode == VK_PRESENT_MODE_FIFO_KHR)
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
		ME::Application& app = ME::Application::Get();
		ME::Window* window = app.GetWindow();

		if (m_Capabilities.currentExtent.width != UINT32_MAX)
		{
			m_Extent = { m_Capabilities.currentExtent.width, m_Capabilities.currentExtent.height };
		}
		else 
		{
			uint32 width, height;
			width = (uint32)window->GetWidth();
			height = (uint32)window->GetHeight();

			Core::Math::Resolution2D<uint32> actualExtent = { width, height };
				
			actualExtent.x = std::clamp(actualExtent.x, m_Capabilities.minImageExtent.width, m_Capabilities.maxImageExtent.width);
			actualExtent.y = std::clamp(actualExtent.y, m_Capabilities.minImageExtent.height, m_Capabilities.maxImageExtent.height);

			m_Extent = actualExtent;
		}
	}

	int32 VulkanSwapChain::Init()
	{
		int32 result;
		VulkanRenderer* render = Render::RenderCommand::Get()->As<VulkanRenderer>();
		ME::Core::Containers::Array<VkSurfaceFormatKHR> formats;
		ME::Core::Containers::Array<VkPresentModeKHR> presentModes;

		result = uint32(SetDetails(render->GetPhysicalDevice(), render->GetSurface(), formats, presentModes));
		if (ME_VK_FAILED(result))
		{
			ME_ASSERT(false, TEXT("Vulkan swap chain: Present modes or formats are empty! Error: {0}"), result);
			return ME_VK_RETURN_V(VulkanErrors::PresentModesOrFormatsEmpty);
		}

		SelectFormat(formats);
		SelectPresentMode(presentModes);
		SelectSwapExtent();

		result = CreateSwapChain(render, nullptr);
		if (ME_VK_FAILED(result))
		{
			ME_ASSERT(false, TEXT("Vulkan swap chain: creation failed! Error: {0}"), result);
			Shutdown();
			return ME_VK_RETURN_V(result);
		}

		m_ImageFormat = m_Format.format;

		result = CreateImages(render);
		if (ME_VK_FAILED(result))
		{
			ME_ASSERT(false, TEXT("Vulkan swap chain: Image recreation failed! Error: {0}"), result);
			Shutdown();
			return ME_VK_RETURN_V(result);

		}

		return result;
	}
}
