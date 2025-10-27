#pragma once

#include <Core.hpp>

#include "Vulkan.hpp"
#include "Renderer/Base/Buffer.h"

namespace ME::Render
{
	class MOON_API VulkanVertexBuffer final : public VertexBuffer
	{
	public:
		VulkanVertexBuffer(const VertexBufferSpecification& specification);
		~VulkanVertexBuffer() override;

	public:
		void Bind(ME::Core::Memory::Reference<ME::Render::CommandBuffer> commandBuffer) override;
		void Unbind() override;

		void SetData(void* data, SIZE_T size) override;
		void SetData(ME::Core::Memory::Reference<ME::Render::CommandBuffer> commandBuffer, void* data, SIZE_T size) override;

		void Shutdown() override;
	private:
		void Init(const VertexBufferSpecification& specification);
		VkResult CreateBuffer();
		VkResult CreateStagingBuffer();

	private:
		VkBuffer m_Buffer;
		VmaAllocation m_Allocation;

		VkBuffer m_StagingBuffer;
		VmaAllocation m_StagingAllocation;

		SIZE_T m_Size;
		const ME::Core::Containers::String m_DebugName;

	};
}