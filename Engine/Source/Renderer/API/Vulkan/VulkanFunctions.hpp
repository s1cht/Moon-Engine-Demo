#pragma once
#include <Core.hpp>

#include "Vulkan.hpp"
#include "Renderer/Base/Pipeline.hpp"
#include "Renderer/Base/RenderPass.hpp"
#include "Renderer/Base/Texture.hpp"

namespace ME::Render
{
	constexpr MEAPI VkFormat ConvertFormatVulkan(ME::Render::Format format);
	constexpr MEAPI VkImageLayout ConvertImageLayoutVulkan(ME::Render::ImageLayout layout);
	constexpr MEAPI VkAttachmentLoadOp ConvertAttachmentLoadOperationVulkan(ME::Render::LoadOperation operation);
	constexpr MEAPI VkAttachmentStoreOp ConvertAttachmentStoreOperationVulkan(ME::Render::StoreOperation operation);
	constexpr MEAPI VkPipelineBindPoint ConvertPipelineBindPointVulkan(ME::Render::PipelineBindPoint operation);
	constexpr MEAPI VkPipelineStageFlags ConvertPipelineStageFlagsVulkan(ME::Render::PipelineStageFlags flags);
	constexpr MEAPI VkAccessFlags ConvertAccessFlagsVulkan(ME::Render::AccessFlags flags);
	constexpr MEAPI VkPrimitiveTopology ConvertPrimitiveTopologyVulkan(ME::Render::PrimitiveTopology topology);
	constexpr MEAPI VkPolygonMode ConvertRasterizerFillVulkan(ME::Render::RasterizationFill fill);
	constexpr MEAPI VkCullModeFlagBits ConvertRasterizerCullVulkan(ME::Render::RasterizationCull cull);
	constexpr MEAPI VkSampleCountFlagBits ConvertSampleCountVulkan(ME::Render::SampleCount sampleCount);
	constexpr MEAPI VkCompareOp ConvertDepthComparisonVulkan(ME::Render::DepthComparison function);
	constexpr MEAPI VkLogicOp ConvertLogicOperationVulkan(ME::Render::LogicOperation operation);
	constexpr MEAPI VkDescriptorType ConvertResourceTypeVulkan(ME::Render::ResourceType type);
	constexpr MEAPI VkShaderStageFlags ConvertShaderStageVulkan(ME::Render::ShaderStage stage);
	constexpr MEAPI VkShaderStageFlags ConvertImageUsageFlagsVulkan(ME::Render::ImageUsageFlags usage);
	constexpr MEAPI VkFilter ConvertSamplerFilterVulkan(ME::Render::SamplerFilter filter);

	constexpr MEAPI ME::Render::Format ConvertFormatEngine(VkFormat format);
	constexpr MEAPI ME::Render::ImageLayout ConvertImageLayoutEngine(VkImageLayout layout);
	constexpr MEAPI ME::Render::LoadOperation ConvertAttachmentLoadOperationEngine(VkAttachmentLoadOp operation);
	constexpr MEAPI ME::Render::StoreOperation ConvertAttachmentStoreOperationEngine(VkAttachmentStoreOp operation);
	constexpr MEAPI ME::Render::PipelineBindPoint ConvertPipelineBindPointEngine(VkPipelineBindPoint operation);
	constexpr MEAPI ME::Render::PipelineStageFlags ConvertPipelineStageFlagsEngine(VkPipelineStageFlags flags);
	constexpr MEAPI ME::Render::AccessFlags ConvertAccessFlagsEngine(VkAccessFlags flags);

	MEAPI const asciichar* VulkanMessageTypeToText(const VkDebugUtilsMessageTypeFlagsEXT& messageType);
}