#pragma once

#include <Core.hpp>

#include "Vulkan.hpp"
#include "Renderer/Base/Buffer.hpp"

namespace ME::Render
{
	class MEAPI VulkanVertexBuffer final : public VertexBuffer
	{
	public:
		VulkanVertexBuffer(const VertexBufferSpecification& specification);
		VulkanVertexBuffer(const VulkanVertexBuffer&) = delete;
		VulkanVertexBuffer(VulkanVertexBuffer&&) = delete;
	    ~VulkanVertexBuffer() override;

	public:
		VulkanVertexBuffer& operator=(const VulkanVertexBuffer&) = delete;
		VulkanVertexBuffer& operator=(VulkanVertexBuffer&&) = delete;

	public:
		void SetData(void* data, SIZE_T size, SIZE_T offset) override;
		void SetData(ME::Core::Memory::Reference<ME::Render::CommandBuffer> commandBuffer, void* data, SIZE_T size, SIZE_T offset) override;

		void Resize(SIZE_T size) override;

		void Clear() override;
		void Clear(ME::Core::Memory::Reference<ME::Render::CommandBuffer> commandBuffer) override;

		void Shutdown() override;

		inline void UpdateResourceSet(uint32 setIndex) override { m_ResourceIndex = setIndex; };
		inline uint32 GetResourceSet() const override { return m_ResourceIndex; }

	public:
		inline VkBuffer GetBuffer() const { return m_Buffer; }

	private:
		void Init(const VertexBufferSpecification& specification);
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