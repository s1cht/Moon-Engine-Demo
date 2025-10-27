#pragma once

#include "Vulkan.hpp"
#include "Renderer/Base/Buffer.hpp"

namespace ME::Render
{
    class VulkanStorageBuffer final: public StorageBuffer
    {
	public:
		VulkanStorageBuffer(const StorageBufferSpecification& specification);
		VulkanStorageBuffer(const VulkanStorageBuffer&) = delete;
		VulkanStorageBuffer(VulkanStorageBuffer&&) = delete;
		~VulkanStorageBuffer() override;

    public:
		VulkanStorageBuffer& operator=(const VulkanStorageBuffer&) = delete;
		VulkanStorageBuffer& operator=(VulkanStorageBuffer&&) = delete;

	public:
		void Shutdown() override;

        void SetData(void* data, SIZE_T size, SIZE_T offset) override;
        void SetData(ME::Core::Memory::Reference<ME::Render::CommandBuffer> commandBuffer, void* data, SIZE_T size, SIZE_T offset) override;

		void Resize(SIZE_T size) override;

        void Clear() override;
        void Clear(ME::Core::Memory::Reference<ME::Render::CommandBuffer> commandBuffer) override;

		inline void UpdateResourceSet(uint32 setIndex) override { m_ResourceIndex = setIndex; };
		inline uint32 GetResourceSet() const override { return m_ResourceIndex; }

	public:
		inline VkBuffer GetBuffer() const { return m_Buffer; }

	private:
		void Init();
		VkResult CreateBuffer();
		VkResult CreateStagingBuffer();
		void ChooseOptimalDstStage();
		void ChooseOptimalSrcStage();

	private:
		void SetDataVRAM(const ME::Core::Memory::Reference<ME::Render::CommandBuffer>& commandBuffer, const void* data, SIZE_T size, SIZE_T offset) const;
		void SetDataRAM(const ME::Core::Memory::Reference<ME::Render::CommandBuffer>& commandBuffer, const void* data, SIZE_T size, SIZE_T offset) const;

    private:
		VkBuffer m_Buffer;
		VmaAllocation m_Allocation;

		VkBuffer m_StagingBuffer;
		VmaAllocation m_StagingAllocation;

	private:
		VkPipelineStageFlagBits m_OptimalDstPipelineStage;
		VkPipelineStageFlagBits m_OptimalSrcPipelineStage;

		void* m_MappedData;
		uint32 m_ResourceIndex;
	};
}