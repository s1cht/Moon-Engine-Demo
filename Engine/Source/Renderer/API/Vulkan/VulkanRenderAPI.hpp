#pragma once

#include <Core.hpp>
#include <Core/Memory/Memory.hpp>
#include <Core/Platform/Base/IO.hpp>
#include <Core/Containers/Array.hpp>

#include "Vulkan.hpp"
#include "VulkanResourceHandler.hpp"
#include "Renderer/Base/RenderAPI.hpp"
#include "VulkanSwapChain.hpp"

namespace ME::Render
{
	class VulkanResourceHandler;
}

namespace ME::Render
{
	enum class VulkanErrors : int32
	{
		Success = 0,
		// Renderer
		LayersUnsupported, ExtensionUnsupported,
		DeviceLayersUnsupported, DeviceExtensionUnsupported,
		IncompatiblePhysicalDevices, QueueFamilyNotFound,
        UnsupportedPlatform, FunctionNotPresent,
		// Swap chain
		PresentModesOrFormatsEmpty
	};

	struct MEAPI VulkanImageInfo
	{
		VkFence InFlight;
		VkSemaphore ImageAvailable;
		ME::Core::Memory::Reference<ME::Render::CommandBuffer> CommandBuffer;
		ME::Core::Memory::Reference<ME::Render::Framebuffer> Framebuffer;
	};

	class MEAPI VulkanRenderAPI final : public RenderAPI
	{
	public:
		VulkanRenderAPI();
		~VulkanRenderAPI() override;

	public:
		void PostInit() override;

		void CreateFramebuffers(ME::Core::Memory::Reference<ME::Render::RenderPass> renderPass,
			const ME::Core::Array<ME::Core::Memory::Reference<Render::RTexture2D>>& attachments) override;
		void Submit(ME::Core::Memory::Reference<Render::CommandBuffer> buffer) override;
		void NewFrame() override;
		void EndFrame() override;
		void Present() override;
		void Clear(ME::Core::Math::Vector4D32 color) override;

	    void Draw(ME::Core::Memory::Reference<Render::CommandBuffer> buffer, uint32 vertexCount, uint32 instanceCount) override;
		void DrawIndexed(ME::Core::Memory::Reference<Render::CommandBuffer> buffer, uint32 indexCount, uint32 index) override;
		void DrawIndexedIndirect(ME::Core::Memory::Reference<Render::CommandBuffer> commandBuffer,
			const ME::Core::Memory::Reference<Render::IndirectBuffer>& buffer, SIZE_T offset,
			uint32 drawCount, uint32 stride) override;
		void DrawIndexedIndirectCount(ME::Core::Memory::Reference<Render::CommandBuffer> commandBuffer,
			PipelineStageFlags bufferSrc,
			const ME::Core::Memory::Reference<Render::IndirectBuffer>& buffer, SIZE_T offset, 
			const ME::Core::Memory::Reference<Render::IndirectBuffer>& drawCount, uint32 drawCountOffset, 
			uint32 maxDrawCount, uint32 stride) override;
        void DispatchMesh(ME::Core::Memory::Reference<Render::CommandBuffer> commandBuffer, uint32 groupCountX,
            uint32 groupCountY, uint32 groupCountZ) override;
		void Dispatch(ME::Core::Memory::Reference<Render::CommandBuffer> commandBuffer, uint32 groupCountX, uint32 groupCountY, uint32 groupCountZ) override;

	    void Shutdown() override;

		inline void WriteResource(ME::Core::Memory::Reference<ME::Render::Uniform> buffer) override;
		inline void WriteResource(ME::Core::Memory::Reference<ME::Render::StorageBuffer> buffer) override;
		inline void WriteResource(ME::Core::Memory::Reference<ME::Render::IndexBuffer> buffer) override;
		inline void WriteResource(ME::Core::Memory::Reference<ME::Render::VertexBuffer> buffer) override;
		inline void WriteResource(ME::Core::Memory::Reference<ME::Render::IndirectBuffer> buffer) override;
		inline void BindResourceSet(ME::Core::Memory::Reference<Render::CommandBuffer> commandBuffer, 
			ME::Core::Memory::Reference<Render::Pipeline> pipeline, 
			ME::Core::Memory::Reference<ME::Render::Buffer> buffer) override;
		inline void BindIndexBuffer(ME::Core::Memory::Reference<Render::CommandBuffer> commandBuffer,
			const ME::Core::Memory::Reference<Render::IndexBuffer>& buffer, uint32 offset) override;
		inline void BindVertexBuffer(ME::Core::Memory::Reference<Render::CommandBuffer> commandBuffer,
			const ME::Core::Memory::Reference<Render::VertexBuffer>& buffer, uint32 offset) override;

		inline void BufferBarrier(ME::Core::Memory::Reference<Render::CommandBuffer> commandBuffer,
			const ME::Core::Memory::Reference<Render::VertexBuffer>& buffer, BarrierInfo src,
			BarrierInfo dst) override;
		inline void BufferBarrier(ME::Core::Memory::Reference<Render::CommandBuffer> commandBuffer,
			const ME::Core::Memory::Reference<Render::IndexBuffer>& buffer, BarrierInfo src,
			BarrierInfo dst) override;
		inline void BufferBarrier(ME::Core::Memory::Reference<Render::CommandBuffer> commandBuffer,
			const ME::Core::Memory::Reference<Render::IndirectBuffer>& buffer, BarrierInfo src,
			BarrierInfo dst) override;
		inline void BufferBarrier(ME::Core::Memory::Reference<Render::CommandBuffer> commandBuffer,
			const ME::Core::Memory::Reference<Render::StorageBuffer>& buffer, BarrierInfo src,
			BarrierInfo dst) override;
		inline void BufferBarrier(ME::Core::Memory::Reference<Render::CommandBuffer> commandBuffer,
			const ME::Core::Memory::Reference<Render::Uniform>& buffer, BarrierInfo src,
			BarrierInfo dst) override;

		inline void TextureBarrier(ME::Core::Memory::Reference<Render::CommandBuffer> commandBuffer,
			const ME::Core::Memory::Reference<Render::Texture1D>& texture, BarrierInfo src,
			BarrierInfo dst) override;
		inline void TextureBarrier(ME::Core::Memory::Reference<Render::CommandBuffer> commandBuffer,
			const ME::Core::Memory::Reference<Render::Texture2D>& texture, BarrierInfo src,
			BarrierInfo dst) override;
		inline void TextureBarrier(ME::Core::Memory::Reference<Render::CommandBuffer> commandBuffer,
			const ME::Core::Memory::Reference<Render::Texture3D>& texture, BarrierInfo src,
			BarrierInfo dst) override;

		void BindTexture(ME::Core::Memory::Reference<Render::CommandBuffer> commandBuffer, ME::Core::Memory::Reference<Render::Pipeline> pipeline, ME::Core::Memory::Reference<ME::Render::Texture2D> texture, uint32 set) override;

		void OnWindowEvent(int32 x, int32 y) override;

		void BeginRenderPass(ME::Core::Memory::Reference<ME::Render::CommandBuffer> buffer, ME::Render::RenderPassBeginInfo& info) override;
		void EndRenderPass(ME::Core::Memory::Reference<ME::Render::CommandBuffer> buffer) override;

		ME::Core::Memory::Reference<ME::Render::CommandBuffer> GetAvailableCommandBuffer() override;

		ME::Core::Memory::Reference<ME::Render::CommandBuffer> GetSingleUseCommandBuffer() override;
		void SubmitAndFreeSingleUseCommandBuffer(ME::Core::Memory::Reference<ME::Render::CommandBuffer> buffer) override;

		void Resize(uint32 x, uint32 y) override;

		ME::Core::Memory::Reference<ME::Render::Framebuffer> GetAvailableFramebuffer() const override;

		inline ME::Core::Memory::Reference<Render::SwapChain> GetSwapChain() override { return m_SwapChain; }
		inline ME::Core::Memory::Reference<Render::ResourceHandler> GetResourceHandler() override { return m_ResourceHandler; }

	public:
		inline VkInstance GetInstance() const { return m_Instance; }
		inline VkSurfaceKHR GetSurface() const { return m_Surface; }
		inline VkPhysicalDevice GetPhysicalDevice() const { return m_PhysicalDevices[m_SelectedPhysicalDevice]; }
		inline int32 GetPresentQueueFamily() const { return m_PresentQueueFamily; }
		inline int32 GetGraphicsQueueFamily() const { return m_PresentQueueFamily; }
		inline VkDevice GetDevice() const { return m_Device; }
		inline VkQueue GetPresentQueue() const { return m_PresentQueue; }
		inline VkQueue GetGraphicsQueue() const { return m_GraphicsQueue; }
		inline VkCommandPool GetCommandPool() const { return m_GraphicsCommandPool; }
		inline VmaAllocator GetAllocator() const { return m_Allocator; }

		inline Render::VulkanResourceHandler* GetVulkanResourceHandler() { return m_ResourceHandler->As<VulkanResourceHandler>(); }
		inline Render::VulkanSwapChain* GetVulkanSwapChain() { return m_SwapChain->As<VulkanSwapChain>(); }

	private:
		static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
			VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
			VkDebugUtilsMessageTypeFlagsEXT messageType,
			const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
			void* pUserData);

	private:
		int32 Init();

	private:
		int32 CreateAllocator(const uint32& apiVersion);
		int32 CreateInstance(const uint32& apiVersion);
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
		static inline bool CheckPhysicalDeviceFeatures(const VkPhysicalDeviceFeatures& features);
		static inline bool CheckPhysicalDeviceProperties(const VkPhysicalDeviceProperties& properties);
		inline void CheckPhysicalDevices();
		void PickBestPhysicalDevice();
		static int32 CalculatePhysicalDevicePower(VkPhysicalDevice device);
		int32 PickQueueFamily();
		static inline bool DeviceSuitable(VkPhysicalDevice device);
		int32 AcquireAllRequiredFunctions();

	private:
		void CreateFrameInfoObjects();
		void CreateFramebuffers(ME::Core::Memory::Reference<ME::Render::RenderPass> renderPass);

    private:
        PFN_vkCmdDrawMeshTasksEXT f_vkCmdDrawMeshTasksEXT;

    private:
		VkInstance m_Instance;
		VkSurfaceKHR m_Surface;
		VkDevice m_Device;

	private:
		uint32 m_SelectedPhysicalDevice;
		ME::Core::Array<VkPhysicalDevice> m_PhysicalDevices;

	private:
		VmaAllocator m_Allocator;
		VkCommandPool m_GraphicsCommandPool;
		ME::Core::Memory::Reference<ResourceHandler> m_ResourceHandler;

	private:
		uint32 m_GraphicsQueueFamily;
		uint32 m_PresentQueueFamily;

		VkQueue m_GraphicsQueue;
		VkQueue m_PresentQueue;

	private:
		ME::Core::Memory::Reference<SwapChain> m_SwapChain;
		ME::Core::Array<VkSemaphore> m_SubmitSemaphores;
		ME::Core::Array<VulkanImageInfo> m_FrameInfos;
		ME::Core::Array<ME::Core::Memory::Reference<Render::RTexture2D>> m_FramebufferAttachments;

	private:
		ME::Core::Array<ME::Core::StringView> m_Layers;
		ME::Core::Array<ME::Core::StringView> m_Extensions;

		ME::Core::Array<ME::Core::StringView> m_DeviceLayers;
		ME::Core::Array<ME::Core::StringView> m_DeviceExtensions;

	private:
		VkDebugUtilsMessengerEXT m_ValidationLayer;

	};
}
