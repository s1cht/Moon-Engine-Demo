#include "VulkanFramebuffer.h"
#include "VulkanRenderPass.h"
#include "VulkanMacros.hpp"
#include "Renderer/RenderCommand.h"

namespace Pawn::Render
{
	Framebuffer* Framebuffer::CreateVulkanFramebuffer(FramebufferSpecification& specification)
	{
		return new VulkanFramebuffer(specification);
	}

	VulkanFramebuffer::VulkanFramebuffer(FramebufferSpecification& specification)
	{
		Init(specification);
	}

	VulkanFramebuffer::~VulkanFramebuffer()
	{
		VulkanRenderer* render = static_cast<VulkanRenderer*>(Render::RenderCommand::Get());
		if (m_Buffer != nullptr)
		{
			vkDestroyFramebuffer(render->GetDevice(), m_Buffer, nullptr);
			m_Buffer = nullptr;
		}
	}

	void VulkanFramebuffer::Init(FramebufferSpecification& specification)
	{	
		VkResult result;
		VulkanRenderer* render = static_cast<VulkanRenderer*>(Render::RenderCommand::Get());
		Pawn::Core::Containers::Array<VkImageView> attachments;

		for (auto attachment : specification.Attachments)
			attachments.EmplaceBack(static_cast<VulkanTexture2D*>(attachment)->GetImageView());

		VkFramebufferCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		createInfo.renderPass = static_cast<VulkanRenderPass*>(specification.RenderPass)->GetRenderPass();
		createInfo.width = specification.Resolution.x;
		createInfo.height = specification.Resolution.y;
		createInfo.attachmentCount = (uint32)specification.Attachments.GetSize();
		createInfo.pAttachments = attachments.Data();
		createInfo.layers = specification.Layers;

		result = vkCreateFramebuffer(render->GetDevice(), &createInfo, nullptr, &m_Buffer);
		if (PE_VK_FAILED(result))
			PE_ASSERT(false, TEXT("Vulkan framebuffer: framebuffer creation failed! Error: {0}"), (uint32)result);
	}

}