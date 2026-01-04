#include "VulkanFramebuffer.hpp"

#include "VulkanRenderAPI.hpp"
#include "VulkanRenderPass.hpp"
#include "VulkanTexture2D.hpp"
#include "Renderer/RenderCommand.hpp"


namespace ME::Render
{
	ME::Core::Memory::Reference<Render::Framebuffer>  Framebuffer::CreateVulkan(const FramebufferSpecification& specification)
	{
		auto object = ME::Core::Memory::MakeReference<VulkanFramebuffer>(specification);
		return object;
	}

	VulkanFramebuffer::VulkanFramebuffer(const FramebufferSpecification& specification)
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
			vkDestroyFramebuffer(Render::RenderCommand::Get()->As<VulkanRenderAPI>()->GetDevice(), m_Buffer, nullptr);
			m_Buffer = nullptr;
		}
	}

	void VulkanFramebuffer::Init()
	{
		m_DebugName = m_Specification.DebugName;

        VulkanRenderAPI* render = Render::RenderCommand::Get()->As<VulkanRenderAPI>();
		ME::Core::Array<VkImageView> attachments;

		for (auto attachment : m_Specification.Attachments)
			attachments.EmplaceBack(attachment->As<VulkanTexture2D>()->GetImageView());

		VkFramebufferCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		createInfo.renderPass = m_Specification.RenderPass->As<VulkanRenderPass>()->GetRenderPass();
		createInfo.width = m_Specification.Resolution.x;
		createInfo.height = m_Specification.Resolution.y;
		createInfo.attachmentCount = static_cast<uint32>(m_Specification.Attachments.Size());
		createInfo.pAttachments = attachments.Data();
		createInfo.layers = m_Specification.Layers;

		VkResult result = vkCreateFramebuffer(render->GetDevice(), &createInfo, nullptr, &m_Buffer);
		if (ME_VK_FAILED(result))
		{
		    ME_ASSERT(false, ME_VK_LOG_OUTPUT_FORMAT("Framebuffer", "Failed to create framebuffer! Error code: {1}"),
			    m_DebugName, static_cast<uint32>(result));
			Shutdown();
			return;
		}
		render->NameVulkanObject(m_DebugName, ME_VK_TO_UINT_HANDLE(m_Buffer), VK_OBJECT_TYPE_FRAMEBUFFER);
	}
}
