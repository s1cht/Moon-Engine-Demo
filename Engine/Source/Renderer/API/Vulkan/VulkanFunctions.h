#pragma once

#include <Core.hpp>
#include <vulkan/vulkan.hpp>

#include "Renderer/Base/Pipeline.h"
#include "Renderer/Base/RenderPass.h"
#include "Renderer/Base/Texture.h"

namespace Pawn::Render
{
	constexpr PAWN_API VkFormat ConvertFormatVulkan(Pawn::Render::Format format);
	constexpr PAWN_API VkImageLayout ConvertImageLayoutVulkan(Pawn::Render::ImageLayout format);
	constexpr PAWN_API VkAttachmentLoadOp ConvertAttachmentLoadOperationVulkan(Pawn::Render::LoadOperation operation);
	constexpr PAWN_API VkAttachmentStoreOp ConvertAttachmentStoreOperationVulkan(Pawn::Render::StoreOperation operation);
	constexpr PAWN_API VkPipelineBindPoint ConvertPipelineBindPointVulkan(Pawn::Render::PipelineBindPoint operation);
	constexpr PAWN_API VkPipelineStageFlags ConvertPipelineStageFlagsVulkan(Pawn::Render::PipelineStageFlags flags);
	constexpr PAWN_API VkAccessFlags ConvertAccessFlagsVulkan(Pawn::Render::AccessFlags flags);

}