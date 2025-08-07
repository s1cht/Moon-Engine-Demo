#pragma once

#include <Core.hpp>
#include <Core/Containers/String/String.hpp>
#include <Core/Containers/Array.hpp>

#include <vulkan/vulkan.hpp>

#include "VulkanCommandBuffer.h"
#include "Renderer/Base/SwapChain.h"
#include "VulkanTexture.h"

namespace ME::Render
{
	class VulkanRenderer;

	struct MOON_API VulkanFrameInfo
	{
		VkSemaphore ImageAvailable;
		VkSemaphore RenderFinished;
		VkFence InFlight;
		ME::Core::Memory::Reference<ME::Render::CommandBuffer> CommandBuffer;
	};

	class MOON_API VulkanSwapChain : public SwapChain
	{
	public:
		VulkanSwapChain(int32& result);
		~VulkanSwapChain();

	public:
		void Shutdown() override;

		void Resize(uint32 x, uint32 y) override;
		void SetFullscreen(bool fullscreen) override;
		inline void SetVSync(bool enabled) override;

		inline uint32 GetFrameIndex() override { return m_CurrentFrame; }

	public:
		void Present(VkQueue queue, VulkanFrameInfo& info);
		void NewFrame(VulkanFrameInfo& info);
		inline VkSwapchainKHR GetSwapChain() { return m_SwapChain; }
		inline VkSurfaceCapabilitiesKHR GetCapabilities() { return m_Capabilities; }
		inline VkSurfaceFormatKHR GetFormats() { return m_Format; }
		inline VkPresentModeKHR GetPresentMode() { return m_PresentMode; }
		inline ME::Core::Containers::Array<Texture2D*> GetImages() override { return m_Images; }
		inline Core::Math::Resolution2D<uint32> GetExtent() override { return m_Extent; }

	private:
		int32 CreateSwapChain(VulkanRenderer* renderer, VkSwapchainKHR oldSwapChain);
		int32 CreateImages(VulkanRenderer* renderer);
		bool SetDetails(VkPhysicalDevice device, VkSurfaceKHR surface, 
			ME::Core::Containers::Array<VkSurfaceFormatKHR>& formats,
			ME::Core::Containers::Array<VkPresentModeKHR>& presentModes);
		void SelectFormat(ME::Core::Containers::Array<VkSurfaceFormatKHR>& formats);
		void SelectPresentMode(ME::Core::Containers::Array<VkPresentModeKHR>& formats);
		void SelectSwapExtent();

	private:
		int32 Init();

	private:
		bool m_VSYNCEnabled;
		VkSwapchainKHR m_SwapChain;

		Core::Math::Resolution2D<uint32> m_Extent;
		VkFormat m_ImageFormat;
		VkSurfaceFormatKHR m_Format;
		VkPresentModeKHR m_PresentMode;
		VkSurfaceCapabilitiesKHR m_Capabilities;

		uint32 m_CurrentFrame;
		uint32 m_NextImageIndex;
		ME::Core::Containers::Array<Texture2D*> m_Images;

	};
}
