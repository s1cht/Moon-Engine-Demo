#include "VulkanTexture.h"

#include "VulkanFunctions.h"
#include "VulkanMacros.hpp"
#include "VulkanRenderer.h"
#include "Renderer/RenderCommand.h"
#include "Renderer/RenderResourcesTracker.hpp"

namespace ME::Render
{
	ME::Core::Memory::Reference<Texture2D> Texture2D::CreateVulkanTexture(Texture2DSpecification& specification)
	{
		auto object = ME::Core::Memory::Reference<Texture2D>(new VulkanTexture2D(specification));
		RenderResourcesTracker::Get().AddItem(object);
		return object;
	}

	VulkanTexture2D::VulkanTexture2D(Texture2DSpecification& specification) : m_Specification(specification)
	{
		Init();
	}

	VulkanTexture2D::VulkanTexture2D(VkImage image, Texture2DSpecification& specification) : m_Specification(specification)
	{
		Init(image);
	}

	VulkanTexture2D::~VulkanTexture2D()
	{
		Shutdown();
	}

	void VulkanTexture2D::Bind()
	{
		
	}

	void VulkanTexture2D::Unbind()
	{
		
	}

	bool VulkanTexture2D::IsLoaded() const
	{
		return false;
	}

	void VulkanTexture2D::SetData(void* data, SIZE_T size)
	{
	}

	void* VulkanTexture2D::GetRawData()
	{
		return nullptr;

	}

	SIZE_T VulkanTexture2D::GetRawDataSize()
	{
		return 0;
	}

	void VulkanTexture2D::Shutdown()
	{
		VulkanRenderer* render = Render::RenderCommand::Get()->As<VulkanRenderer>();

		if (m_ImageView != nullptr)
		{
			vkDestroyImageView(render->GetDevice(), m_ImageView, nullptr);
			m_ImageView = nullptr;
		}

		if (m_Image != nullptr && m_Specification.bOwnsImage == false)
		{
			vkDestroyImage(render->GetDevice(), m_Image, nullptr);
			m_Image = nullptr;
		}
	}

	void VulkanTexture2D::Init()
	{
		//VkResult result;

	}
	
	void VulkanTexture2D::Init(VkImage image)
	{
		VkResult result;

		m_Image = image;
		m_ImageFormat = ME::Render::ConvertFormatVulkan(m_Specification.Format);

		result = CreateImageView();
		if (ME_VK_FAILED(result))
		{
			ME_ASSERT(false, TEXT("Vulkan Texture2D: failed to create image view!: Error: {0}"), (uint32)result);
			return;
		}
	}

	VkResult VulkanTexture2D::CreateImage()
	{
		VulkanRenderer* render = Render::RenderCommand::Get()->As<VulkanRenderer>();
		uint32 graphicsQueue = static_cast<uint32_t>(render->GetGraphicsQueueFamily());

		VkImageCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		createInfo.imageType = VK_IMAGE_TYPE_2D;
		createInfo.format = ME::Render::ConvertFormatVulkan(m_Specification.Format);
		createInfo.extent.width = m_Specification.Resolution.x;
		createInfo.extent.height = m_Specification.Resolution.y;
		createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		createInfo.mipLevels = 1;
		createInfo.queueFamilyIndexCount = 1;
		createInfo.pQueueFamilyIndices = &graphicsQueue;
		createInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		createInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	//	createInfo.usage = Pawn::Render::ConvertImageUsageFlagsVulkan(m_Specification.Usage);
		createInfo.arrayLayers = 1;

		return vkCreateImage(render->GetDevice(), &createInfo, nullptr, &m_Image);
	}

	VkResult VulkanTexture2D::CreateImageView()
	{
		VulkanRenderer* render = Render::RenderCommand::Get()->As<VulkanRenderer>();

		VkImageViewCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = m_Image;
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = m_ImageFormat;
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		if (m_Specification.IsStencil || m_Specification.IsDepth)
		{
			createInfo.subresourceRange.aspectMask = 0;
			if (m_Specification.IsStencil)
				createInfo.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
			if (m_Specification.IsDepth)
				createInfo.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_DEPTH_BIT;
		}

		return vkCreateImageView(render->GetDevice(), &createInfo, nullptr, &m_ImageView);
	}

}
