#include "VulkanPipeline.h"
#include "VulkanCommandBuffer.h"
#include "VulkanFramebuffer.h"
#include "VulkanFunctions.h"
#include "VulkanMacros.hpp"
#include "VulkanRenderPass.h"
#include "VulkanShader.h"
#include "Renderer/RenderCommand.h"
#include "Renderer/RenderResourcesTracker.hpp"

namespace ME::Render
{
	ME::Core::Memory::Reference<Pipeline> Pipeline::CreateVulkanPipeline(const PipelineSpecification& specification)
	{
		auto object = ME::Core::Memory::Reference<Pipeline>(new VulkanPipeline(specification));
		RenderResourcesTracker::Get().AddItem(object);
		return object;
	}

	VulkanPipeline::VulkanPipeline(const PipelineSpecification& specification)
	{
		m_Specification = std::move(specification);
		Init();
	}

	VulkanPipeline::~VulkanPipeline()
	{
		Shutdown();
	}

	void VulkanPipeline::Shutdown()
	{
		auto render = RenderCommand::Get()->As<VulkanRenderer>();

		if (m_Pipeline != nullptr)
		{
			vkDestroyPipeline(render->GetDevice(), m_Pipeline, nullptr);
			m_Pipeline = nullptr;
		}
		if (m_PipelineLayout != nullptr)
		{
			vkDestroyPipelineLayout(render->GetDevice(), m_PipelineLayout, nullptr);
			m_PipelineLayout = nullptr;
		}
	}

	void VulkanPipeline::SetViewports(ME::Core::Memory::Reference<ME::Render::CommandBuffer> buffer, ME::Core::Containers::Array<ME::Render::ViewportSpecification> specifications)
	{
		ME::Core::Containers::Array<VkViewport> viewports;

		for (const auto& spec : specifications)
		{
			VkViewport viewport = {};
			viewport.x = spec.X;
			viewport.y = spec.Y;
			viewport.height = spec.Height;
			viewport.width = spec.Width;
			viewport.minDepth = spec.MinDepth;
			viewport.maxDepth = spec.MaxDepth;

			viewports.EmplaceBack(viewport);
		}

		vkCmdSetViewport(buffer->As<VulkanCommandBuffer>()->GetBuffer(), 0, static_cast<uint32>(viewports.GetSize()), viewports.Data());
	}

	void VulkanPipeline::SetScissors(ME::Core::Memory::Reference<ME::Render::CommandBuffer> buffer, ME::Core::Containers::Array<ME::Core::Math::Rect2D> scissors)
	{
		ME::Core::Containers::Array<VkRect2D> vkScissors;

		for (const auto& spec : scissors)
		{
			VkRect2D scissor = {};
			scissor.extent.width = spec.Extent.x;
			scissor.extent.height = spec.Extent.y;
			scissor.offset.x = spec.Offset.x;
			scissor.offset.y = spec.Offset.y;

			vkScissors.EmplaceBack(scissor);
		}

		vkCmdSetScissor(buffer->As<VulkanCommandBuffer>()->GetBuffer(), 0, static_cast<uint32>(vkScissors.GetSize()), vkScissors.Data());
	}

	void VulkanPipeline::Bind(ME::Core::Memory::Reference<ME::Render::CommandBuffer> buffer)
	{
		vkCmdBindPipeline(buffer->As<VulkanCommandBuffer>()->GetBuffer(), m_PipelineBindPoint, m_Pipeline);
	}

	void VulkanPipeline::Init()
	{
		CreatePipelineLayout();
		switch (m_Specification.Type)
		{
		case PipelineType::Compute: CreateComputePipeline(); m_PipelineBindPoint = VK_PIPELINE_BIND_POINT_COMPUTE; break;
		case PipelineType::Graphics: CreateGraphicsPipeline(); m_PipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS; break;
		case PipelineType::None: ME_ASSERT(false, TEXT("Pipeline: Can't create pipeline with type None!")); break;
		}	
	}

	ME::Core::Containers::Array<VkPipelineShaderStageCreateInfo> VulkanPipeline::FormatPipelineShaderStageCI()
	{
		ME::Core::Containers::Array<VkPipelineShaderStageCreateInfo> shaderStages(5);

		for (uint8 i = 0; i < 5; i++)
		{
			shaderStages[i].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			shaderStages[i].pSpecializationInfo = nullptr;
		}

		shaderStages[0].module = m_Specification.Shaders.Vertex->As<VulkanShader>()->GetShaderModule();
		shaderStages[0].pName = ME_SHADER_VERTEX_ENTRY_POINT;
		shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;

		shaderStages[1].module = m_Specification.Shaders.Pixel->As<VulkanShader>()->GetShaderModule();
		shaderStages[1].pName = ME_SHADER_PIXEL_ENTRY_POINT;
		shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;

		if (m_Specification.Shaders.Hull != nullptr)
		{
			shaderStages[2].module = m_Specification.Shaders.Hull->As<VulkanShader>()->GetShaderModule();
			shaderStages[2].pName = ME_SHADER_HULL_ENTRY_POINT;
			shaderStages[2].stage = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
		}
		else
		{
			shaderStages.SetSize(shaderStages.GetSize() - 1);
		}
		if (m_Specification.Shaders.Domain != nullptr)
		{
			shaderStages[3].module = m_Specification.Shaders.Domain->As<VulkanShader>()->GetShaderModule();
			shaderStages[3].pName = ME_SHADER_DOMAIN_ENTRY_POINT;
			shaderStages[3].stage = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
		}
		else
		{
			shaderStages.SetSize(shaderStages.GetSize() - 1);
		}
		if (m_Specification.Shaders.Geometry != nullptr)
		{
			shaderStages[4].module = m_Specification.Shaders.Geometry->As<VulkanShader>()->GetShaderModule();
			shaderStages[4].pName = ME_SHADER_GEOMETRY_ENTRY_POINT;
			shaderStages[4].stage = VK_SHADER_STAGE_GEOMETRY_BIT;
		}
		else
		{
			shaderStages.SetSize(shaderStages.GetSize() - 1);
		}

		return shaderStages;
	}

	void VulkanPipeline::CreatePipelineLayout()
	{
		VkResult result;
		VkPipelineLayoutCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		createInfo.setLayoutCount = 0;
		createInfo.pSetLayouts = nullptr;
		createInfo.pushConstantRangeCount = 0;
		createInfo.pPushConstantRanges = nullptr;

		result = vkCreatePipelineLayout(ME::Render::RenderCommand::Get()->As<VulkanRenderer>()->GetDevice(), &createInfo, nullptr, &m_PipelineLayout);
		if (ME_VK_FAILED(result))
		{
			ME_CORE_ASSERT(false, TEXT("Vulkan: pipeline layout creation failed! Error: {0}"), static_cast<int32>(result));
		}
	}


	void VulkanPipeline::CreateComputePipeline()
	{
		VkResult result;
		VkPipelineShaderStageCreateInfo shaderCreateInfo = {};
		shaderCreateInfo.pSpecializationInfo = nullptr;
		shaderCreateInfo.module = m_Specification.ComputeShader->As<VulkanShader>()->GetShaderModule();
		shaderCreateInfo.pName = ME_SHADER_COMPUTE_ENTRY_POINT;
		shaderCreateInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;

		VkComputePipelineCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
		createInfo.basePipelineHandle = m_Specification.BasePipeline->As<VulkanPipeline>()->GetPipeline();
		createInfo.basePipelineIndex = -1;
		createInfo.layout = m_PipelineLayout;
		createInfo.stage = shaderCreateInfo;

		result = vkCreateComputePipelines(ME::Render::RenderCommand::Get()->As<VulkanRenderer>()->GetDevice(), nullptr, 1, &createInfo, nullptr, &m_Pipeline);
		if (ME_VK_FAILED(result))
		{
			ME_CORE_ASSERT(false, TEXT("Vulkan: pipeline creation failed! Error: {0}"), static_cast<int32>(result));
		}
	}

	void VulkanPipeline::CreateGraphicsPipeline()
	{
		VkResult result;

		ME::Core::Containers::Array<VkPipelineShaderStageCreateInfo> shaderStage = FormatPipelineShaderStageCI();

		VkVertexInputBindingDescription visBindingDesc = {};
		visBindingDesc.binding = 0;
		visBindingDesc.stride = m_Specification.BufferLayout.GetStride();
		switch (m_Specification.BufferLayout.GetInputClassification())
		{
		case InputClassification::PerVertex: visBindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX; break;
		case InputClassification::PerInstance: visBindingDesc.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE; break;
		case InputClassification::None: visBindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX; ME_WARN(TEXT("Vulkan pipeline: InputClassification::None is used! Using VK_VERTEX_INPUT_RATE_VERTEX instead.")); break;
		}

		ME::Core::Containers::Array<VkVertexInputAttributeDescription> visAttributeDesc = {};

		for (const auto& element : m_Specification.BufferLayout.GetElements())
		{
			VkVertexInputAttributeDescription desc = {};
			desc.location = element.SemanticIndex;
			desc.format = static_cast<VkFormat>(ConvertShaderTypeVulkan(element.Type));
			desc.binding = element.InputSlot;
			desc.offset = static_cast<uint32>(element.Offset);

			visAttributeDesc.EmplaceBack(desc);
		}

		VkPipelineVertexInputStateCreateInfo visCreateInfo = {};
		visCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		visCreateInfo.pVertexBindingDescriptions = &visBindingDesc;
		visCreateInfo.vertexBindingDescriptionCount = 1;
		visCreateInfo.pVertexAttributeDescriptions = visAttributeDesc.Data();
		visCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32>(visAttributeDesc.GetSize());

		VkPipelineInputAssemblyStateCreateInfo iasCreateInfo = {};
		iasCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		iasCreateInfo.topology = ConvertPrimitiveTopologyVulkan(m_Specification.InputAssembly.Topology);
		iasCreateInfo.primitiveRestartEnable = m_Specification.InputAssembly.PrimitiveRestart;
		
		VkPipelineTessellationStateCreateInfo tsCreateInfo = {};
		tsCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
		tsCreateInfo.patchControlPoints = m_Specification.InputAssembly.Topology == PrimitiveTopology::PatchList ? m_Specification.PatchControlPoints : 0;

		VkPipelineViewportStateCreateInfo vsCreateInfo = {};
		vsCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		vsCreateInfo.scissorCount = 1;
		vsCreateInfo.pScissors = nullptr;
		vsCreateInfo.viewportCount = 1;
		vsCreateInfo.pViewports = nullptr;

		VkPipelineRasterizationStateCreateInfo rsCreateInfo = {};
		rsCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rsCreateInfo.rasterizerDiscardEnable = m_Specification.Rasterization.DiscardEnabled;
		rsCreateInfo.depthBiasEnable = m_Specification.Rasterization.DepthBiasEnabled;
		rsCreateInfo.depthBiasConstantFactor = m_Specification.Rasterization.DepthBiasEnabled ? m_Specification.Rasterization.DepthBiasConstantFactor : 1.f;
		rsCreateInfo.depthBiasClamp = m_Specification.Rasterization.DepthBiasEnabled ? m_Specification.Rasterization.DepthBiasClamp : 0.f;
		rsCreateInfo.depthBiasSlopeFactor = m_Specification.Rasterization.DepthBiasEnabled ? m_Specification.Rasterization.DepthBiasSlopeFactor : 0.f;
		rsCreateInfo.depthClampEnable = m_Specification.Rasterization.DepthClampEnabled;
		rsCreateInfo.frontFace = m_Specification.Rasterization.FrontCounterClockwise ? VK_FRONT_FACE_COUNTER_CLOCKWISE : VK_FRONT_FACE_CLOCKWISE;
		rsCreateInfo.lineWidth = m_Specification.Rasterization.LineWidth;
		rsCreateInfo.polygonMode = ConvertRasterizerFillVulkan(m_Specification.Rasterization.Fill);
		rsCreateInfo.cullMode = ConvertRasterizerCullVulkan(m_Specification.Rasterization.Cull);

		VkPipelineMultisampleStateCreateInfo msCreateInfo = {};
		msCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		msCreateInfo.sampleShadingEnable = m_Specification.Multisampling.EnableSampleShading;
		msCreateInfo.minSampleShading = m_Specification.Multisampling.MinSampleShading;
		msCreateInfo.alphaToOneEnable = m_Specification.Multisampling.AlphaToOne;
		msCreateInfo.alphaToCoverageEnable = m_Specification.Multisampling.AlphaToCoverage;
		msCreateInfo.rasterizationSamples = ConvertSampleCountVulkan(m_Specification.Multisampling.Samples);
		msCreateInfo.pSampleMask = nullptr;

		VkPipelineDepthStencilStateCreateInfo dssCreateInfo = {};
		dssCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		dssCreateInfo.depthBoundsTestEnable = false;
		dssCreateInfo.depthCompareOp = ConvertDepthComparisonVulkan(m_Specification.DepthStencil.DepthFunction);
		dssCreateInfo.depthTestEnable = m_Specification.DepthStencil.DepthEnabled;
		dssCreateInfo.depthWriteEnable = m_Specification.DepthStencil.DepthEnabled;
		dssCreateInfo.stencilTestEnable = m_Specification.DepthStencil.StencilEnabled;
		dssCreateInfo.front.compareMask = 0xff;
		dssCreateInfo.front.writeMask = 0xff;
		dssCreateInfo.front.reference = 0;
		dssCreateInfo.front.failOp = VK_STENCIL_OP_KEEP;
		dssCreateInfo.front.depthFailOp = m_Specification.DepthStencil.StencilEnabled ? VK_STENCIL_OP_INCREMENT_AND_CLAMP : VK_STENCIL_OP_KEEP;
		dssCreateInfo.front.passOp = VK_STENCIL_OP_KEEP;
		dssCreateInfo.front.compareOp = m_Specification.DepthStencil.StencilEnabled ? VK_COMPARE_OP_ALWAYS : VK_COMPARE_OP_NEVER;
		dssCreateInfo.back = dssCreateInfo.front;
		dssCreateInfo.minDepthBounds = m_Specification.DepthStencil.MinDepthBounds;
		dssCreateInfo.maxDepthBounds = m_Specification.DepthStencil.MaxDepthBounds;

		ME::Core::Containers::Array<VkPipelineColorBlendAttachmentState> cbsAttachments(m_Specification.ColorBlending.Attachments.GetSize());

		for (uint32 i = 0; i < m_Specification.ColorBlending.Attachments.GetSize(); i++)
		{
			cbsAttachments[i].blendEnable = m_Specification.ColorBlending.Attachments[i].EnableBlend;
			cbsAttachments[i].srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
			cbsAttachments[i].dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
			cbsAttachments[i].colorBlendOp = VK_BLEND_OP_ADD;
			cbsAttachments[i].srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
			cbsAttachments[i].dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
			cbsAttachments[i].alphaBlendOp = VK_BLEND_OP_ADD;
			cbsAttachments[i].colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | (m_Specification.ColorBlending.Attachments[i].EnableAlphaMask ? VK_COLOR_COMPONENT_A_BIT : 0);
		}


		VkPipelineColorBlendStateCreateInfo cbsCreateInfo = {};
		cbsCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		cbsCreateInfo.attachmentCount = static_cast<uint32>(cbsAttachments.GetSize());
		cbsCreateInfo.pAttachments = cbsAttachments.Data();
		cbsCreateInfo.blendConstants[0] = m_Specification.ColorBlending.BlendConstants.X;
		cbsCreateInfo.blendConstants[1] = m_Specification.ColorBlending.BlendConstants.Y;
		cbsCreateInfo.blendConstants[2] = m_Specification.ColorBlending.BlendConstants.Z;
		cbsCreateInfo.blendConstants[3] = m_Specification.ColorBlending.BlendConstants.W;
		cbsCreateInfo.logicOpEnable = m_Specification.ColorBlending.LogicOperation != LogicOperation::None;
		cbsCreateInfo.logicOp = ConvertLogicOperationVulkan(m_Specification.ColorBlending.LogicOperation);

		ME::Core::Containers::Array<VkDynamicState> dynamicStates = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR,
			VK_DYNAMIC_STATE_STENCIL_REFERENCE,
		};

		VkPipelineDynamicStateCreateInfo dsCreateInfo = {};	
		dsCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dsCreateInfo.dynamicStateCount = static_cast<uint32>(dynamicStates.GetSize());
		dsCreateInfo.pDynamicStates = dynamicStates.Data();

		VkGraphicsPipelineCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		createInfo.renderPass = m_Specification.RenderPass->As<VulkanRenderPass>()->GetRenderPass();
		createInfo.subpass = m_Specification.Subpass;
		createInfo.pStages = shaderStage.Data();
		createInfo.stageCount = static_cast<uint32>(shaderStage.GetSize());
		createInfo.pVertexInputState = &visCreateInfo;
		createInfo.pInputAssemblyState = &iasCreateInfo;
		createInfo.pTessellationState = &tsCreateInfo;
		createInfo.pViewportState = &vsCreateInfo;
		createInfo.pRasterizationState = &rsCreateInfo;
		createInfo.pMultisampleState = &msCreateInfo;
		createInfo.pDepthStencilState = &dssCreateInfo;
		createInfo.pColorBlendState = &cbsCreateInfo;
		createInfo.pDynamicState = &dsCreateInfo;
		createInfo.basePipelineHandle = m_Specification.BasePipeline ? m_Specification.BasePipeline->As<VulkanPipeline>()->GetPipeline() : nullptr;
		createInfo.basePipelineIndex = -1;
		createInfo.layout = m_PipelineLayout;

		result = vkCreateGraphicsPipelines(ME::Render::RenderCommand::Get()->As<VulkanRenderer>()->GetDevice(), nullptr, 1, &createInfo, nullptr, &m_Pipeline);

		if (ME_VK_FAILED(result))
		{
			ME_CORE_ASSERT(false, TEXT("Vulkan: pipeline creation failed! Error: {0}"), static_cast<int32>(result));
		}
	}
}
