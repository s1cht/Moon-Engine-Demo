#pragma once

#include <Core.hpp>
#include <Core/Containers/String/BasicString.hpp>
#include <Core/Containers/String/StringView.hpp>

#include "Vulkan.hpp"
#include "Renderer/Base/Texture.h"

namespace ME::Render
{
	class MEAPI VulkanTexture2D : public Texture2D
	{
	public:
		VulkanTexture2D(const Texture2DSpecification& specification);
		VulkanTexture2D(VkImage image, const Texture2DSpecification& specification);

		~VulkanTexture2D() override;

	public:
		bool Loaded() const override { return m_Loaded; }
		uint32 GetSet() const override { return m_Set; }

		void LoadTexture(uint32 set) override;
		void UnloadTexture() override;

		void SetData(void* data, SIZE_T size) override;

		void Shutdown() override;

		inline ME::Core::StringView GetTexturePath() override { return m_Path; }
		inline ME::Core::StringView GetDebugName() override { return m_Specification.DebugName;  }

		inline ME::Render::Texture2DSpecification& GetSpecification() override { return m_Specification; }
		inline ME::Core::Math::Resolution2D<uint32> GetResolution() const override { return m_Specification.Resolution; };

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
		uint32 m_Set;
		bool m_Loaded;

	private:
		ME::Core::String m_Path;
		Texture2DSpecification m_Specification;

	};
}