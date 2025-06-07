#include "VulkanFunctions.h"

namespace Pawn::Render
{
	constexpr PAWN_API VkFormat ConvertFormatVulkan(Pawn::Render::Format format)
	{
		switch (format)
		{
			case Format::R8_UINT: return VK_FORMAT_R8_UINT;
			case Format::R8_SINT: return VK_FORMAT_R8_SINT;
			case Format::R8_SRGB: return VK_FORMAT_R8_SRGB;
			case Format::R8_UNORM: return VK_FORMAT_R8_UNORM;
			case Format::R8_SNORM: return VK_FORMAT_R8_SNORM;
			case Format::RG8_UINT: return VK_FORMAT_R8G8_UINT;
			case Format::RG8_SINT: return VK_FORMAT_R8G8_SINT;
			case Format::RG8_SRGB: return VK_FORMAT_R8G8_SRGB;
			case Format::RG8_UNORM: return VK_FORMAT_R8G8_UNORM;
			case Format::RG8_SNORM: return VK_FORMAT_R8G8_SNORM;
			case Format::RGB8_UINT: return VK_FORMAT_R8G8B8_UINT;
			case Format::RGB8_SINT: return VK_FORMAT_R8G8B8_SINT;
			case Format::RGB8_SRGB: return VK_FORMAT_R8G8B8_SRGB;
			case Format::RGB8_UNORM: return VK_FORMAT_R8G8B8_UNORM;
			case Format::RGB8_SNORM: return VK_FORMAT_R8G8B8_SNORM;
			case Format::RGBA8_UINT: return VK_FORMAT_R8G8B8A8_UINT;
			case Format::RGBA8_SINT: return VK_FORMAT_R8G8B8A8_SINT;
			case Format::RGBA8_SRGB: return VK_FORMAT_R8G8B8A8_SRGB;
			case Format::RGBA8_UNORM: return VK_FORMAT_R8G8B8A8_UNORM;
			case Format::RGBA8_SNORM: return VK_FORMAT_R8G8B8A8_SNORM;

			case Format::R16_UINT: return VK_FORMAT_R16_UINT;
			case Format::R16_SINT: return VK_FORMAT_R16_SINT;
			case Format::R16_UNORM: return VK_FORMAT_R16_UNORM;
			case Format::R16_SNORM: return VK_FORMAT_R16_SNORM;
			case Format::R16_SFLOAT: return VK_FORMAT_R16_SFLOAT;
			case Format::RG16_UINT: return VK_FORMAT_R16G16_UINT;
			case Format::RG16_SINT: return VK_FORMAT_R16G16_SINT;
			case Format::RG16_UNORM: return VK_FORMAT_R16G16_UNORM;
			case Format::RG16_SNORM: return VK_FORMAT_R16G16_SNORM;
			case Format::RG16_SFLOAT: return VK_FORMAT_R16G16_SFLOAT;
			case Format::RGB16_UINT: return VK_FORMAT_R16G16B16_UINT;
			case Format::RGB16_SINT: return VK_FORMAT_R16G16B16_SINT;
			case Format::RGB16_UNORM: return VK_FORMAT_R16G16B16_UNORM;
			case Format::RGB16_SNORM: return VK_FORMAT_R16G16B16_SNORM;
			case Format::RGB16_SFLOAT: return VK_FORMAT_R16G16B16_SFLOAT;
			case Format::RGBA16_UINT: return VK_FORMAT_R16G16B16A16_UINT;
			case Format::RGBA16_SINT: return VK_FORMAT_R16G16B16A16_SINT;
			case Format::RGBA16_UNORM: return VK_FORMAT_R16G16B16A16_UNORM;
			case Format::RGBA16_SNORM: return VK_FORMAT_R16G16B16A16_SNORM;
			case Format::RGBA16_SFLOAT: return VK_FORMAT_R16G16B16A16_SFLOAT;

			case Format::R32_UINT: return VK_FORMAT_R32_UINT;
			case Format::R32_SINT: return VK_FORMAT_R32_SINT;
			case Format::R32_SFLOAT: return VK_FORMAT_R32_SFLOAT;
			case Format::RG32_UINT: return VK_FORMAT_R32G32_UINT;
			case Format::RG32_SINT: return VK_FORMAT_R32G32_SINT;
			case Format::RG32_SFLOAT: return VK_FORMAT_R32G32_SFLOAT;
			case Format::RGB32_UINT: return VK_FORMAT_R32G32B32_UINT;
			case Format::RGB32_SINT: return VK_FORMAT_R32G32B32_SINT;
			case Format::RGB32_SFLOAT: return VK_FORMAT_R32G32B32_SFLOAT;
			case Format::RGBA32_UINT: return VK_FORMAT_R32G32B32A32_UINT;
			case Format::RGBA32_SINT: return VK_FORMAT_R32G32B32A32_SINT;
			case Format::RGBA32_SFLOAT: return VK_FORMAT_R32G32B32A32_SFLOAT;

			case Format::R64_UINT: return VK_FORMAT_R64_UINT;
			case Format::R64_SINT: return VK_FORMAT_R64_SINT;
			case Format::R64_SFLOAT: return VK_FORMAT_R64_SFLOAT;
			case Format::RG64_UINT: return VK_FORMAT_R64G64_UINT;
			case Format::RG64_SINT: return VK_FORMAT_R64G64_SINT;
			case Format::RG64_SFLOAT: return VK_FORMAT_R64G64_SFLOAT;
			case Format::RGB64_UINT: return VK_FORMAT_R64G64B64_UINT;
			case Format::RGB64_SINT: return VK_FORMAT_R64G64B64_SINT;
			case Format::RGB64_SFLOAT: return VK_FORMAT_R64G64B64_SFLOAT;
			case Format::RGBA64_UINT: return VK_FORMAT_R64G64B64A64_UINT;
			case Format::RGBA64_SINT: return VK_FORMAT_R64G64B64A64_SINT;
			case Format::RGBA64_SFLOAT: return VK_FORMAT_R64G64B64A64_SFLOAT;

			case Format::D16_UNORM: return VK_FORMAT_D16_UNORM;
			case Format::D16_UNORM_S8_UINT: return VK_FORMAT_D16_UNORM_S8_UINT;
			case Format::D24_UNORM_S8_UINT: return VK_FORMAT_D24_UNORM_S8_UINT;
			case Format::D32_SFLOAT: return VK_FORMAT_D32_SFLOAT;
			case Format::D32_SFLOAT_S8_UINT: return VK_FORMAT_D32_SFLOAT_S8_UINT;

			case Format::None:
			default:
			{
				
			}
		}
	}

	constexpr PAWN_API VkImageLayout ConvertImageLayoutVulkan(Pawn::Render::ImageLayout layout)
	{
		switch (layout)
		{
			case ImageLayout::Undefined: return VK_IMAGE_LAYOUT_UNDEFINED;
			case ImageLayout::Present: return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
			case ImageLayout::ColorAttachment: return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			case ImageLayout::DepthStencilAttachment: return VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
			case ImageLayout::ShaderReadOnly: return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			case ImageLayout::TransferSrc: return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			case ImageLayout::TransferDst: return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			default:
			{
			}
		}
	}

	constexpr PAWN_API VkAttachmentLoadOp ConvertAttachmentLoadOperationVulkan(Pawn::Render::LoadOperation operation)
	{
		switch (operation)
		{
			case LoadOperation::None: return VK_ATTACHMENT_LOAD_OP_NONE;
			case LoadOperation::DontCare: return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			case LoadOperation::Load: return VK_ATTACHMENT_LOAD_OP_LOAD;
			case LoadOperation::Clear: return VK_ATTACHMENT_LOAD_OP_CLEAR;
			default:
			{
			}
		}
	}
	constexpr PAWN_API VkAttachmentStoreOp ConvertAttachmentStoreOperationVulkan(Pawn::Render::StoreOperation operation)
	{
		switch (operation)
		{
			case StoreOperation::None: return VK_ATTACHMENT_STORE_OP_NONE;
			case StoreOperation::DontCare: return VK_ATTACHMENT_STORE_OP_DONT_CARE;
			case StoreOperation::Store: return VK_ATTACHMENT_STORE_OP_STORE;
			default:
			{
			}
		}
	}

	constexpr PAWN_API VkPipelineBindPoint ConvertPipelineBindPointVulkan(Pawn::Render::PipelineBindPoint bindPoint)
	{
		switch (bindPoint)
		{
			case PipelineBindPoint::Graphics: return VK_PIPELINE_BIND_POINT_GRAPHICS;
			case PipelineBindPoint::Compute: return VK_PIPELINE_BIND_POINT_COMPUTE;
			case PipelineBindPoint::RayTracing: return VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR;
			case PipelineBindPoint::SubpassShading: return VK_PIPELINE_BIND_POINT_SUBPASS_SHADING_HUAWEI;
			case PipelineBindPoint::None:
			default:
			{
			}
		}
	}


	constexpr PAWN_API VkPipelineStageFlags ConvertPipelineStageFlagsVulkan(Pawn::Render::PipelineStageFlags flags)
	{
		int32 value = 0;
		if (static_cast<int32>(flags) == 0)
			value |= 0;
		if (static_cast<int32>(flags & PipelineStageFlags::TopOfPipe) != 0)
			value |= VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		if (static_cast<int32>(flags & PipelineStageFlags::BottomOfPipe) != 0)
			value |= VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
		if (static_cast<int32>(flags & PipelineStageFlags::DrawIndirect) != 0)
			value |= VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT;
		if (static_cast<int32>(flags & PipelineStageFlags::VertexInput) != 0)
			value |= VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;
		if (static_cast<int32>(flags & PipelineStageFlags::VertexShader) != 0)
			value |= VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;
		if (static_cast<int32>(flags & PipelineStageFlags::TesselationEvaluationShader) != 0)
			value |= VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT;
		if (static_cast<int32>(flags & PipelineStageFlags::TesselationControlShader) != 0)
			value |= VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT;
		if (static_cast<int32>(flags & PipelineStageFlags::GeometryShader) != 0)
			value |= VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT;
		if (static_cast<int32>(flags & PipelineStageFlags::FragmentShader) != 0)
			value |= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		if (static_cast<int32>(flags & PipelineStageFlags::EarlyFragmentTests) != 0)
			value |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		if (static_cast<int32>(flags & PipelineStageFlags::LateFragmentTests) != 0)
			value |= VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
		if (static_cast<int32>(flags & PipelineStageFlags::ColorAttachmentOutput) != 0)
			value |= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		if (static_cast<int32>(flags & PipelineStageFlags::ComputeShader) != 0)
			value |= VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
		if (static_cast<int32>(flags & PipelineStageFlags::Transfer) != 0)
			value |= VK_PIPELINE_STAGE_TRANSFER_BIT;
		if (static_cast<int32>(flags & PipelineStageFlags::AllGraphics) != 0)
			value |= VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT;
		if (static_cast<int32>(flags & PipelineStageFlags::AllCommands) != 0)
			value |= VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
		if (static_cast<int32>(flags & PipelineStageFlags::RayTracing) != 0)
			value |= VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR;
		if (static_cast<int32>(flags & PipelineStageFlags::AccelerationStructure) != 0)
			value |= VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR;
		if (static_cast<int32>(flags & PipelineStageFlags::ConditionalRendering) != 0)
			value |= VK_PIPELINE_STAGE_CONDITIONAL_RENDERING_BIT_EXT;

		return static_cast<VkPipelineStageFlags>(value);
	}

	constexpr PAWN_API VkAccessFlags ConvertAccessFlagsVulkan(Pawn::Render::AccessFlags flags)
	{
		int32 value = 0;
		if (static_cast<int32>(flags) == 0)
			value |= 0;
		if (static_cast<int32>(flags & AccessFlags::IndirectCommandRead) != 0)
			value |= VK_ACCESS_2_INDIRECT_COMMAND_READ_BIT;
		if (static_cast<int32>(flags & AccessFlags::IndexRead) != 0)
			value |= VK_ACCESS_2_INDEX_READ_BIT;
		if (static_cast<int32>(flags & AccessFlags::VertexAttributeRead) != 0)
			value |= VK_ACCESS_2_VERTEX_ATTRIBUTE_READ_BIT;
		if (static_cast<int32>(flags & AccessFlags::UniformRead) != 0)
			value |= VK_ACCESS_2_UNIFORM_READ_BIT;
		if (static_cast<int32>(flags & AccessFlags::InputAttachmentRead) != 0)
			value |= VK_ACCESS_2_INPUT_ATTACHMENT_READ_BIT;
		if (static_cast<int32>(flags & AccessFlags::ShaderRead) != 0)
			value |= VK_ACCESS_2_SHADER_READ_BIT;
		if (static_cast<int32>(flags & AccessFlags::ShaderWrite) != 0)
			value |= VK_ACCESS_2_SHADER_WRITE_BIT;
		if (static_cast<int32>(flags & AccessFlags::ColorAttachmentRead) != 0)
			value |= VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT;
		if (static_cast<int32>(flags & AccessFlags::ColorAttachmentWrite) != 0)
			value |= VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
		if (static_cast<int32>(flags & AccessFlags::DepthStencilRead) != 0)
			value |= VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
		if (static_cast<int32>(flags & AccessFlags::DepthStencilWrite) != 0)
			value |= VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		if (static_cast<int32>(flags & AccessFlags::TransferRead) != 0)
			value |= VK_ACCESS_2_TRANSFER_READ_BIT;
		if (static_cast<int32>(flags & AccessFlags::TransferWrite) != 0)
			value |= VK_ACCESS_2_TRANSFER_WRITE_BIT;
		if (static_cast<int32>(flags & AccessFlags::HostRead) != 0)
			value |= VK_ACCESS_2_HOST_READ_BIT;
		if (static_cast<int32>(flags & AccessFlags::HostWrite) != 0)
			value |= VK_ACCESS_2_HOST_WRITE_BIT;
		if (static_cast<int32>(flags & AccessFlags::MemoryRead) != 0)
			value |= VK_ACCESS_2_MEMORY_READ_BIT;
		if (static_cast<int32>(flags & AccessFlags::MemoryWrite) != 0)
			value |= VK_ACCESS_2_MEMORY_WRITE_BIT;

		return static_cast<VkAccessFlags>(value);
	}

	constexpr Pawn::Render::Format ConvertFormatEngine(VkFormat format)
	{
		switch (format)
		{
			case VK_FORMAT_R8_UINT: return Format::R8_UINT;
			case VK_FORMAT_R8_SINT: return Format::R8_SINT;
			case VK_FORMAT_R8_SRGB: return Format::R8_SRGB;
			case VK_FORMAT_R8_UNORM: return Format::R8_UNORM;
			case VK_FORMAT_R8_SNORM: return Format::R8_SNORM;
			case VK_FORMAT_R8G8_UINT: return Format::RG8_UINT;
			case VK_FORMAT_R8G8_SINT: return Format::RG8_SINT;
			case VK_FORMAT_R8G8_SRGB: return Format::RG8_SRGB;
			case VK_FORMAT_R8G8_UNORM: return Format::RG8_UNORM;
			case VK_FORMAT_R8G8_SNORM: return Format::RG8_SNORM;
			case VK_FORMAT_R8G8B8_UINT: return Format::RGB8_UINT;
			case VK_FORMAT_R8G8B8_SINT: return Format::RGB8_SINT;
			case VK_FORMAT_R8G8B8_SRGB: return Format::RGB8_SRGB;
			case VK_FORMAT_R8G8B8_UNORM: return Format::RGB8_UNORM;
			case VK_FORMAT_R8G8B8_SNORM: return Format::RGB8_SNORM;
			case VK_FORMAT_R8G8B8A8_UINT: return Format::RGBA8_UINT;
			case VK_FORMAT_R8G8B8A8_SINT: return Format::RGBA8_SINT;
			case VK_FORMAT_R8G8B8A8_SRGB: return Format::RGBA8_SRGB;
			case VK_FORMAT_R8G8B8A8_UNORM: return Format::RGBA8_UNORM;
			case VK_FORMAT_R8G8B8A8_SNORM: return Format::RGBA8_SNORM;

			case VK_FORMAT_R16_UINT: return Format::R16_UINT;
			case VK_FORMAT_R16_SINT: return Format::R16_SINT;
			case VK_FORMAT_R16_UNORM: return Format::R16_UNORM;
			case VK_FORMAT_R16_SNORM: return Format::R16_SNORM;
			case VK_FORMAT_R16_SFLOAT: return Format::R16_SFLOAT;
			case VK_FORMAT_R16G16_UINT: return Format::RG16_UINT;
			case VK_FORMAT_R16G16_SINT: return Format::RG16_SINT;
			case VK_FORMAT_R16G16_UNORM: return Format::RG16_UNORM;
			case VK_FORMAT_R16G16_SNORM: return Format::RG16_SNORM;
			case VK_FORMAT_R16G16_SFLOAT: return Format::RG16_SFLOAT;
			case VK_FORMAT_R16G16B16_UINT: return Format::RGB16_UINT;
			case VK_FORMAT_R16G16B16_SINT: return Format::RGB16_SINT;
			case VK_FORMAT_R16G16B16_UNORM: return Format::RGB16_UNORM;
			case VK_FORMAT_R16G16B16_SNORM: return Format::RGB16_SNORM;
			case VK_FORMAT_R16G16B16_SFLOAT: return Format::RGB16_SFLOAT;
			case VK_FORMAT_R16G16B16A16_UINT: return Format::RGBA16_UINT;
			case VK_FORMAT_R16G16B16A16_SINT: return Format::RGBA16_SINT;
			case VK_FORMAT_R16G16B16A16_UNORM: return Format::RGBA16_UNORM;
			case VK_FORMAT_R16G16B16A16_SNORM: return Format::RGBA16_SNORM;
			case VK_FORMAT_R16G16B16A16_SFLOAT: return Format::RGBA16_SFLOAT;

			case VK_FORMAT_R32_UINT: return Format::R32_UINT;
			case VK_FORMAT_R32_SINT: return Format::R32_SINT;
			case VK_FORMAT_R32_SFLOAT: return Format::R32_SFLOAT;
			case VK_FORMAT_R32G32_UINT: return Format::RG32_UINT;
			case VK_FORMAT_R32G32_SINT: return Format::RG32_SINT;
			case VK_FORMAT_R32G32_SFLOAT: return Format::RG32_SFLOAT;
			case VK_FORMAT_R32G32B32_UINT: return Format::RGB32_UINT;
			case VK_FORMAT_R32G32B32_SINT: return Format::RGB32_SINT;
			case VK_FORMAT_R32G32B32_SFLOAT: return Format::RGB32_SFLOAT;
			case VK_FORMAT_R32G32B32A32_UINT: return Format::RGBA32_UINT;
			case VK_FORMAT_R32G32B32A32_SINT: return Format::RGBA32_SINT;
			case VK_FORMAT_R32G32B32A32_SFLOAT: return Format::RGBA32_SFLOAT;

			case VK_FORMAT_R64_UINT: return Format::R64_UINT;
			case VK_FORMAT_R64_SINT: return Format::R64_SINT;
			case VK_FORMAT_R64_SFLOAT: return Format::R64_SFLOAT;
			case VK_FORMAT_R64G64_UINT: return Format::RG64_UINT;
			case VK_FORMAT_R64G64_SINT: return Format::RG64_SINT;
			case VK_FORMAT_R64G64_SFLOAT: return Format::RG64_SFLOAT;
			case VK_FORMAT_R64G64B64_UINT: return Format::RGB64_UINT;
			case VK_FORMAT_R64G64B64_SINT: return Format::RGB64_SINT;
			case VK_FORMAT_R64G64B64_SFLOAT: return Format::RGB64_SFLOAT;
			case VK_FORMAT_R64G64B64A64_UINT: return Format::RGBA64_UINT;
			case VK_FORMAT_R64G64B64A64_SINT: return Format::RGBA64_SINT;
			case VK_FORMAT_R64G64B64A64_SFLOAT: return Format::RGBA64_SFLOAT;

			case VK_FORMAT_D16_UNORM: return Format::D16_UNORM;
			case VK_FORMAT_D16_UNORM_S8_UINT: return Format::D16_UNORM_S8_UINT;
			case VK_FORMAT_D24_UNORM_S8_UINT: return Format::D24_UNORM_S8_UINT;
			case VK_FORMAT_D32_SFLOAT: return  Format::D32_SFLOAT;
			case VK_FORMAT_D32_SFLOAT_S8_UINT: return Format::D32_SFLOAT_S8_UINT;
			default:
			{

			}
		}
	}

	constexpr Pawn::Render::ImageLayout ConvertImageLayoutEngine(VkImageLayout layout)
	{
		return ImageLayout::Undefined;
	}

	constexpr Pawn::Render::LoadOperation ConvertAttachmentLoadOperationEngine(VkAttachmentLoadOp operation)
	{
		return LoadOperation::None;
	}

	constexpr Pawn::Render::StoreOperation ConvertAttachmentStoreOperationEngine(VkAttachmentStoreOp operation)
	{
		return StoreOperation::None;
	}

	constexpr Pawn::Render::PipelineBindPoint ConvertPipelineBindPointEngine(VkPipelineBindPoint operation)
	{
		return PipelineBindPoint::None;
	}

	constexpr Pawn::Render::PipelineStageFlags ConvertPipelineStageFlagsEngine(VkPipelineStageFlags flags)
	{
		return PipelineStageFlags::None;
	}

	constexpr Pawn::Render::AccessFlags ConvertAccessFlagsEngine(VkAccessFlags flags)
	{
		return AccessFlags::None;
	}


}
