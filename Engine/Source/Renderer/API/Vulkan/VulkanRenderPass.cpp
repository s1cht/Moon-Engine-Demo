#include "VulkanRenderPass.hpp"
#include "VulkanRenderAPI.hpp"
#include "VulkanFunctions.hpp"
#include "VulkanCommandBuffer.hpp"
#include "VulkanFramebuffer.hpp"
#include "Renderer/RenderCommand.hpp"
#include "Renderer/RenderResourcesTracker.hpp"

namespace ME::Render
{
	Core::Memory::Reference<Render::RenderPass> RenderPass::CreateVulkan(const RenderPassSpecification& specification)
	{
		auto object = Core::Memory::MakeReference<VulkanRenderPass>(specification);
		RenderResourcesTracker::Get().AddItem(object);
		return object;
	}

	VulkanRenderPass::VulkanRenderPass(const RenderPassSpecification& specification)
		: m_Specification(specification)
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
			vkDestroyRenderPass(Render::RenderCommand::Get()->As<VulkanRenderAPI>()->GetDevice(), m_Pass, nullptr);
			m_Pass = nullptr;
		}
	}

	void VulkanRenderPass::Begin(ME::Core::Memory::Reference<ME::Render::CommandBuffer> buffer, RenderPassBeginInfo& beginInfo)
	{
		VulkanCommandBuffer* cmdBuf = buffer->As<VulkanCommandBuffer>();

		Core::Array<VkClearValue> clearValues(beginInfo.ClearValues.Size());
	    for (uint32 i = 0; i < beginInfo.ClearValues.Size(); i++)
		{
			if (beginInfo.ClearValues[i].UsingDepth)
			{
				clearValues[i].depthStencil.depth = beginInfo.ClearValues[i].DepthClearValue.Depth;
				clearValues[i].depthStencil.stencil = beginInfo.ClearValues[i].DepthClearValue.Stencil;
				continue;
			}
	        clearValues[i].color = reinterpret_cast<VkClearColorValue&&>(beginInfo.ClearValues[i].ColorClearValue);
		}

		VkRenderPassBeginInfo vkBeginInfo = {};
		vkBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		vkBeginInfo.renderPass = m_Pass;
		vkBeginInfo.clearValueCount = static_cast<uint32>(clearValues.Size());
		vkBeginInfo.pClearValues = clearValues.Data();
		vkBeginInfo.framebuffer = beginInfo.Framebuffer->As<VulkanFramebuffer>()->GetFramebuffer();
		vkBeginInfo.renderArea.offset.x = beginInfo.RenderArea.Offset.x;
		vkBeginInfo.renderArea.offset.y = beginInfo.RenderArea.Offset.y;
		vkBeginInfo.renderArea.extent.width = beginInfo.RenderArea.Extent.x;
		vkBeginInfo.renderArea.extent.height = beginInfo.RenderArea.Extent.y;

		vkCmdBeginRenderPass(cmdBuf->GetCommandBuffer(), &vkBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
	}

	void VulkanRenderPass::End(ME::Core::Memory::Reference<ME::Render::CommandBuffer> buffer)
	{
		vkCmdEndRenderPass(buffer->As<VulkanCommandBuffer>()->GetCommandBuffer());
	}

	void VulkanRenderPass::Init(const RenderPassSpecification& specification)
	{
		VkResult result;
		VulkanRenderAPI* render = Render::RenderCommand::Get()->As<VulkanRenderAPI>();

		ME::Core::Array<VkAttachmentDescription> attachmentDescriptions;
		ME::Core::Array<VkAttachmentReference> globalAttachmentReferences;

		for (auto& attachment : specification.AttachmentSpecs)
		{
			VkAttachmentDescription desc = {};
			desc.format = ME::Render::ConvertFormatVulkan(attachment.AttachmentFormat);
			desc.initialLayout = ME::Render::ConvertImageLayoutVulkan(attachment.InitialLayout);
			desc.finalLayout = ME::Render::ConvertImageLayoutVulkan(attachment.FinalLayout);
			desc.loadOp = attachment.IsStencil ? VK_ATTACHMENT_LOAD_OP_DONT_CARE : ME::Render::ConvertAttachmentLoadOperationVulkan(attachment.LoadOp);
			desc.stencilLoadOp = attachment.IsStencil ? VK_ATTACHMENT_LOAD_OP_DONT_CARE : ME::Render::ConvertAttachmentLoadOperationVulkan(attachment.LoadOp);
			desc.storeOp = attachment.IsStencil ? VK_ATTACHMENT_STORE_OP_DONT_CARE : ME::Render::ConvertAttachmentStoreOperationVulkan(attachment.StoreOp);
			desc.stencilStoreOp = attachment.IsStencil ? VK_ATTACHMENT_STORE_OP_DONT_CARE : ME::Render::ConvertAttachmentStoreOperationVulkan(attachment.StoreOp);
			desc.samples = ConvertSampleCountVulkan(attachment.SampleCount);
			desc.flags = 0;

			attachmentDescriptions.EmplaceBack(desc);

			VkAttachmentReference ref = {};
			ref.attachment = static_cast<uint32>(globalAttachmentReferences.Size());
			ref.layout = (attachment.IsDepth || attachment.IsStencil)
				? VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
				: VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

			globalAttachmentReferences.EmplaceBack(ref);
		}

		ME::Core::Array<ME::Core::Array<VkAttachmentReference>> colorAttachmentRefsList;
		ME::Core::Array<ME::Core::Array<VkAttachmentReference>> inputAttachmentRefsList;
		ME::Core::Array<VkAttachmentReference> depthStencilAttachmentRefs;

		ME::Core::Array<VkSubpassDescription> subpassDescriptions;

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
			subpassDesc.pipelineBindPoint			= ME::Render::ConvertPipelineBindPointVulkan(subpass.PipelineBindPoint);
			subpassDesc.colorAttachmentCount		= static_cast<uint32>(colorAttachmentRefsList.Back().Size());
			subpassDesc.pColorAttachments			= colorAttachmentRefsList.Back().Data();
			subpassDesc.inputAttachmentCount		= static_cast<uint32>(inputAttachmentRefsList.Back().Size());
			subpassDesc.pInputAttachments			= inputAttachmentRefsList.Back().Data();
			subpassDesc.pDepthStencilAttachment		= (subpass.DepthStencilAttachmentRef != ~0u) ? &depthStencilAttachmentRefs.Back() : nullptr;

			subpassDescriptions.EmplaceBack(subpassDesc);
		}

		ME::Core::Array<VkSubpassDependency> subpassDependencies;
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
		createInfo.attachmentCount = static_cast<uint32>(attachmentDescriptions.Size());
		createInfo.pAttachments = attachmentDescriptions.Data();
		createInfo.subpassCount = static_cast<uint32>(subpassDescriptions.Size());
		createInfo.pSubpasses = subpassDescriptions.Data();
		createInfo.dependencyCount = static_cast<uint32>(subpassDependencies.Size());
		createInfo.pDependencies = subpassDependencies.Data();

		result = vkCreateRenderPass(render->GetDevice(), &createInfo, nullptr, &m_Pass);
		if (ME_VK_FAILED(result))
			ME_ASSERT(false, "Vulkan render pass: failed to create render pass! Error {0}", static_cast<int32>(result));
	}


}
