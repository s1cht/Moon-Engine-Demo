#pragma once

#include <Core.hpp>

#include "Vulkan.hpp"
#include "Renderer/Base/Buffer.h"

namespace ME::Render
{
	class MOON_API VulkanUniform final : public Uniform
	{
	public:
		VulkanUniform(const UniformSpecification& specification);
		~VulkanUniform() override;

	public:
		void Shutdown() override;

		void SetData(void* data, SIZE_T size) override;
		void SetData(ME::Core::Memory::Reference<ME::Render::CommandBuffer> commandBuffer, void* data, SIZE_T size) override;

		inline uint32 GetResourceSet() const override { return m_DescriptorIndex; }
		inline const UniformSpecification& GetSpecification() const override { return m_Specification; }

	public:
		inline VkBuffer GetBuffer() { return m_Buffer; }

	private:
		void Init();
		VkResult CreateBuffer();
		VkResult CreateStagingBuffer();

	private:
		void SetDataVRAM(const ME::Core::Memory::Reference<ME::Render::CommandBuffer>& commandBuffer, const void* data, SIZE_T size);
		void SetDataRAM(ME::Core::Memory::Reference<ME::Render::CommandBuffer> commandBuffer, void* data, SIZE_T size);

	private:
		VkBuffer m_Buffer;
		VmaAllocation m_Allocation;

		VkBuffer m_StagingBuffer;
		VmaAllocation m_StagingAllocation;

	private:
		void* m_MappedData;
		int32 m_DescriptorIndex;

		UniformSpecification m_Specification;
	};
}