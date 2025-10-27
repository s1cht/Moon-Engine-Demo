#pragma once

#include <Core.hpp>

#include "Vulkan.hpp"
#include "Renderer/Base/SwapChain.h"

namespace ME::Render
{
	class Framebuffer;
	class CommandBuffer;
	class Texture2D;
}

namespace ME::Render
{
	class VulkanRenderAPI;

	class MEAPI VulkanSwapChain : public SwapChain
	{
	public:
		VulkanSwapChain(int32& result);
		~VulkanSwapChain();

	public:
		void Shutdown() override;

		void Resize(uint32 x, uint32 y) override;
		void SetFullscreen(bool fullscreen) override;
		inline void SetVSync(bool enabled) override;

		inline uint32 GetFrameIndex() const override { return m_CurrentFrame; }
		inline uint32 GetImageIndex() const override { return m_CurrentImage; }
		inline uint32 GetLastImageIndex() const override { return m_LastImage; }
		uint32 GetFrameCount() const override { return static_cast<uint32>(m_Images.Size()); }

		inline bool UpdateRequired() const override { return m_UpdateRequired; }
		inline void Updated() override
		{
			m_UpdateRequired = false;
		}

	public:
		void Present(VkQueue queue, VkSemaphore renderFinishedSemaphore);
		void NewFrame(VkSemaphore acquireSemaphore);
		inline VkSwapchainKHR GetSwapChain() { return m_SwapChain; }
		inline VkSurfaceCapabilitiesKHR GetCapabilities() { return m_Capabilities; }
		inline VkSurfaceFormatKHR GetFormats() { return m_Format; }
		inline VkPresentModeKHR GetPresentMode() { return m_PresentMode; }
		inline ME::Core::Containers::Array<ME::Core::Memory::Reference<ME::Render::Texture2D>> GetImages() override { return m_Images; }
		inline Core::Math::Resolution2D<uint32> GetExtent() override { return m_Extent; }

	private:
		int32 CreateSwapChain(VulkanRenderAPI* renderer, VkSwapchainKHR oldSwapChain);
		int32 CreateImages(VulkanRenderAPI* renderer);
		bool SetDetails(VkPhysicalDevice device, VkSurfaceKHR surface, 
			ME::Core::Containers::Array<VkSurfaceFormatKHR>& formats,
			ME::Core::Containers::Array<VkPresentModeKHR>& presentModes);
		void SelectFormat(ME::Core::Containers::Array<VkSurfaceFormatKHR>& formats);
		void SelectPresentMode(ME::Core::Containers::Array<VkPresentModeKHR>& formats);
		void SelectSwapExtent();
		void UpdateSwapExtent(uint32 x, uint32 y);

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
		uint32 m_CurrentImage;
		uint32 m_LastImage;
		ME::Core::Containers::Array<ME::Core::Memory::Reference<ME::Render::Texture2D>> m_Images;

		bool m_UpdateRequired;

	};
}
