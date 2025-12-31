#pragma once
#include <Core.hpp>
#include <Core/Containers/String/BasicString.hpp>
#include <Core/Containers/String/StringView.hpp>

#include "Vulkan.hpp"
#include "Renderer/Base/Texture.hpp"

namespace ME::Render
{
	class MEAPI VulkanTexture2D final : public Texture2D
	{
	public:
		VulkanTexture2D(const Texture2DSpecification& specification);
		VulkanTexture2D(VkImage image, const Texture2DSpecification& specification);

		~VulkanTexture2D() override;

	public:
		void Shutdown() override;

		void SetData(void* data, SIZE_T size) override;
		inline ME::Core::StringView GetTexturePath() override { return m_Path; }
		inline ME::Core::Math::Resolution2D<uint32> GetResolution() const override { return m_Specification.Resolution; }

		void Bind(ME::Core::Memory::Reference<CommandBuffer> commandBuffer, uint32 offset) override {}
		void Bind(ME::Core::Memory::Reference<CommandBuffer> commandBuffer,
			ME::Core::Memory::Reference<Pipeline> pipeline) override;

		void Barrier(ME::Core::Memory::Reference<CommandBuffer> commandBuffer, BarrierInfo src, BarrierInfo dst) override;

		void Write() override;

	public:
		inline VkImage GetImage() { return m_Image; }
		inline VkImageView GetImageView() { return m_ImageView; }
		inline VkSampler GetSampler() { return m_Sampler; }

	private:
		void Init();
		void Init(VkImage image);

		VkResult CreateImage();
		VkResult CreateImageView();
		VkResult CreateSampler();
		VkResult UpdateImage(void* data, SIZE_T size);

    private:
		VkImage m_Image;
		VkImageView m_ImageView;
		VkSampler m_Sampler;
		VmaAllocation m_Allocation;

		VkFormat m_ImageFormat;

	private:
		ME::Core::String m_Path;
	};
}