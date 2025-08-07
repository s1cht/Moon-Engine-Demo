#include "VulkanFramebuffer.h"
#include "VulkanRenderPass.h"
#include "VulkanMacros.hpp"
#include "Renderer/RenderCommand.h"
#include "Renderer/RenderResourcesTracker.hpp"

namespace ME::Render
{
	ME::Core::Memory::Reference<Render::Framebuffer>  Framebuffer::CreateVulkanFramebuffer(FramebufferSpecification& specification)
	{
		auto object = ME::Core::Memory::Reference<Render::Framebuffer>(new VulkanFramebuffer(specification));
		RenderResourcesTracker::Get().AddItem(object);
		return object;
	}

	VulkanFramebuffer::VulkanFramebuffer(FramebufferSpecification& specification)
		: m_Specification(specification)
	{
		Init();
	}

	VulkanFramebuffer::~VulkanFramebuffer()
	{
		Shutdown();
	}

	void VulkanFramebuffer::Shutdown()
	{
		if (m_Buffer != nullptr)
		{
			vkDestroyFramebuffer(Render::RenderCommand::Get()->As<VulkanRenderer>()->GetDevice(), m_Buffer, nullptr);
			m_Buffer = nullptr;
		}
	}

	void VulkanFramebuffer::Init()
	{	
		VkResult result;
		VulkanRenderer* render = Render::RenderCommand::Get()->As<VulkanRenderer>();
		ME::Core::Containers::Array<VkImageView> attachments;

		for (auto attachment : m_Specification.Attachments)
			attachments.EmplaceBack(attachment->As<VulkanTexture2D>()->GetImageView());

		VkFramebufferCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		createInfo.renderPass = m_Specification.RenderPass->As<VulkanRenderPass>()->GetRenderPass();
		createInfo.width = m_Specification.Resolution.x;
		createInfo.height = m_Specification.Resolution.y;
		createInfo.attachmentCount = (uint32)m_Specification.Attachments.GetSize();
		createInfo.pAttachments = attachments.Data();
		createInfo.layers = m_Specification.Layers;

		result = vkCreateFramebuffer(render->GetDevice(), &createInfo, nullptr, &m_Buffer);
		if (ME_VK_FAILED(result))
			ME_ASSERT(false, TEXT("Vulkan framebuffer: framebuffer creation failed! Error: {0}"), (uint32)result);
	}

}
