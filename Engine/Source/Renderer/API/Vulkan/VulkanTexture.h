#pragma once

#include <Core.hpp>
#include <vulkan/vulkan.hpp>

#include "Core/Containers/Array.hpp"
#include "Core/Containers/String/BasicString.hpp"
#include "Core/Containers/String/StringView.hpp"
#include "Renderer/Base/Texture.h"

namespace ME::Render
{
	class MOON_API VulkanTexture2D : public Texture2D
	{
	public:
		VulkanTexture2D(Texture2DSpecification& specification);
		VulkanTexture2D(VkImage image, Texture2DSpecification& specification);

		~VulkanTexture2D() override;

	public:
		void Bind() override;
		void Unbind() override;

		bool IsLoaded() const override;

		void SetData(void* data, SIZE_T size) override;

		void* GetRawData() override;
		SIZE_T GetRawDataSize() override;

		void Shutdown() override;

		inline ME::Core::Containers::StringView GetTexturePath() override { return m_Path; }
		inline ME::Core::Containers::AnsiStringView GetDebugName() override { return m_Specification.DebugName;  }

		inline ME::Render::Texture2DSpecification& GetSpecification() override { return m_Specification; }
		inline ME::Core::Math::Resolution2D<uint32> GetResolution() const override { return m_Specification.Resolution; };

	public:
		inline VkImage GetImage() { return m_Image; }
		inline VkImageView GetImageView() { return m_ImageView; }

	private:
		void Init();
		void Init(VkImage image);

		VkResult CreateImage();
		VkResult CreateImageView();

	private:
		VkImage m_Image;
		VkImageView m_ImageView;
		VkFormat m_ImageFormat;

	private:
		ME::Core::Containers::String m_Path;
		Texture2DSpecification m_Specification;

	};
}