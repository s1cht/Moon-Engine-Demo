#include "VulkanSwapChain.hpp"
#include <algorithm>
#include <complex>
#include <limits>

#include "Application/Application.h"
#include "VulkanRenderAPI.hpp"
#include "VulkanFunctions.hpp"
#include "VulkanTexture.hpp"
#include "Renderer/RenderCommand.h"
#include "Renderer/RenderResourcesTracker.hpp"

namespace ME::Render
{
	ME::Core::Memory::Reference<SwapChain> SwapChain::CreateVulkanSwapChain(int32& result)
	{
		return ME::Core::Memory::Reference<SwapChain>(new VulkanSwapChain(result));
	}

	VulkanSwapChain::VulkanSwapChain(int32& result)
		: m_SwapChain(nullptr), m_VSYNCEnabled(false), m_CurrentFrame(0), m_CurrentImage(0), m_UpdateRequired(false), m_Images({})
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
			image->Shutdown();
			image = nullptr;
		}
		m_Images.Clear();
		if (m_SwapChain != nullptr)
		{
			vkDestroySwapchainKHR(Render::RenderCommand::Get()->As<VulkanRenderAPI>()->GetDevice(), m_SwapChain, nullptr);
			m_SwapChain = nullptr;
		}
	}

	void VulkanSwapChain::Present(VkQueue queue, VkSemaphore renderFinishedSemaphore)
	{
		VkResult result;
		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &renderFinishedSemaphore;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &m_SwapChain;
		presentInfo.pImageIndices = &m_CurrentImage;

		result = vkQueuePresentKHR(queue, &presentInfo);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
		{
			ME::Application& app = ME::Application::Get();
			uint32 width = static_cast<uint32>(app.GetWindow()->GetWidth());
			uint32 height = static_cast<uint32>(app.GetWindow()->GetHeight());
			Render::RenderCommand::Get()->Resize(width, height);
			m_UpdateRequired = true;
		}
		else if (ME_VK_FAILED(result))
			ME_ASSERT(false, "Failed to present! Error: {}", static_cast<int32>(result));
		m_CurrentFrame = (m_CurrentFrame + 1) % m_Images.Size();
	}

	void VulkanSwapChain::NewFrame(VkSemaphore acquireSemaphore)
	{
		m_LastImage = m_CurrentImage;
	    VkResult result = vkAcquireNextImageKHR(RenderCommand::Get()->As<VulkanRenderAPI>()->GetDevice(), m_SwapChain, UINT64_MAX, acquireSemaphore, nullptr, &m_CurrentImage);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
		{
			ME::Application& app = ME::Application::Get();
			uint32 width = static_cast<uint32>(app.GetWindow()->GetWidth());
			uint32 height = static_cast<uint32>(app.GetWindow()->GetHeight());
			Render::RenderCommand::Get()->Resize(width, height);
			m_UpdateRequired = true;
		}
		else if (ME_VK_FAILED(result))
			ME_ASSERT("Failed to start new frame! Error: {}", static_cast<int32>(result));
	}

	void VulkanSwapChain::Resize(uint32 x, uint32 y)
	{
		if (x == 0 || y == 0)
		{
			m_UpdateRequired = true;
			return;
		}

		int32 result;
		VkSwapchainKHR oldSwapChain;
		VulkanRenderAPI* render = Render::RenderCommand::Get()->As<VulkanRenderAPI>();
		vkDeviceWaitIdle(render->GetDevice());

		UpdateSwapExtent(x, y);

		oldSwapChain = m_SwapChain;
		m_SwapChain = nullptr;

		for (auto image : m_Images)
		{
			image->Shutdown();
		}
		m_Images.Clear();

		result = CreateSwapChain(render, oldSwapChain);
		if (ME_VK_FAILED(result))
		{
			ME_ASSERT(false, "Vulkan swap chain: recreation failed! Error: {0}", result);
			Shutdown();
			return;
		}

		result = CreateImages(render);
		if (ME_VK_FAILED(result))
		{
			ME_ASSERT(false, "Vulkan swap chain: Image recreation failed! Error: {0}", result);
			Shutdown();
			return;
		}

		if (oldSwapChain != nullptr)
		{
			vkDestroySwapchainKHR(Render::RenderCommand::Get()->As<VulkanRenderAPI>()->GetDevice(), oldSwapChain, nullptr);
		}

		oldSwapChain = nullptr;
	}

	void VulkanSwapChain::SetFullscreen(bool fullscreen)
	{
	}

	inline void VulkanSwapChain::SetVSync(bool enabled)
	{
		m_VSYNCEnabled = enabled;
	}

	int32 VulkanSwapChain::CreateSwapChain(VulkanRenderAPI* renderer, VkSwapchainKHR oldSwapChain)
	{
		uint32 imageCount;
		ME::Core::Containers::Array<uint32> queueFamilies;

		imageCount = m_Capabilities.minImageCount;
		queueFamilies = { static_cast<uint32>(renderer->GetGraphicsQueueFamily()), static_cast<uint32>(renderer->GetPresentQueueFamily()) };

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
		createInfo.queueFamilyIndexCount = renderer->GetGraphicsQueueFamily() == renderer->GetPresentQueueFamily() ? 0 : (uint32)queueFamilies.Size();
		createInfo.pQueueFamilyIndices = queueFamilies.Data();
		createInfo.preTransform = m_Capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = m_PresentMode;
		createInfo.clipped = VK_TRUE;
		
		createInfo.oldSwapchain = oldSwapChain;

		return ME_VK_RETURN_V(vkCreateSwapchainKHR(renderer->GetDevice(), &createInfo, nullptr, &m_SwapChain));
	}

	int32 VulkanSwapChain::CreateImages(VulkanRenderAPI* renderer)
	{
		Core::Containers::Array<VkImage> swapChainImages;
		Texture2DSpecification specs = {};
		specs.bOwnsImage = false;
		specs.Data = nullptr;
		specs.DataSize = -1;
		specs.Format = ME::Render::ConvertFormatEngine(m_ImageFormat);
		specs.Usage = ImageUsageFlags::ColorAttachment;
		specs.IsDepth = false;
		specs.IsStencil = false;
		specs.MipLevels = 1;
		specs.SampleCount = SampleCount::Count1;
		specs.CubeMapCount = 0;
		specs.Layout = ImageLayout::ColorAttachment;
		specs.Resolution.x = m_Extent.x;
		specs.Resolution.y = m_Extent.y;

		uint32 count = 0;
		vkGetSwapchainImagesKHR(renderer->GetDevice(), m_SwapChain, &count, nullptr);
		swapChainImages.Reserve(count);
		swapChainImages.Resize(count);
		vkGetSwapchainImagesKHR(renderer->GetDevice(), m_SwapChain, &count, swapChainImages.Data());

		for (uint32 i = 0; i < count; i++)
		{
			specs.DebugName = "SwapChain image" + ME::Core::ToString(i);
			m_Images.PushBack(ME::Core::Memory::Reference<Texture2D>(new VulkanTexture2D(swapChainImages[i], specs)));
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
			formats.Resize(formatCount);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, formats.Data());
		}

		uint32 presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

		if (presentModeCount != 0) {
			presentModes.Reserve(formatCount);
			presentModes.Resize(formatCount);
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

	void VulkanSwapChain::UpdateSwapExtent(uint32 x, uint32 y)
	{
		VulkanRenderAPI* render = Render::RenderCommand::Get()->As<VulkanRenderAPI>();

		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(render->GetPhysicalDevice(), render->GetSurface(), &m_Capabilities);

		if (m_Capabilities.currentExtent.width != UINT32_MAX) {
			m_Extent = { m_Capabilities.currentExtent.width, m_Capabilities.currentExtent.height };
		}
		else {
			m_Extent = { x, y };
			m_Extent.x = std::clamp(m_Extent.x, m_Capabilities.minImageExtent.width, m_Capabilities.maxImageExtent.width);
			m_Extent.y = std::clamp(m_Extent.y, m_Capabilities.minImageExtent.height, m_Capabilities.maxImageExtent.height);
		}
	}

	int32 VulkanSwapChain::Init()
	{
		int32 result;
		VulkanRenderAPI* render = Render::RenderCommand::Get()->As<VulkanRenderAPI>();
		ME::Core::Containers::Array<VkSurfaceFormatKHR> formats;
		ME::Core::Containers::Array<VkPresentModeKHR> presentModes;

		result = uint32(SetDetails(render->GetPhysicalDevice(), render->GetSurface(), formats, presentModes));
		if (ME_VK_FAILED(result))
		{
			ME_ASSERT(false, "Vulkan swap chain: Present modes or formats are empty! Error: {0}", result);
			return ME_VK_RETURN_V(VulkanErrors::PresentModesOrFormatsEmpty);
		}

		SelectFormat(formats);
		SelectPresentMode(presentModes);
		SelectSwapExtent();

		result = CreateSwapChain(render, nullptr);
		if (ME_VK_FAILED(result))
		{
			ME_ASSERT(false, "Vulkan swap chain: creation failed! Error: {0}", result);
			Shutdown();
			return ME_VK_RETURN_V(result);
		}

		m_ImageFormat = m_Format.format;

		result = CreateImages(render);
		if (ME_VK_FAILED(result))
		{
			ME_ASSERT(false, "Vulkan swap chain: Image recreation failed! Error: {0}", result);
			Shutdown();
			return ME_VK_RETURN_V(result);
		}

		return result;
	}
}
