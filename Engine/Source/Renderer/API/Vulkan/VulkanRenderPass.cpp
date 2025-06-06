#include "VulkanRenderPass.h"

#include "VulkanCommandBuffer.h"
#include "VulkanFunctions.h"
#include "VulkanMacros.hpp"
#include "Renderer/RenderCommand.h"

namespace Pawn::Render
{
	RenderPass* RenderPass::CreateVulkanRenderPass(RenderPassSpecification& specification)
	{
		return new VulkanRenderPass(specification);
	}

	VulkanRenderPass::VulkanRenderPass(RenderPassSpecification& specification)
	{
		Init(specification);
	}

	VulkanRenderPass::~VulkanRenderPass()
	{
		VulkanRenderer* render = static_cast<VulkanRenderer*>(Render::RenderCommand::Get());

		if (m_Pass != nullptr)
		{
			vkDestroyRenderPass(render->GetDevice(), m_Pass, nullptr);
			m_Pass = nullptr;
		}
	}

	void VulkanRenderPass::Begin(CommandBuffer* buffer, RenderPassBeginInfo& beginInfo)
	{
		VulkanCommandBuffer* cmdBuf = static_cast<VulkanCommandBuffer*>(buffer);

		VkRenderPassBeginInfo vkBeginInfo = {};
		vkBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		vkBeginInfo.renderPass = m_Pass;
		vkBeginInfo.clearValueCount = (uint32)beginInfo.ClearValues.GetSize();
		vkBeginInfo.pClearValues = reinterpret_cast<const VkClearValue*>(beginInfo.ClearValues.Data());
		vkBeginInfo.framebuffer = static_cast<VulkanFramebuffer*>(beginInfo.Framebuffer)->GetFramebuffer();
		vkBeginInfo.renderArea.offset.x = beginInfo.RenderArea.Offset.x;
		vkBeginInfo.renderArea.offset.y = beginInfo.RenderArea.Offset.y;
		vkBeginInfo.renderArea.extent.height = beginInfo.RenderArea.Extent.x;
		vkBeginInfo.renderArea.extent.width = beginInfo.RenderArea.Extent.y;

		vkCmdBeginRenderPass(cmdBuf->GetBuffer(), &vkBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
	}

	void VulkanRenderPass::End(CommandBuffer* buffer)
	{
		vkCmdEndRenderPass(static_cast<VulkanCommandBuffer*>(buffer)->GetBuffer());
	}

	void VulkanRenderPass::Init(RenderPassSpecification& specification)
	{
		VkResult result;
		VulkanRenderer* render = static_cast<VulkanRenderer*>(Render::RenderCommand::Get());

		// Храним все описания, потом сохраняем их по указателям
		Pawn::Core::Containers::Array<VkAttachmentDescription> attachmentDescriptions;
		Pawn::Core::Containers::Array<VkAttachmentReference> globalAttachmentReferences;

		for (auto& attachment : specification.AttachmentSpecs)
		{
			VkAttachmentDescription desc{};
			desc.format = Pawn::Render::ConvertFormatVulkan(attachment.AttachmentFormat);
			desc.initialLayout = Pawn::Render::ConvertImageLayoutVulkan(attachment.InitialLayout);
			desc.finalLayout = Pawn::Render::ConvertImageLayoutVulkan(attachment.FinalLayout);
			desc.loadOp = attachment.IsStencil ? VK_ATTACHMENT_LOAD_OP_DONT_CARE : Pawn::Render::ConvertAttachmentLoadOperationVulkan(attachment.LoadOp);
			desc.stencilLoadOp = attachment.IsStencil ? VK_ATTACHMENT_LOAD_OP_DONT_CARE : Pawn::Render::ConvertAttachmentLoadOperationVulkan(attachment.LoadOp);
			desc.storeOp = attachment.IsStencil ? VK_ATTACHMENT_STORE_OP_DONT_CARE : Pawn::Render::ConvertAttachmentStoreOperationVulkan(attachment.StoreOp);
			desc.stencilStoreOp = attachment.IsStencil ? VK_ATTACHMENT_STORE_OP_DONT_CARE : Pawn::Render::ConvertAttachmentStoreOperationVulkan(attachment.StoreOp);
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

		Pawn::Core::Containers::Array<Pawn::Core::Containers::Array<VkAttachmentReference>> colorAttachmentRefsList;
		Pawn::Core::Containers::Array<Pawn::Core::Containers::Array<VkAttachmentReference>> inputAttachmentRefsList;
		Pawn::Core::Containers::Array<VkAttachmentReference> depthStencilAttachmentRefs;

		Pawn::Core::Containers::Array<VkSubpassDescription> subpassDescriptions;

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
			subpassDesc.pipelineBindPoint = Pawn::Render::ConvertPipelineBindPointVulkan(subpass.PipelineBindPoint);
			subpassDesc.colorAttachmentCount = (uint32)colorAttachmentRefsList.Back().GetSize();
			subpassDesc.pColorAttachments = colorAttachmentRefsList.Back().Data();
			subpassDesc.inputAttachmentCount = (uint32)inputAttachmentRefsList.Back().GetSize();
			subpassDesc.pInputAttachments = inputAttachmentRefsList.Back().Data();
			subpassDesc.pDepthStencilAttachment = (subpass.DepthStencilAttachmentRef != ~0u) ? &depthStencilAttachmentRefs.Back() : nullptr;

			subpassDescriptions.EmplaceBack(subpassDesc);
		}

		Pawn::Core::Containers::Array<VkSubpassDependency> subpassDependencies;
		for (auto& dep : specification.SubpassDependencies)
		{
			VkSubpassDependency vkDep{};
			vkDep.srcSubpass = dep.SubpassSrc;
			vkDep.dstSubpass = dep.SubpassDst;
			vkDep.srcStageMask = Pawn::Render::ConvertPipelineStageFlagsVulkan(dep.PipelineStageFlagsSrc);
			vkDep.dstStageMask = Pawn::Render::ConvertPipelineStageFlagsVulkan(dep.PipelineStageFlagsDst);
			vkDep.srcAccessMask = Pawn::Render::ConvertAccessFlagsVulkan(dep.AccessFlagsSrc);
			vkDep.dstAccessMask = Pawn::Render::ConvertAccessFlagsVulkan(dep.AccessFlagsDst);
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
		if (PE_VK_FAILED(result))
		{
			PE_ASSERT(false, TEXT("Vulkan render pass: failed to create render pass! Error {0}"), (int32)result);
		}
	}


}