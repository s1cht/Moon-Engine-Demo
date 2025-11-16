#pragma once

#include <Core.hpp>

#include "Vulkan.hpp"
#include "Renderer/Base/Buffer.hpp"

namespace ME::Render
{
	class MEAPI VulkanIndexBuffer final : public IndexBuffer
	{
	public:
		VulkanIndexBuffer(const IndexBufferSpecification& specification);
		VulkanIndexBuffer(const VulkanIndexBuffer&) = delete;
		VulkanIndexBuffer(VulkanIndexBuffer&&) = delete;
	    ~VulkanIndexBuffer() override;

	public:
		VulkanIndexBuffer& operator=(const VulkanIndexBuffer&) = delete;
		VulkanIndexBuffer& operator=(VulkanIndexBuffer&&) = delete;

	public:
		void SetData(uint32* indices, SIZE_T indexCount, SIZE_T offset) override;
		void SetData(ME::Core::Memory::Reference<ME::Render::CommandBuffer> commandBuffer, uint32* indices, SIZE_T indexCount, SIZE_T offset) override;

	    void Resize(SIZE_T indexCount) override;

	    void Clear() override;
		void Clear(ME::Core::Memory::Reference<ME::Render::CommandBuffer> commandBuffer) override;

		void Shutdown() override;

		inline SIZE_T GetCount() const override { return m_Specification.IndexCount; }

		inline void UpdateResourceSet(uint32 setIndex) override { m_ResourceIndex = setIndex; };
		inline uint32 GetResourceSet() const override { return m_ResourceIndex; }

	public:
		inline VkBuffer GetBuffer() const { return m_Buffer; }

	private:
		void Init(const IndexBufferSpecification& specification);
		VkResult CreateBuffer();
		VkResult CreateStagingBuffer();

    private:
		VkBuffer m_Buffer;
		VmaAllocation m_Allocation;

		VkBuffer m_StagingBuffer;
		VmaAllocation m_StagingAllocation;

		uint32 m_ResourceIndex;
	};
}