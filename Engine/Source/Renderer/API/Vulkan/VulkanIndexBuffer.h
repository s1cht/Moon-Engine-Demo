#pragma once

#include <Core.hpp>

#include "Vulkan.hpp"
#include "Renderer/Base/Buffer.h"

namespace ME::Render
{
	class MOON_API VulkanIndexBuffer final : public IndexBuffer
	{
	public:
		VulkanIndexBuffer(const IndexBufferSpecification& specification);
		~VulkanIndexBuffer() override;

	public:
		void Bind(ME::Core::Memory::Reference<ME::Render::CommandBuffer> commandBuffer) override;
		void Unbind() override;

		void SetData(ME::Core::Memory::Reference<ME::Render::CommandBuffer> commandBuffer, void* data, SIZE_T size) override;
		void SetData(ME::Core::Memory::Reference<ME::Render::CommandBuffer> commandBuffer, int32* indices, SIZE_T indexCount) override;

		uint32 GetCount() const override { return m_IndexCount; }

		void Shutdown() override;

	private:
		void Init(const IndexBufferSpecification& specification);
		VkResult CreateBuffer();
		VkResult CreateStagingBuffer();

	private:
		VkBuffer m_Buffer;
		VmaAllocation m_Allocation;

		VkBuffer m_StagingBuffer;
		VmaAllocation m_StagingAllocation;

		int32 m_IndexCount;
		const ME::Core::Containers::String m_DebugName;

	};
}