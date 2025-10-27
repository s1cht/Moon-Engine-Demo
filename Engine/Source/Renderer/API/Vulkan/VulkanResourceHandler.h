#pragma once

#include <Core.hpp>

#include "Vulkan.hpp"
#include "Renderer/Base/RenderObject.hpp"
#include "Renderer/Base/ResourceHandler.h"
#include "Renderer/Base/Shader.h"
#include "Renderer/Base/Texture.h"

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

	struct VulkanResourceSet
	{
		VkDescriptorSet Set;
		uint32 Layout;
		bool InUse = false;
	};

	struct VulkanTextureSet
	{
		VkDescriptorSet Set;
		uint32 References = 0;
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

		uint32 GetResourceSet(uint32 layout) override;
		uint32 GetResourceSet(const ME::Render::ResourceLayout& layout) override;

		uint32 GetTextureSet(uint32 layout) override;
		uint32 GetTextureSet(const ME::Render::ResourceLayout& layout) override;

		bool IncrementTextureSetReference(uint32 set) override;
		void DecrementTextureSetReference(uint32 set) override;

		uint32 GetResourceLayoutIndex(const ME::Render::ResourceLayout& layout) override;

	public:
		void WriteResource(ME::Core::Memory::Reference<ME::Render::Uniform> buffer) override;
		void WriteResource(ME::Core::Memory::Reference<ME::Render::StorageBuffer> buffer) override;
		void QueueTexture(ME::Core::Memory::Reference<ME::Render::Texture1D> texture) override;
		void QueueTexture(ME::Core::Memory::Reference<ME::Render::Texture2D> texture) override;
		void QueueTexture(ME::Core::Memory::Reference<ME::Render::Texture3D> texture) override;
		void WriteTexture() override;

		void BindResourceSet(ME::Core::Memory::Reference<Render::CommandBuffer> commandBuffer, ME::Core::Memory::Reference<Render::Pipeline> pipeline,
			uint32 set, uint32 setIndex) override;
		void BindTextureSet(ME::Core::Memory::Reference<Render::CommandBuffer> commandBuffer, ME::Core::Memory::Reference<Render::Pipeline> pipeline,
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
		uint32 FindUnusedTextureSet() const;

	private:
		static int32 CreateDescriptorSetLayout(DescriptorSetLayoutPair& layout);

	private:
		VkDescriptorPool m_DescriptorPool;
		ME::Core::Containers::Array<VulkanResourceSet> m_Sets;
		ME::Core::Containers::Array<VulkanTextureSet> m_TextureSets;

	private:
		ME::Core::Containers::Array<DescriptorSetLayoutPair> m_Layouts;
		uint32 m_BufferCount;

	private:
		ME::Core::Containers::Array<VkDescriptorImageInfo> m_QueuedImages;
		ME::Core::Containers::Array<VkDescriptorImageInfo> m_QueuedSamplers;
		bool m_WritingTextures;
		uint32 m_CurrentTextureSet;
		
	};
}
