#pragma once

#include <Core.hpp>

#include "Vulkan.hpp"
#include "Renderer/Base/Buffer.hpp"

namespace ME::Render
{
    class MEAPI VulkanIndirectBuffer final : public IndirectBuffer
	{
	public:
		VulkanIndirectBuffer(const IndirectBufferSpecification& specification);
		VulkanIndirectBuffer(const VulkanIndirectBuffer&) = delete;
		VulkanIndirectBuffer(VulkanIndirectBuffer&&) = delete;
		~VulkanIndirectBuffer() override;

    public:
		VulkanIndirectBuffer& operator=(const VulkanIndirectBuffer&) = delete;
		VulkanIndirectBuffer& operator=(VulkanIndirectBuffer&&) = delete;

	public:
		void Shutdown() override;

        void Clear() override;
		void Clear(ME::Core::Memory::Reference<ME::Render::CommandBuffer> commandBuffer) override;

        void Resize(SIZE_T size) override;

		void Bind(ME::Core::Memory::Reference<CommandBuffer> commandBuffer, uint32 offset) override {}
		void Bind(ME::Core::Memory::Reference<CommandBuffer> commandBuffer,
			ME::Core::Memory::Reference<Pipeline> pipeline) override;

		void Write() override;

		void Barrier(ME::Core::Memory::Reference<CommandBuffer> commandBuffer, BarrierInfo src, BarrierInfo dst) override;

		inline void UpdateResourceSet(uint32 setIndex) override { m_ResourceIndex = setIndex; }
		inline uint32 GetResourceSet() const override { return m_ResourceIndex; }

    public:
        inline VkBuffer GetBuffer() const { return m_Buffer; }

	private:
		void Init();
		VkResult CreateBuffer();

    private:
		VkBuffer m_Buffer;
		VmaAllocation m_Allocation;

	private:
		uint32 m_ResourceIndex;
	};
}