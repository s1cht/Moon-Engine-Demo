#pragma once

#include <Core.hpp>
#include <vulkan/vulkan.hpp>

#include "Renderer/Base/Pipeline.h"
#include "Renderer/Base/RenderPass.h"
#include "Renderer/Base/Texture.h"

namespace ME::Render
{
	constexpr MOON_API VkFormat ConvertFormatVulkan(ME::Render::Format format);
	constexpr MOON_API VkImageLayout ConvertImageLayoutVulkan(ME::Render::ImageLayout layout);
	constexpr MOON_API VkAttachmentLoadOp ConvertAttachmentLoadOperationVulkan(ME::Render::LoadOperation operation);
	constexpr MOON_API VkAttachmentStoreOp ConvertAttachmentStoreOperationVulkan(ME::Render::StoreOperation operation);
	constexpr MOON_API VkPipelineBindPoint ConvertPipelineBindPointVulkan(ME::Render::PipelineBindPoint operation);
	constexpr MOON_API VkPipelineStageFlags ConvertPipelineStageFlagsVulkan(ME::Render::PipelineStageFlags flags);
	constexpr MOON_API VkAccessFlags ConvertAccessFlagsVulkan(ME::Render::AccessFlags flags);
	constexpr MOON_API VkPrimitiveTopology ConvertPrimitiveTopologyVulkan(ME::Render::PrimitiveTopology topology);
	constexpr MOON_API VkPolygonMode ConvertRasterizerFillVulkan(ME::Render::RasterizationFill fill);
	constexpr MOON_API VkCullModeFlagBits ConvertRasterizerCullVulkan(ME::Render::RasterizationCull cull);
	constexpr MOON_API VkSampleCountFlagBits ConvertSampleCountVulkan(ME::Render::SampleCount sampleCount);
	constexpr MOON_API VkCompareOp ConvertDepthComparisonVulkan(ME::Render::DepthComparison function);
	constexpr MOON_API VkLogicOp ConvertLogicOperationVulkan(ME::Render::LogicOperation operation);

	constexpr MOON_API ME::Render::Format ConvertFormatEngine(VkFormat format);
	constexpr MOON_API ME::Render::ImageLayout ConvertImageLayoutEngine(VkImageLayout layout);
	constexpr MOON_API ME::Render::LoadOperation ConvertAttachmentLoadOperationEngine(VkAttachmentLoadOp operation);
	constexpr MOON_API ME::Render::StoreOperation ConvertAttachmentStoreOperationEngine(VkAttachmentStoreOp operation);
	constexpr MOON_API ME::Render::PipelineBindPoint ConvertPipelineBindPointEngine(VkPipelineBindPoint operation);
	constexpr MOON_API ME::Render::PipelineStageFlags ConvertPipelineStageFlagsEngine(VkPipelineStageFlags flags);
	constexpr MOON_API ME::Render::AccessFlags ConvertAccessFlagsEngine(VkAccessFlags flags);

	MOON_API const ansichar* VulkanMessageTypeToText(const VkDebugUtilsMessageTypeFlagsEXT& messageType);
}