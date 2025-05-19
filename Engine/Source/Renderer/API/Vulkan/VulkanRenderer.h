#pragma once

#include <Core.hpp>
#include <Core/Memory/Memory.hpp>
#include <Core/Platform/Base/IO.hpp>
#include <Core/Containers/Array.hpp>
#include <Core/Containers/String/String.hpp>

#ifdef PLATFORM_WINDOWS
	#define VK_USE_PLATFORM_WIN32_KHR
	#include "Platform/Windows/Win32Window.h"
	#include "Platform/Windows/Win32Platform.h"

#endif

#include "EngineDefines.h"
#include "Renderer/Base/RendererAPI.h"
#include "VulkanSwapChain.h"

#include <vulkan/vulkan.hpp>

namespace Pawn::Render
{
	enum class VulkanErrors : int32
	{
		Success = 0,
		// Renderer
		LayersUnsupported, ExtensionUnsupported,
		DeviceLayersUnsupported, DeviceExtensionUnsupported,
		IncompatiblePhysicalDevices, QueueFamilyNotFound,
		UnsupportedPlatform, 
		// Swap chain
		PresentModesOrFormatsEmpty
	};

	class PAWN_API VulkanRenderer : public RendererAPI
	{
	public:
		VulkanRenderer();
		~VulkanRenderer();

	public:
		void PostInit() override;

		void Present() override;
		void Clear(Pawn::Core::Math::Vector4D32 color) override;
		void DrawIndexed(uint32 indexCount, uint32 index) override;
		void Shutdown() override;

		void OnWindowEvent(int32 x, int32 y) override;

		void BindBackBuffer() override;
		void UnbindBackBuffer() override;

	public:
		inline VkInstance GetInstance() { return m_Instance; }
		inline VkSurfaceKHR GetSurface() { return m_Surface; }
		inline VkPhysicalDevice GetPhysicalDevice() { return m_PhysicalDevices[m_SelectedPhysicalDevice]; };
		inline int32 GetPresentQueueFamily() const { return m_PresentQueueFamily; }
		inline int32 GetGraphicsQueueFamily() const { return m_PresentQueueFamily; }
		inline VkDevice GetDevice() { return m_Device; }
		inline VkQueue GetPresentQueue() { return m_PresentQueue; }
		inline VkQueue GetGraphicsQueue() { return m_GraphicsQueue; }

	private:
		static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
			VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
			VkDebugUtilsMessageTypeFlagsEXT messageType,
			const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
			void* pUserData);

	private:
		int32 Init();

	private:
		int32 CreateInstance();
		int32 CreateValidationLayer();
		int32 CreateDevice();
		int32 CreateWindowSurface();

	private:
		void DestroyValidationLayer();

	private:
		void UpdatePhysicalDevices();
		
		bool CheckLayerSupport(uint32& unsupportedLayer) const;
		bool CheckExtensionSupport(uint32& unsupportedExtension) const;
		bool CheckDeviceLayerSupport(uint32& unsupportedLayer) const;
		bool CheckDeviceExtensionSupport(uint32& unsupportedExtension) const;
		void CheckPhysicalDevices();
		void PickBestPhysicalDevice();
		int32 CalculatePhysicalDevicePower(VkPhysicalDevice device);
		int32 PickQueueFamily();
		inline bool DeviceSuitable(VkPhysicalDevice device) const;


	private:
		VkInstance m_Instance;
		VkSurfaceKHR m_Surface;
		VkDevice m_Device;

		VkQueue m_GraphicsQueue;
		VkQueue m_PresentQueue;

		uint32 m_SelectedPhysicalDevice;

		uint32 m_GraphicsQueueFamily;
		uint32 m_PresentQueueFamily;

	private:
		VulkanSwapChain* m_SwapChain;

	private:
		VkDebugUtilsMessengerEXT m_ValidationLayer;

	private:
		Pawn::Core::Containers::Array<VkPhysicalDevice> m_PhysicalDevices;

	private:
		Pawn::Core::Containers::Array<const ansichar*> m_Layers;
		Pawn::Core::Containers::Array<const ansichar*> m_Extensions;

		Pawn::Core::Containers::Array<const ansichar*> m_DeviceLayers;
		Pawn::Core::Containers::Array<const ansichar*> m_DeviceExtensions;

	};
}

#define PE_VK_RETURN_V(val) (int32(val))
#define PE_VK_FAILED(result) (result != (int32)VkResult::VK_SUCCESS || (int32)VulkanErrors::Success)