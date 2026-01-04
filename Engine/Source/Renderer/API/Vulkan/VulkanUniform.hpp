#pragma once

#include <Core.hpp>

#include "Vulkan.hpp"
#include "Renderer/Base/Buffer.hpp"

namespace ME::Render
{
	class MEAPI VulkanUniform final : public Uniform
	{
	public:
		VulkanUniform(const UniformSpecification& specification);
		VulkanUniform(const VulkanUniform&) = delete;
		VulkanUniform(VulkanUniform&&) = delete;
		~VulkanUniform() override;

	public:
		VulkanUniform& operator=(const VulkanUniform&) = delete;
		VulkanUniform& operator=(VulkanUniform&&) = delete;

	public:
		void Shutdown() override;

		void SetData(void* data, SIZE_T size, SIZE_T offset) override;
		void SetData(ME::Core::Memory::Reference<ME::Render::CommandBuffer> commandBuffer, void* data, SIZE_T size, SIZE_T offset) override;

		MappedBufferData Map() override;
		void Unmap() override;


        void Resize(SIZE_T size) override;

        void Clear() override;
        void Clear(ME::Core::Memory::Reference<ME::Render::CommandBuffer> commandBuffer) override;

		void Bind(ME::Core::Memory::Reference<CommandBuffer> commandBuffer, uint32 offset) override {}
		void Bind(ME::Core::Memory::Reference<CommandBuffer> commandBuffer, ME::Core::Memory::Reference<Pipeline> pipeline) override;

		/**
		 * Write buffer to a resource set
		 */
		void Write() override;

		/**
		 * Write buffer part to a resource set
		 * @param offset Offset of writable part
		 */
		void Write(SIZE_T offset) override;

		/**
		 * Write buffer part to a resource set
		 * @param offset Offset of writable part
		 * @param binding Custom binding
		 */
		void Write(SIZE_T offset, uint32 binding) override;

		/**
		 * Write buffer part to a resource set
		 * @param size Size of writable part
		 * @param offset Offset of writable part
		 */
		void Write(SIZE_T size, SIZE_T offset) override;

		/**
		 * Write buffer part to a resource set
		 * @param size Size of writable part
		 * @param offset Offset of writable part
		 * @param binding Custom binding
		 */
		void Write(SIZE_T size, SIZE_T offset, uint32 binding) override;

		void Barrier(ME::Core::Memory::Reference<CommandBuffer> commandBuffer, BarrierInfo src, BarrierInfo dst) override;

		inline void UpdateResourceSet(uint32 setIndex) override { m_ResourceIndex = setIndex; };
		inline uint32 GetResourceSet() const override { return m_ResourceIndex; };

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