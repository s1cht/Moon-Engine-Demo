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

namespace ME::Render
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

	class MOON_API VulkanRenderer : public RendererAPI
	{
	public:
		VulkanRenderer();
		~VulkanRenderer();

	public:
		void PostInit() override;

		void Submit(ME::Core::Memory::Reference<Render::CommandBuffer> buffer) override;
		void NewFrame() override;
		void Present() override;
		void Clear(ME::Core::Math::Vector4D32 color) override;
		void Draw(ME::Core::Memory::Reference<Render::CommandBuffer> buffer, uint32 vertexCount, uint32 instanceCount) override;
		void DrawIndexed(ME::Core::Memory::Reference<Render::CommandBuffer> buffer, uint32 indexCount, uint32 index) override;
		void Shutdown() override;

		void OnWindowEvent(int32 x, int32 y) override;

		void BeginRenderPass(ME::Core::Memory::Reference<ME::Render::CommandBuffer> buffer, ME::Render::RenderPassBeginInfo& info) override;
		void EndRenderPass(ME::Core::Memory::Reference<ME::Render::CommandBuffer> buffer) override;

		ME::Core::Memory::Reference<ME::Render::CommandBuffer> GetAvailableCommandBuffer() override;

		inline SwapChain* GetSwapChain() override { return m_SwapChain; };

	public:
		inline VkInstance GetInstance() { return m_Instance; }
		inline VkSurfaceKHR GetSurface() { return m_Surface; }
		inline VkPhysicalDevice GetPhysicalDevice() { return m_PhysicalDevices[m_SelectedPhysicalDevice]; };
		inline int32 GetPresentQueueFamily() const { return m_PresentQueueFamily; }
		inline int32 GetGraphicsQueueFamily() const { return m_PresentQueueFamily; }
		inline VkDevice GetDevice() { return m_Device; }
		inline VkQueue GetPresentQueue() { return m_PresentQueue; }
		inline VkQueue GetGraphicsQueue() { return m_GraphicsQueue; }
		inline VkCommandPool GetCommandPool() { return m_GraphicsCommandPool; };

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
		int32 CreateCommandPool();

	private:
		void DestroyValidationLayer();
		void DestroySynchronizationObjects();

	private:
		void UpdatePhysicalDevices();
		
		bool CheckLayerSupport(uint32& unsupportedLayer) const;
		bool CheckExtensionSupport(uint32& unsupportedExtension) const;
		bool CheckDeviceLayerSupport(uint32& unsupportedLayer) const;
		bool CheckDeviceExtensionSupport(uint32& unsupportedExtension) const;
		inline bool CheckPhysicalDeviceFeatures(VkPhysicalDeviceFeatures features) const;
		inline bool CheckPhysicalDeviceProperties(VkPhysicalDeviceProperties properties) const;
		inline void CheckPhysicalDevices();
		void PickBestPhysicalDevice();
		int32 CalculatePhysicalDevicePower(VkPhysicalDevice device);
		int32 PickQueueFamily();
		inline bool DeviceSuitable(VkPhysicalDevice device) const;

	private:
		void CreateFrameInfoObjects();

	private:
		VkInstance m_Instance;
		VkSurfaceKHR m_Surface;
		VkDevice m_Device;

		VkQueue m_GraphicsQueue;
		VkQueue m_PresentQueue;

		VkCommandPool m_GraphicsCommandPool;

		uint32 m_SelectedPhysicalDevice;

		uint32 m_GraphicsQueueFamily;
		uint32 m_PresentQueueFamily;

	private:
		VulkanSwapChain* m_SwapChain;
		ME::Core::Containers::Array<VulkanFrameInfo> m_FrameInfos;

	private:
		VkDebugUtilsMessengerEXT m_ValidationLayer;

	private:
		ME::Core::Containers::Array<VkPhysicalDevice> m_PhysicalDevices;

	private:
		ME::Core::Containers::Array<const ansichar*> m_Layers;
		ME::Core::Containers::Array<const ansichar*> m_Extensions;

		ME::Core::Containers::Array<const ansichar*> m_DeviceLayers;
		ME::Core::Containers::Array<const ansichar*> m_DeviceExtensions;

	};
}
