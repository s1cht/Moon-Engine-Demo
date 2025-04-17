#pragma once

#include <Core.h>
#include <vulkan/vulkan.hpp>
#include "Renderer/Base/SwapChain.h"

import Pawn.Core.Container.Array;
import Pawn.Core.Container.String;

namespace Pawn::Render
{
	class VulkanRenderer;

	class PAWN_API VulkanSwapChain : public SwapChain
	{
	public:
		VulkanSwapChain(int32& result);
		~VulkanSwapChain();

	public:
		void Shutdown() override;

		inline void Present() override;
		void Resize(uint32 x, uint32 y) override;
		void SetFullscreen(bool fullscreen) override;
		inline void SetVSync(bool enabled) override;

	public:
		inline VkSwapchainKHR GetSwapChain() { return m_SwapChain; }
		inline VkSurfaceCapabilitiesKHR GetCapabilities() { return m_Capabilities; }
		inline VkSurfaceFormatKHR GetFormats() { return m_Format; }
		inline VkPresentModeKHR GetPresentMode() { return m_PresentMode; }
		inline VkExtent2D GetExtent() { return m_Extent; }
		inline Pawn::Core::Containers::Array<VkImage>& GetImages() { return m_Images; }
		inline Pawn::Core::Containers::Array<VkImageView>& GetImageViews() { return m_ImageViews; }

	private:
		int32 CreateSwapChain(VulkanRenderer* renderer, VkSwapchainKHR oldSwapChain);
		int32 CreateImageViews(VulkanRenderer* renderer);
		void SetImages(VulkanRenderer* renderer);
		bool SetDetails(VkPhysicalDevice device, VkSurfaceKHR surface, 
			Pawn::Core::Containers::Array<VkSurfaceFormatKHR>& formats,
			Pawn::Core::Containers::Array<VkPresentModeKHR>& presentModes);
		void SelectFormat(Pawn::Core::Containers::Array<VkSurfaceFormatKHR>& formats);
		void SelectPresentMode(Pawn::Core::Containers::Array<VkPresentModeKHR>& formats);
		void SelectSwapExtent();

	private:
		int32 Init();

	private:
		bool m_VSYNCEnabled;
		VkSwapchainKHR m_SwapChain;

		VkExtent2D m_Extent;
		VkFormat m_ImageFormat;
		VkSurfaceFormatKHR m_Format;
		VkPresentModeKHR m_PresentMode;
		VkSurfaceCapabilitiesKHR m_Capabilities;
		
		Pawn::Core::Containers::Array<VkImage> m_Images;
		Pawn::Core::Containers::Array<VkImageView> m_ImageViews;

	};
}