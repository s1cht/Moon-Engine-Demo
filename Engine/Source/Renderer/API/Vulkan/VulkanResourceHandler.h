#pragma once

#include <Core.hpp>

#include "Vulkan.hpp"
#include "Renderer/Base/RenderObject.hpp"
#include "Renderer/Base/ResourceHandler.h"
#include "Renderer/Base/Shader.h"

namespace ME::Render
{
	class StorageBuffer;
	class Uniform;
}

namespace ME::Render
{
	struct DescriptorSetLayoutPair
	{
		VkDescriptorSetLayout vkLayout;
		ResourceLayout Layout;
	};

	class MOON_API VulkanResourceHandler final : public ResourceHandler
	{
	public:
		VulkanResourceHandler(uint32 bufferCount);
		~VulkanResourceHandler() override;

	public:
		void Shutdown() override;
		uint32 CreateLayout(const ME::Render::ResourceLayout& layout) override;
		uint32 CreateResourceSet(uint32 layout) override;
		uint32 CreateResourceSet(const ME::Render::ResourceLayout& layout) override;

		uint32 GetResourceLayoutIndex(const ME::Render::ResourceLayout& layout) override;

	public:
		void WriteResource(ME::Core::Memory::Reference<ME::Render::Uniform> buffer) override;
		void WriteResource(ME::Core::Memory::Reference<ME::Render::StorageBuffer> buffer) override;

		void BindResourceSet(ME::Core::Memory::Reference<Render::CommandBuffer> commandBuffer, ME::Core::Memory::Reference<Render::Pipeline> pipeline,
			uint32 set, uint32 setIndex) override;

	public:
		VkDescriptorPool GetDescriptorPool() const { return m_DescriptorPool; }

	public:
		ME::Core::Containers::Array<VkDescriptorSetLayout> GetDescriptorSetLayouts(ME::Core::Containers::Array<uint32> layouts) const;
		void CreateDescriptorSet(uint32 layout, VkDescriptorSet* descriptor) const;
		void DestroyDescriptor(VkDescriptorSet* descriptor) const;

	private:
		void Init();
		int32 GetLayoutIndex(const ME::Render::ResourceLayout& layout) const;

	private:
		static int32 CreateDescriptorSetLayout(DescriptorSetLayoutPair& layout);

	private:
		VkDescriptorPool m_DescriptorPool;
		ME::Core::Containers::Array<VkDescriptorSet> m_Sets;

	private:
		ME::Core::Containers::Array<DescriptorSetLayoutPair> m_Layouts;
		uint32 m_BufferCount;
	};
}
