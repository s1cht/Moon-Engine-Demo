#pragma once
#include <Core.hpp>

#include "Vulkan.hpp"
#include "Renderer/Base/RenderCore.hpp"
#include "Renderer/Base/ResourceHandler.hpp"
#include "Renderer/Base/Shader.hpp"
#include "Renderer/Base/Texture.hpp"

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

	class MEAPI VulkanResourceHandler final : public ResourceHandler
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
		void WriteResource(ME::Core::Memory::Reference<ME::Render::IndirectBuffer> buffer) override;
		void WriteResource(ME::Core::Memory::Reference<ME::Render::VertexBuffer> buffer) override;
		void WriteResource(ME::Core::Memory::Reference<ME::Render::IndexBuffer> buffer) override;
		void WriteResource(ME::Core::Memory::Reference<ME::Render::Texture1D> texture) override;
		void WriteResource(ME::Core::Memory::Reference<ME::Render::Texture2D> texture) override;
		void WriteResource(ME::Core::Memory::Reference<ME::Render::Texture3D> texture) override;

		void BindResourceSet(ME::Core::Memory::Reference<Render::CommandBuffer> commandBuffer, ME::Core::Memory::Reference<Render::Pipeline> pipeline,
			uint32 set, uint32 setIndex) override;

	    void BufferBarrier(ME::Core::Memory::Reference<Render::CommandBuffer> commandBuffer,
			const ME::Core::Memory::Reference<Render::VertexBuffer>& buffer, BarrierInfo src,
			BarrierInfo dst) override;
		void BufferBarrier(ME::Core::Memory::Reference<Render::CommandBuffer> commandBuffer,
			const ME::Core::Memory::Reference<Render::IndexBuffer>& buffer, BarrierInfo src,
			BarrierInfo dst) override;
		void BufferBarrier(ME::Core::Memory::Reference<Render::CommandBuffer> commandBuffer,
			const ME::Core::Memory::Reference<Render::IndirectBuffer>& buffer, BarrierInfo src,
			BarrierInfo dst) override;
		void BufferBarrier(ME::Core::Memory::Reference<Render::CommandBuffer> commandBuffer,
			const ME::Core::Memory::Reference<Render::StorageBuffer>& buffer, BarrierInfo src,
			BarrierInfo dst) override;
		void BufferBarrier(ME::Core::Memory::Reference<Render::CommandBuffer> commandBuffer,
			const ME::Core::Memory::Reference<Render::Uniform>& buffer, BarrierInfo src,
			BarrierInfo dst) override;

		void TextureBarrier(ME::Core::Memory::Reference<Render::CommandBuffer> commandBuffer,
			const ME::Core::Memory::Reference<Render::Texture1D>& texture, BarrierInfo src,
			BarrierInfo dst) override;
		void TextureBarrier(ME::Core::Memory::Reference<Render::CommandBuffer> commandBuffer,
			const ME::Core::Memory::Reference<Render::Texture2D>& texture, BarrierInfo src,
			BarrierInfo dst) override;
		void TextureBarrier(ME::Core::Memory::Reference<Render::CommandBuffer> commandBuffer,
			const ME::Core::Memory::Reference<Render::Texture3D>& texture, BarrierInfo src,
			BarrierInfo dst) override;

	public:
		void WriteResource(VulkanTexture1D* texture, uint32 binding);
		void WriteResource(VulkanTexture2D* texture, uint32 binding);
		void WriteResource(VulkanTexture3D* texture, uint32 binding);
		void WriteResource(VulkanUniform* buffer, SIZE_T size, SIZE_T offset, uint32 binding);
		void WriteResource(VulkanIndexBuffer* buffer, SIZE_T size, SIZE_T offset, uint32 binding);
		void WriteResource(VulkanVertexBuffer* buffer, SIZE_T size, SIZE_T offset, uint32 binding);
		void WriteResource(VulkanIndirectBuffer* buffer, SIZE_T size, SIZE_T offset, uint32 binding);
		void WriteResource(VulkanStorageBuffer* buffer, SIZE_T size, SIZE_T offset, uint32 binding);

		void BufferBarrier(ME::Core::Memory::Reference<Render::CommandBuffer> commandBuffer,
			VkBuffer buffer, BarrierInfo src,
			BarrierInfo dst);
		void TextureBarrier(ME::Core::Memory::Reference<Render::CommandBuffer> commandBuffer,
			VkImage image, const TextureSpecification& specs, BarrierInfo src,
			BarrierInfo dst);

	public:
		VkDescriptorPool GetDescriptorPool() const { return m_DescriptorPool; }

	public:
		ME::Core::Array<VkDescriptorSetLayout> GetDescriptorSetLayouts(ME::Core::Array<uint32> layouts) const;
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
		ME::Core::Array<VulkanResourceSet> m_Sets;
		ME::Core::Array<VulkanTextureSet> m_TextureSets;

	private:
		ME::Core::Array<DescriptorSetLayoutPair> m_Layouts;
		uint32 m_BufferCount;
	};
}
