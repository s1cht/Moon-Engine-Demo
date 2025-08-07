#include "VulkanRenderPass.h"

#include "VulkanCommandBuffer.h"
#include "VulkanFunctions.h"
#include "VulkanMacros.hpp"
#include "VulkanFramebuffer.h"
#include "Renderer/RenderCommand.h"
#include "Renderer/RenderResourcesTracker.hpp"

namespace ME::Render
{
	Core::Memory::Reference<Render::RenderPass> RenderPass::CreateVulkanRenderPass(RenderPassSpecification& specification)
	{
		auto object = Core::Memory::Reference<Render::RenderPass>(new VulkanRenderPass(specification));
		RenderResourcesTracker::Get().AddItem(object);
		return object;
	}

	VulkanRenderPass::VulkanRenderPass(RenderPassSpecification& specification)
	{
		Init(specification);
	}

	VulkanRenderPass::~VulkanRenderPass()
	{
		Shutdown();
	}

	void VulkanRenderPass::Shutdown()
	{
		if (m_Pass != nullptr)
		{
			vkDestroyRenderPass(Render::RenderCommand::Get()->As<VulkanRenderer>()->GetDevice(), m_Pass, nullptr);
			m_Pass = nullptr;
		}
	}

	void VulkanRenderPass::Begin(CommandBuffer* buffer, RenderPassBeginInfo& beginInfo)
	{
		VulkanCommandBuffer* cmdBuf = buffer->As<VulkanCommandBuffer>();

		VkRenderPassBeginInfo vkBeginInfo = {};
		vkBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		vkBeginInfo.renderPass = m_Pass;
		vkBeginInfo.clearValueCount = (uint32)beginInfo.ClearValues.GetSize();
		vkBeginInfo.pClearValues = reinterpret_cast<const VkClearValue*>(beginInfo.ClearValues.Data());
		vkBeginInfo.framebuffer = beginInfo.Framebuffer->As<VulkanFramebuffer>()->GetFramebuffer();
		vkBeginInfo.renderArea.offset.x = beginInfo.RenderArea.Offset.x;
		vkBeginInfo.renderArea.offset.y = beginInfo.RenderArea.Offset.y;
		vkBeginInfo.renderArea.extent.height = beginInfo.RenderArea.Extent.x;
		vkBeginInfo.renderArea.extent.width = beginInfo.RenderArea.Extent.y;

		vkCmdBeginRenderPass(cmdBuf->GetBuffer(), &vkBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
	}

	void VulkanRenderPass::End(CommandBuffer* buffer)
	{
		vkCmdEndRenderPass(buffer->As<VulkanCommandBuffer>()->GetBuffer());
	}

	void VulkanRenderPass::Init(RenderPassSpecification& specification)
	{
		VkResult result;
		VulkanRenderer* render = Render::RenderCommand::Get()->As<VulkanRenderer>();

		ME::Core::Containers::Array<VkAttachmentDescription> attachmentDescriptions;
		ME::Core::Containers::Array<VkAttachmentReference> globalAttachmentReferences;

		for (auto& attachment : specification.AttachmentSpecs)
		{
			VkAttachmentDescription desc{};
			desc.format = ME::Render::ConvertFormatVulkan(attachment.AttachmentFormat);
			desc.initialLayout = ME::Render::ConvertImageLayoutVulkan(attachment.InitialLayout);
			desc.finalLayout = ME::Render::ConvertImageLayoutVulkan(attachment.FinalLayout);
			desc.loadOp = attachment.IsStencil ? VK_ATTACHMENT_LOAD_OP_DONT_CARE : ME::Render::ConvertAttachmentLoadOperationVulkan(attachment.LoadOp);
			desc.stencilLoadOp = attachment.IsStencil ? VK_ATTACHMENT_LOAD_OP_DONT_CARE : ME::Render::ConvertAttachmentLoadOperationVulkan(attachment.LoadOp);
			desc.storeOp = attachment.IsStencil ? VK_ATTACHMENT_STORE_OP_DONT_CARE : ME::Render::ConvertAttachmentStoreOperationVulkan(attachment.StoreOp);
			desc.stencilStoreOp = attachment.IsStencil ? VK_ATTACHMENT_STORE_OP_DONT_CARE : ME::Render::ConvertAttachmentStoreOperationVulkan(attachment.StoreOp);
			desc.samples = VK_SAMPLE_COUNT_1_BIT;
			desc.flags = 0;

			attachmentDescriptions.EmplaceBack(desc);

			VkAttachmentReference ref{};
			ref.attachment = static_cast<uint32_t>(globalAttachmentReferences.GetSize());
			ref.layout = (attachment.IsDepth || attachment.IsStencil)
				? VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
				: VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

			globalAttachmentReferences.EmplaceBack(ref);
		}

		ME::Core::Containers::Array<ME::Core::Containers::Array<VkAttachmentReference>> colorAttachmentRefsList;
		ME::Core::Containers::Array<ME::Core::Containers::Array<VkAttachmentReference>> inputAttachmentRefsList;
		ME::Core::Containers::Array<VkAttachmentReference> depthStencilAttachmentRefs;

		ME::Core::Containers::Array<VkSubpassDescription> subpassDescriptions;

		for (const auto& subpass : specification.SubpassSpecs)
		{
			colorAttachmentRefsList.EmplaceBack();
			inputAttachmentRefsList.EmplaceBack();

			for (auto refIndex : subpass.ColorAttachmentRefs)
			{
				colorAttachmentRefsList.Back().PushBack(globalAttachmentReferences[refIndex]);
			}

			for (auto refIndex : subpass.InputAttachmentRefs)
			{
				inputAttachmentRefsList.Back().PushBack(globalAttachmentReferences[refIndex]);
			}

			VkAttachmentReference depthRef = {};
			if (subpass.DepthStencilAttachmentRef != ~0u)
				depthRef = globalAttachmentReferences[subpass.DepthStencilAttachmentRef];

			depthStencilAttachmentRefs.PushBack(depthRef);

			VkSubpassDescription subpassDesc{};
			subpassDesc.pipelineBindPoint = ME::Render::ConvertPipelineBindPointVulkan(subpass.PipelineBindPoint);
			subpassDesc.colorAttachmentCount = (uint32)colorAttachmentRefsList.Back().GetSize();
			subpassDesc.pColorAttachments = colorAttachmentRefsList.Back().Data();
			subpassDesc.inputAttachmentCount = (uint32)inputAttachmentRefsList.Back().GetSize();
			subpassDesc.pInputAttachments = inputAttachmentRefsList.Back().Data();
			subpassDesc.pDepthStencilAttachment = (subpass.DepthStencilAttachmentRef != ~0u) ? &depthStencilAttachmentRefs.Back() : nullptr;

			subpassDescriptions.EmplaceBack(subpassDesc);
		}

		ME::Core::Containers::Array<VkSubpassDependency> subpassDependencies;
		for (auto& dep : specification.SubpassDependencies)
		{
			VkSubpassDependency vkDep = {};
			vkDep.srcSubpass = dep.SubpassSrc;
			vkDep.dstSubpass = dep.SubpassDst;
			vkDep.srcStageMask = ME::Render::ConvertPipelineStageFlagsVulkan(dep.PipelineStageFlagsSrc);
			vkDep.dstStageMask = ME::Render::ConvertPipelineStageFlagsVulkan(dep.PipelineStageFlagsDst);
			vkDep.srcAccessMask = ME::Render::ConvertAccessFlagsVulkan(dep.AccessFlagsSrc);
			vkDep.dstAccessMask = ME::Render::ConvertAccessFlagsVulkan(dep.AccessFlagsDst);
			vkDep.dependencyFlags = 0;

			subpassDependencies.PushBack(vkDep);
		}

		VkRenderPassCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		createInfo.attachmentCount = (uint32)attachmentDescriptions.GetSize();
		createInfo.pAttachments = attachmentDescriptions.Data();
		createInfo.subpassCount = (uint32)subpassDescriptions.GetSize();
		createInfo.pSubpasses = subpassDescriptions.Data();
		createInfo.dependencyCount = (uint32)subpassDependencies.GetSize();
		createInfo.pDependencies = subpassDependencies.Data();

		result = vkCreateRenderPass(render->GetDevice(), &createInfo, nullptr, &m_Pass);
		if (ME_VK_FAILED(result))
		{
			ME_ASSERT(false, TEXT("Vulkan render pass: failed to create render pass! Error {0}"), (int32)result);
		}
	}


}
