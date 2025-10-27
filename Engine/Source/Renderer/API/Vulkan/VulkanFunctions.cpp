#include "VulkanFunctions.hpp"

namespace ME::Render
{
	constexpr VkFormat ConvertFormatVulkan(ME::Render::Format format)
	{
		switch (format)
		{
			// R 8-BIT
		case Format::R8_UINT: return VK_FORMAT_R8_UINT;
		case Format::R8_SINT: return VK_FORMAT_R8_SINT;
		case Format::R8_SRGB: return VK_FORMAT_R8_SRGB;
		case Format::R8_UNORM: return VK_FORMAT_R8_UNORM;
		case Format::R8_SNORM: return VK_FORMAT_R8_SNORM;
		case Format::R8_USCALED: return VK_FORMAT_R8_USCALED;
		case Format::R8_SSCALED: return VK_FORMAT_R8_SSCALED;
			// RG 8-BIT
		case Format::RG8_UINT: return VK_FORMAT_R8G8_UINT;
		case Format::RG8_SINT: return VK_FORMAT_R8G8_SINT;
		case Format::RG8_SRGB: return VK_FORMAT_R8G8_SRGB;
		case Format::RG8_UNORM: return VK_FORMAT_R8G8_UNORM;
		case Format::RG8_SNORM: return VK_FORMAT_R8G8_SNORM;
		case Format::RG8_USCALED: return VK_FORMAT_R8G8_USCALED;
		case Format::RG8_SSCALED: return VK_FORMAT_R8G8_SSCALED;
			// RGB 8-BIT
		case Format::RGB8_UINT: return VK_FORMAT_R8G8B8_UINT;
		case Format::RGB8_SINT: return VK_FORMAT_R8G8B8_SINT;
		case Format::RGB8_SRGB: return VK_FORMAT_R8G8B8_SRGB;
		case Format::RGB8_UNORM: return VK_FORMAT_R8G8B8_UNORM;
		case Format::RGB8_SNORM: return VK_FORMAT_R8G8B8_SNORM;
		case Format::RGB8_USCALED: return VK_FORMAT_R8G8B8_USCALED;
		case Format::RGB8_SSCALED: return VK_FORMAT_R8G8B8_SSCALED;
			// BGR 8-BIT
		case Format::BGR8_UINT: return VK_FORMAT_B8G8R8_UINT;
		case Format::BGR8_SINT: return VK_FORMAT_B8G8R8_SINT;
		case Format::BGR8_SRGB: return VK_FORMAT_B8G8R8_SRGB;
		case Format::BGR8_UNORM: return VK_FORMAT_B8G8R8_UNORM;
		case Format::BGR8_SNORM: return VK_FORMAT_B8G8R8_SNORM;
		case Format::BGR8_USCALED: return VK_FORMAT_B8G8R8_USCALED;
		case Format::BGR8_SSCALED: return VK_FORMAT_B8G8R8_SSCALED;
			// RGBA 8-BIT
		case Format::RGBA8_UINT: return VK_FORMAT_R8G8B8A8_UINT;
		case Format::RGBA8_SINT: return VK_FORMAT_R8G8B8A8_SINT;
		case Format::RGBA8_SRGB: return VK_FORMAT_R8G8B8A8_SRGB;
		case Format::RGBA8_UNORM: return VK_FORMAT_R8G8B8A8_UNORM;
		case Format::RGBA8_SNORM: return VK_FORMAT_R8G8B8A8_SNORM;
		case Format::RGBA8_USCALED: return VK_FORMAT_R8G8B8A8_USCALED;
		case Format::RGBA8_SSCALED: return VK_FORMAT_R8G8B8A8_SSCALED;
			// BGRA 8-BIT
		case Format::BGRA8_UINT: return VK_FORMAT_B8G8R8A8_UINT;
		case Format::BGRA8_SINT: return VK_FORMAT_B8G8R8A8_SINT;
		case Format::BGRA8_SRGB: return VK_FORMAT_B8G8R8A8_SRGB;
		case Format::BGRA8_UNORM: return VK_FORMAT_B8G8R8A8_UNORM;
		case Format::BGRA8_SNORM: return VK_FORMAT_B8G8R8A8_SNORM;
		case Format::BGRA8_USCALED: return VK_FORMAT_B8G8R8A8_USCALED;
		case Format::BGRA8_SSCALED: return VK_FORMAT_B8G8R8A8_SSCALED;

			// R 16-BIT
		case Format::R16_UINT: return VK_FORMAT_R16_UINT;
		case Format::R16_SINT: return VK_FORMAT_R16_SINT;
		case Format::R16_UNORM: return VK_FORMAT_R16_UNORM;
		case Format::R16_SNORM: return VK_FORMAT_R16_SNORM;
		case Format::R16_SFLOAT: return VK_FORMAT_R16_SFLOAT;
		case Format::R16_USCALED: return VK_FORMAT_R16_USCALED;
		case Format::R16_SSCALED: return VK_FORMAT_R16_SSCALED;
			// RG 16-BIT
		case Format::RG16_UINT: return VK_FORMAT_R16G16_UINT;
		case Format::RG16_SINT: return VK_FORMAT_R16G16_SINT;
		case Format::RG16_UNORM: return VK_FORMAT_R16G16_UNORM;
		case Format::RG16_SNORM: return VK_FORMAT_R16G16_SNORM;
		case Format::RG16_SFLOAT: return VK_FORMAT_R16G16_SFLOAT;
		case Format::RG16_USCALED: return VK_FORMAT_R16G16_USCALED;
		case Format::RG16_SSCALED: return VK_FORMAT_R16G16_SSCALED;
			// RGB 16-BIT
		case Format::RGB16_UINT: return VK_FORMAT_R16G16B16_UINT;
		case Format::RGB16_SINT: return VK_FORMAT_R16G16B16_SINT;
		case Format::RGB16_UNORM: return VK_FORMAT_R16G16B16_UNORM;
		case Format::RGB16_SNORM: return VK_FORMAT_R16G16B16_SNORM;
		case Format::RGB16_SFLOAT: return VK_FORMAT_R16G16B16_SFLOAT;
		case Format::RGB16_USCALED: return VK_FORMAT_R16G16B16_USCALED;
		case Format::RGB16_SSCALED: return VK_FORMAT_R16G16B16_SSCALED;
			// RGBA 16-BIT
		case Format::RGBA16_UINT: return VK_FORMAT_R16G16B16A16_UINT;
		case Format::RGBA16_SINT: return VK_FORMAT_R16G16B16A16_SINT;
		case Format::RGBA16_UNORM: return VK_FORMAT_R16G16B16A16_UNORM;
		case Format::RGBA16_SNORM: return VK_FORMAT_R16G16B16A16_SNORM;
		case Format::RGBA16_SFLOAT: return VK_FORMAT_R16G16B16A16_SFLOAT;
		case Format::RGBA16_USCALED: return VK_FORMAT_R16G16B16A16_USCALED;
		case Format::RGBA16_SSCALED: return VK_FORMAT_R16G16B16A16_SSCALED;

			// R 32-BIT
		case Format::R32_UINT: return VK_FORMAT_R32_UINT;
		case Format::R32_SINT: return VK_FORMAT_R32_SINT;
		case Format::R32_SFLOAT: return VK_FORMAT_R32_SFLOAT;
			// RG 32-BIT
		case Format::RG32_UINT: return VK_FORMAT_R32G32_UINT;
		case Format::RG32_SINT: return VK_FORMAT_R32G32_SINT;
		case Format::RG32_SFLOAT: return VK_FORMAT_R32G32_SFLOAT;
			// RGB 32-BIT
		case Format::RGB32_UINT: return VK_FORMAT_R32G32B32_UINT;
		case Format::RGB32_SINT: return VK_FORMAT_R32G32B32_SINT;
		case Format::RGB32_SFLOAT: return VK_FORMAT_R32G32B32_SFLOAT;
			// RGBA 32-BIT
		case Format::RGBA32_UINT: return VK_FORMAT_R32G32B32A32_UINT;
		case Format::RGBA32_SINT: return VK_FORMAT_R32G32B32A32_SINT;
		case Format::RGBA32_SFLOAT: return VK_FORMAT_R32G32B32A32_SFLOAT;

			// R 64-BIT
		case Format::R64_UINT: return VK_FORMAT_R64_UINT;
		case Format::R64_SINT: return VK_FORMAT_R64_SINT;
		case Format::R64_SFLOAT: return VK_FORMAT_R64_SFLOAT;
			// RG 64-BIT
		case Format::RG64_UINT: return VK_FORMAT_R64G64_UINT;
		case Format::RG64_SINT: return VK_FORMAT_R64G64_SINT;
		case Format::RG64_SFLOAT: return VK_FORMAT_R64G64_SFLOAT;
			// RGB 64-BIT
		case Format::RGB64_UINT: return VK_FORMAT_R64G64B64_UINT;
		case Format::RGB64_SINT: return VK_FORMAT_R64G64B64_SINT;
		case Format::RGB64_SFLOAT: return VK_FORMAT_R64G64B64_SFLOAT;
			// RGBA 64-BIT
		case Format::RGBA64_UINT: return VK_FORMAT_R64G64B64A64_UINT;
		case Format::RGBA64_SINT: return VK_FORMAT_R64G64B64A64_SINT;
		case Format::RGBA64_SFLOAT: return VK_FORMAT_R64G64B64A64_SFLOAT;

		case Format::D16_UNORM: return VK_FORMAT_D16_UNORM;
		case Format::D16_UNORM_S8_UINT: return VK_FORMAT_D16_UNORM_S8_UINT;
		case Format::D24_UNORM_S8_UINT: return VK_FORMAT_D24_UNORM_S8_UINT;
		case Format::D32_SFLOAT: return VK_FORMAT_D32_SFLOAT;
		case Format::D32_SFLOAT_S8_UINT: return VK_FORMAT_D32_SFLOAT_S8_UINT;

		case Format::None:
            return VK_FORMAT_UNDEFINED;
		default:
            return VK_FORMAT_UNDEFINED;
		}
	}

	constexpr VkImageLayout ConvertImageLayoutVulkan(ME::Render::ImageLayout layout)
	{
		switch (layout)
		{
			case ImageLayout::Undefined:
			    return VK_IMAGE_LAYOUT_UNDEFINED;
			case ImageLayout::Present:
			    return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
			case ImageLayout::ColorAttachment:
			    return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
			case ImageLayout::DepthStencilAttachment:
			    return VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
			case ImageLayout::ShaderReadOnly:
			    return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			case ImageLayout::TransferSrc:
			    return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			case ImageLayout::TransferDst:
			    return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			default:
			    return VK_IMAGE_LAYOUT_MAX_ENUM;
		}
	}

	constexpr VkAttachmentLoadOp ConvertAttachmentLoadOperationVulkan(ME::Render::LoadOperation operation)
	{
		switch (operation)
		{
			case LoadOperation::None:
			    return VK_ATTACHMENT_LOAD_OP_NONE;
			case LoadOperation::DontCare:
			    return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
			case LoadOperation::Load:
			    return VK_ATTACHMENT_LOAD_OP_LOAD;
			case LoadOperation::Clear:
			    return VK_ATTACHMENT_LOAD_OP_CLEAR;
			default:
			    return VK_ATTACHMENT_LOAD_OP_MAX_ENUM;
		}
	}
	constexpr VkAttachmentStoreOp ConvertAttachmentStoreOperationVulkan(ME::Render::StoreOperation operation)
	{
		switch (operation)
		{
			case StoreOperation::None:
			    return VK_ATTACHMENT_STORE_OP_NONE;
			case StoreOperation::DontCare:
			    return VK_ATTACHMENT_STORE_OP_DONT_CARE;
			case StoreOperation::Store:
			    return VK_ATTACHMENT_STORE_OP_STORE;
			default:
			    return VK_ATTACHMENT_STORE_OP_MAX_ENUM;
		}
	}

	constexpr VkPipelineBindPoint ConvertPipelineBindPointVulkan(ME::Render::PipelineBindPoint bindPoint)
	{
		switch (bindPoint)
		{
			case PipelineBindPoint::Graphics:
		        return VK_PIPELINE_BIND_POINT_GRAPHICS;
			case PipelineBindPoint::Compute:
		        return VK_PIPELINE_BIND_POINT_COMPUTE;
			case PipelineBindPoint::RayTracing:
		        return VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR;
			case PipelineBindPoint::SubpassShading:
		        return VK_PIPELINE_BIND_POINT_SUBPASS_SHADING_HUAWEI;
            case PipelineBindPoint::None:
		        return VK_PIPELINE_BIND_POINT_MAX_ENUM;
			default:
		        return VK_PIPELINE_BIND_POINT_MAX_ENUM;
		}
	}


	constexpr VkPipelineStageFlags ConvertPipelineStageFlagsVulkan(ME::Render::PipelineStageFlags flags)
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

	constexpr VkAccessFlags ConvertAccessFlagsVulkan(ME::Render::AccessFlags flags)
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

	constexpr ME::Render::Format ConvertFormatEngine(VkFormat format)
	{
		switch (format)
		{
			    // R 8-BIT
			case VK_FORMAT_R8_UINT: return Format::R8_UINT;
			case VK_FORMAT_R8_SINT: return Format::R8_SINT;
			case VK_FORMAT_R8_SRGB: return Format::R8_SRGB;
			case VK_FORMAT_R8_UNORM: return Format::R8_UNORM;
			case VK_FORMAT_R8_SNORM: return Format::R8_SNORM;
			case VK_FORMAT_R8_SSCALED: return Format::R8_SSCALED;
			case VK_FORMAT_R8_USCALED: return Format::R8_USCALED;
				// RG 8-BIT
			case VK_FORMAT_R8G8_UINT: return Format::RG8_UINT;
			case VK_FORMAT_R8G8_SINT: return Format::RG8_SINT;
			case VK_FORMAT_R8G8_SRGB: return Format::RG8_SRGB;
			case VK_FORMAT_R8G8_UNORM: return Format::RG8_UNORM;
			case VK_FORMAT_R8G8_SNORM: return Format::RG8_SNORM;
			case VK_FORMAT_R8G8_SSCALED: return Format::RG8_SSCALED;
			case VK_FORMAT_R8G8_USCALED: return Format::RG8_USCALED;
				// RGB 8-BIT
			case VK_FORMAT_R8G8B8_UINT: return Format::RGB8_UINT;
			case VK_FORMAT_R8G8B8_SINT: return Format::RGB8_SINT;
			case VK_FORMAT_R8G8B8_SRGB: return Format::RGB8_SRGB;
			case VK_FORMAT_R8G8B8_UNORM: return Format::RGB8_UNORM;
			case VK_FORMAT_R8G8B8_SNORM: return Format::RGB8_SNORM;
			case VK_FORMAT_R8G8B8_SSCALED: return Format::RGB8_SSCALED;
			case VK_FORMAT_R8G8B8_USCALED: return Format::RGB8_USCALED;
				// BGR 8-BIT
			case VK_FORMAT_B8G8R8_UINT: return Format::BGR8_UINT;
			case VK_FORMAT_B8G8R8_SINT: return Format::BGR8_SINT;
			case VK_FORMAT_B8G8R8_SRGB: return Format::BGR8_SRGB;
			case VK_FORMAT_B8G8R8_UNORM: return Format::BGR8_UNORM;
			case VK_FORMAT_B8G8R8_SNORM: return Format::BGR8_SNORM;
			case VK_FORMAT_B8G8R8_USCALED: return Format::BGR8_USCALED;
			case VK_FORMAT_B8G8R8_SSCALED: return Format::BGR8_SSCALED;
				// RGBA 8-BIT
			case VK_FORMAT_R8G8B8A8_UINT: return Format::RGBA8_UINT;
			case VK_FORMAT_R8G8B8A8_SINT: return Format::RGBA8_SINT;
			case VK_FORMAT_R8G8B8A8_SRGB: return Format::RGBA8_SRGB;
			case VK_FORMAT_R8G8B8A8_UNORM: return Format::RGBA8_UNORM;
			case VK_FORMAT_R8G8B8A8_SNORM: return Format::RGBA8_SNORM;
			case VK_FORMAT_R8G8B8A8_USCALED: return Format::RGBA8_USCALED;
			case VK_FORMAT_R8G8B8A8_SSCALED: return Format::RGBA8_SSCALED;
				// BGRA 8-BIT
			case VK_FORMAT_B8G8R8A8_UINT: return Format::BGRA8_UINT;
			case VK_FORMAT_B8G8R8A8_SINT: return Format::BGRA8_SINT;
			case VK_FORMAT_B8G8R8A8_SRGB: return Format::BGRA8_SRGB;
			case VK_FORMAT_B8G8R8A8_UNORM: return Format::BGRA8_UNORM;
			case VK_FORMAT_B8G8R8A8_SNORM: return Format::BGRA8_SNORM;
			case VK_FORMAT_B8G8R8A8_USCALED: return Format::BGRA8_USCALED;
			case VK_FORMAT_B8G8R8A8_SSCALED: return Format::BGRA8_SSCALED;

				// R 16-BIT
			case VK_FORMAT_R16_UINT: return Format::R16_UINT;
			case VK_FORMAT_R16_SINT: return Format::R16_SINT;
			case VK_FORMAT_R16_UNORM: return Format::R16_UNORM;
			case VK_FORMAT_R16_SNORM: return Format::R16_SNORM;
			case VK_FORMAT_R16_SFLOAT: return Format::R16_SFLOAT;
			case VK_FORMAT_R16_USCALED: return Format::R16_USCALED;
			case VK_FORMAT_R16_SSCALED: return Format::R16_SSCALED;
				// RG 16-BIT
			case VK_FORMAT_R16G16_UINT: return Format::RG16_UINT;
			case VK_FORMAT_R16G16_SINT: return Format::RG16_SINT;
			case VK_FORMAT_R16G16_UNORM: return Format::RG16_UNORM;
			case VK_FORMAT_R16G16_SNORM: return Format::RG16_SNORM;
			case VK_FORMAT_R16G16_SFLOAT: return Format::RG16_SFLOAT;
			case VK_FORMAT_R16G16_USCALED: return Format::RG16_USCALED;
			case VK_FORMAT_R16G16_SSCALED: return Format::RG16_SSCALED;
				// RGB 16-BIT
			case VK_FORMAT_R16G16B16_UINT: return Format::RGB16_UINT;
			case VK_FORMAT_R16G16B16_SINT: return Format::RGB16_SINT;
			case VK_FORMAT_R16G16B16_UNORM: return Format::RGB16_UNORM;
			case VK_FORMAT_R16G16B16_SNORM: return Format::RGB16_SNORM;
			case VK_FORMAT_R16G16B16_SFLOAT: return Format::RGB16_SFLOAT;
			case VK_FORMAT_R16G16B16_USCALED: return Format::RGB16_USCALED;
			case VK_FORMAT_R16G16B16_SSCALED: return Format::RGB16_SSCALED;
				// RGBA 16-BIT
			case VK_FORMAT_R16G16B16A16_UINT: return Format::RGBA16_UINT;
			case VK_FORMAT_R16G16B16A16_SINT: return Format::RGBA16_SINT;
			case VK_FORMAT_R16G16B16A16_UNORM: return Format::RGBA16_UNORM;
			case VK_FORMAT_R16G16B16A16_SNORM: return Format::RGBA16_SNORM;
			case VK_FORMAT_R16G16B16A16_SFLOAT: return Format::RGBA16_SFLOAT;
			case VK_FORMAT_R16G16B16A16_USCALED: return Format::RGBA16_USCALED;
			case VK_FORMAT_R16G16B16A16_SSCALED: return Format::RGBA16_SSCALED;

				// R 32-BIT
			case VK_FORMAT_R32_UINT: return Format::R32_UINT;
			case VK_FORMAT_R32_SINT: return Format::R32_SINT;
			case VK_FORMAT_R32_SFLOAT: return Format::R32_SFLOAT;
				// RG 32-BIT
			case VK_FORMAT_R32G32_UINT: return Format::RG32_UINT;
			case VK_FORMAT_R32G32_SINT: return Format::RG32_SINT;
			case VK_FORMAT_R32G32_SFLOAT: return Format::RG32_SFLOAT;
				// RGB 32-BIT
			case VK_FORMAT_R32G32B32_UINT: return Format::RGB32_UINT;
			case VK_FORMAT_R32G32B32_SINT: return Format::RGB32_SINT;
			case VK_FORMAT_R32G32B32_SFLOAT: return Format::RGB32_SFLOAT;
				// RGBA 32-BIT
			case VK_FORMAT_R32G32B32A32_UINT: return Format::RGBA32_UINT;
			case VK_FORMAT_R32G32B32A32_SINT: return Format::RGBA32_SINT;
			case VK_FORMAT_R32G32B32A32_SFLOAT: return Format::RGBA32_SFLOAT;

				// R 64-BIT
			case VK_FORMAT_R64_UINT: return Format::R64_UINT;
			case VK_FORMAT_R64_SINT: return Format::R64_SINT;
			case VK_FORMAT_R64_SFLOAT: return Format::R64_SFLOAT;
				// RG 64-BIT
			case VK_FORMAT_R64G64_UINT: return Format::RG64_UINT;
			case VK_FORMAT_R64G64_SINT: return Format::RG64_SINT;
			case VK_FORMAT_R64G64_SFLOAT: return Format::RG64_SFLOAT;
				// RGB 64-BIT
			case VK_FORMAT_R64G64B64_UINT: return Format::RGB64_UINT;
			case VK_FORMAT_R64G64B64_SINT: return Format::RGB64_SINT;
			case VK_FORMAT_R64G64B64_SFLOAT: return Format::RGB64_SFLOAT;
				// RGBA 64-BIT
			case VK_FORMAT_R64G64B64A64_UINT: return Format::RGBA64_UINT;
			case VK_FORMAT_R64G64B64A64_SINT: return Format::RGBA64_SINT;
			case VK_FORMAT_R64G64B64A64_SFLOAT: return Format::RGBA64_SFLOAT;

			case VK_FORMAT_D16_UNORM: return Format::D16_UNORM;
			case VK_FORMAT_D16_UNORM_S8_UINT: return Format::D16_UNORM_S8_UINT;
			case VK_FORMAT_D24_UNORM_S8_UINT: return Format::D24_UNORM_S8_UINT;
			case VK_FORMAT_D32_SFLOAT: return  Format::D32_SFLOAT;
			case VK_FORMAT_D32_SFLOAT_S8_UINT: return Format::D32_SFLOAT_S8_UINT;
        }
        return Format::None;
	}

	constexpr VkPrimitiveTopology ConvertPrimitiveTopologyVulkan(ME::Render::PrimitiveTopology topology)
	{
		ME_ASSERT(topology != PrimitiveTopology::None, "Invalid PrimitiveTopology: None");
		switch (topology)
		{
			case PrimitiveTopology::LineList:
		        return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
			case PrimitiveTopology::LineStrip:
		        return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
			case PrimitiveTopology::PatchList:
		        return VK_PRIMITIVE_TOPOLOGY_PATCH_LIST;
			case PrimitiveTopology::PointList:
		        return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
			case PrimitiveTopology::TriangleFan:
		        return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
			case PrimitiveTopology::TriangleList:
		        return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
			case PrimitiveTopology::TriangleStrip:
		        return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
			case PrimitiveTopology::None:
		        return VK_PRIMITIVE_TOPOLOGY_MAX_ENUM;
		}
	    return VK_PRIMITIVE_TOPOLOGY_MAX_ENUM;
	}

	constexpr VkPolygonMode ConvertRasterizerFillVulkan(ME::Render::RasterizationFill fill)
	{
		switch (fill)
		{
			case ME::Render::RasterizationFill::Fill:
				return VK_POLYGON_MODE_FILL;
			case ME::Render::RasterizationFill::Wireframe:
				return VK_POLYGON_MODE_LINE;
			case ME::Render::RasterizationFill::Point:
				return VK_POLYGON_MODE_POINT;
		}
        return VK_POLYGON_MODE_MAX_ENUM;
	}

	constexpr VkCullModeFlagBits ConvertRasterizerCullVulkan(ME::Render::RasterizationCull cull)
	{
		switch (cull)
		{
			case ME::Render::RasterizationCull::None:
				return VK_CULL_MODE_NONE;
			case ME::Render::RasterizationCull::Back:
				return VK_CULL_MODE_BACK_BIT;
			case ME::Render::RasterizationCull::Front:
				return VK_CULL_MODE_FRONT_BIT;
			case ME::Render::RasterizationCull::FrontAndBack:
				return VK_CULL_MODE_FRONT_AND_BACK;
		}
	    return VK_CULL_MODE_FLAG_BITS_MAX_ENUM;
	}

	constexpr VkSampleCountFlagBits ConvertSampleCountVulkan(ME::Render::SampleCount sampleCount)
	{
		ME_ASSERT(sampleCount != SampleCount::None, "Invalid SampleCount: None");

		switch (sampleCount)
		{
			case SampleCount::Count1:
				return VK_SAMPLE_COUNT_1_BIT;
			case SampleCount::Count2:
				return VK_SAMPLE_COUNT_2_BIT;
			case SampleCount::Count4:
				return VK_SAMPLE_COUNT_4_BIT;
			case SampleCount::Count8:
				return VK_SAMPLE_COUNT_8_BIT;
			case SampleCount::Count16:
				return VK_SAMPLE_COUNT_16_BIT;
			case SampleCount::Count32:
				return VK_SAMPLE_COUNT_32_BIT;
			case SampleCount::Count64:
				return VK_SAMPLE_COUNT_64_BIT;
            case SampleCount::None:
				return VK_SAMPLE_COUNT_FLAG_BITS_MAX_ENUM;
		}
        return VK_SAMPLE_COUNT_FLAG_BITS_MAX_ENUM;
	}

	constexpr VkCompareOp ConvertDepthComparisonVulkan(ME::Render::DepthComparison function)
	{
		ME_ASSERT(function != DepthComparison::None, "Invalid DepthComparison: None");
		switch (function)
		{
			case DepthComparison::Less:
				return VK_COMPARE_OP_LESS;
			case DepthComparison::Greater:
				return VK_COMPARE_OP_LESS;
			case DepthComparison::LessEqual:
				return VK_COMPARE_OP_LESS_OR_EQUAL;
			case DepthComparison::GreaterEqual:
				return VK_COMPARE_OP_GREATER_OR_EQUAL;
			case DepthComparison::NotEqual:
				return VK_COMPARE_OP_NOT_EQUAL;
			case DepthComparison::Equal:
				return VK_COMPARE_OP_EQUAL;
			case DepthComparison::Always:
				return VK_COMPARE_OP_ALWAYS;
			case DepthComparison::Never:
				return VK_COMPARE_OP_NEVER;
            case DepthComparison::None:
				return VK_COMPARE_OP_MAX_ENUM;
		}
	    return VK_COMPARE_OP_MAX_ENUM;
	}

	constexpr VkLogicOp ConvertLogicOperationVulkan(ME::Render::LogicOperation operation)
	{
		switch (operation)
		{
			case LogicOperation::None:
				return VK_LOGIC_OP_NO_OP;
			case LogicOperation::Clear:
				return VK_LOGIC_OP_CLEAR;
			case LogicOperation::Copy:
				return VK_LOGIC_OP_COPY;
			case LogicOperation::Set:
				return VK_LOGIC_OP_SET;
			case LogicOperation::CopyInverted:
				return VK_LOGIC_OP_COPY_INVERTED;
			case LogicOperation::And:
				return VK_LOGIC_OP_AND;
			case LogicOperation::AndReverse:
				return VK_LOGIC_OP_AND_REVERSE;
			case LogicOperation::AndInverted:
				return VK_LOGIC_OP_AND_INVERTED;
			case LogicOperation::Nand:
				return VK_LOGIC_OP_NAND;
			case LogicOperation::Xor:
				return VK_LOGIC_OP_XOR;
			case LogicOperation::Or:
				return VK_LOGIC_OP_OR;
			case LogicOperation::Nor:
				return VK_LOGIC_OP_NOR;
			case LogicOperation::OrReverse:
				return VK_LOGIC_OP_OR_REVERSE;
			case LogicOperation::OrInverted:
				return VK_LOGIC_OP_OR_INVERTED;
			case LogicOperation::Equivalent:
				return VK_LOGIC_OP_EQUIVALENT;
			case LogicOperation::Invert:
				return VK_LOGIC_OP_INVERT;
		}
	    return VK_LOGIC_OP_MAX_ENUM;
	}

	constexpr VkDescriptorType ConvertResourceTypeVulkan(ME::Render::ResourceType type)
	{
		switch (type)
		{
			case ResourceType::None:
				return VK_DESCRIPTOR_TYPE_MAX_ENUM;
			case ResourceType::Uniform:
				return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			case ResourceType::StorageBuffer:
				return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			case ResourceType::Texture1D:
				return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
			case ResourceType::Texture2D:
				return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
			case ResourceType::Texture3D:
				return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
			case ResourceType::Sampler:
				return VK_DESCRIPTOR_TYPE_SAMPLER;
		}
	    return VK_DESCRIPTOR_TYPE_MAX_ENUM;
	}

	constexpr VkShaderStageFlags ConvertShaderStageVulkan(ME::Render::ShaderStage stage)
	{
		VkShaderStageFlags flags = 0;
		if (static_cast<int32>(stage & ShaderStage::Compute) != 0)
			flags |= VK_SHADER_STAGE_COMPUTE_BIT;
		if (static_cast<int32>(stage & ShaderStage::Vertex) != 0)
			flags |= VK_SHADER_STAGE_VERTEX_BIT;
		if (static_cast<int32>(stage & ShaderStage::Hull) != 0)
			flags |= VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
		if (static_cast<int32>(stage & ShaderStage::Domain) != 0)
			flags |= VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
		if (static_cast<int32>(stage & ShaderStage::Geometry) != 0)
			flags |= VK_SHADER_STAGE_GEOMETRY_BIT;
		if (static_cast<int32>(stage & ShaderStage::Pixel) != 0)
			flags |= VK_SHADER_STAGE_FRAGMENT_BIT;

		return flags;
	}

	constexpr VkShaderStageFlags ConvertImageUsageFlagsVulkan(ME::Render::ImageUsageFlags usage)
	{
		VkImageUsageFlags flags = 0;
		if (static_cast<int32>(usage & ImageUsageFlags::ColorAttachment) != 0)
			flags |= VkImageUsageFlagBits::VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		if (static_cast<int32>(usage & ImageUsageFlags::DepthStencilAttachment) != 0)
			flags |= VkImageUsageFlagBits::VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
		if (static_cast<int32>(usage & ImageUsageFlags::HostTransfer) != 0)
			flags |= VkImageUsageFlagBits::VK_IMAGE_USAGE_HOST_TRANSFER_BIT;
		if (static_cast<int32>(usage & ImageUsageFlags::InputAttachment) != 0)
			flags |= VkImageUsageFlagBits::VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
		if (static_cast<int32>(usage & ImageUsageFlags::Sampled) != 0)
			flags |= VkImageUsageFlagBits::VK_IMAGE_USAGE_SAMPLED_BIT;
		if (static_cast<int32>(usage & ImageUsageFlags::Storage) != 0)
			flags |= VkImageUsageFlagBits::VK_IMAGE_USAGE_STORAGE_BIT;
		if (static_cast<int32>(usage & ImageUsageFlags::TransferDst) != 0)
			flags |= VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		if (static_cast<int32>(usage & ImageUsageFlags::TransferSrc) != 0)
			flags |= VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
		if (static_cast<int32>(usage & ImageUsageFlags::TransientAttachment) != 0)
			flags |= VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT;
		if (static_cast<int32>(usage & ImageUsageFlags::VideoDecodeDpb) != 0)
			flags |= VkImageUsageFlagBits::VK_IMAGE_USAGE_VIDEO_DECODE_DPB_BIT_KHR;
		if (static_cast<int32>(usage & ImageUsageFlags::VideoDecodeDst) != 0)
			flags |= VkImageUsageFlagBits::VK_IMAGE_USAGE_VIDEO_DECODE_DST_BIT_KHR;
		if (static_cast<int32>(usage & ImageUsageFlags::VideoDecodeSrc) != 0)
			flags |= VkImageUsageFlagBits::VK_IMAGE_USAGE_VIDEO_DECODE_SRC_BIT_KHR;
		if (static_cast<int32>(usage & ImageUsageFlags::VideoEncodeDpb) != 0)
			flags |= VkImageUsageFlagBits::VK_IMAGE_USAGE_VIDEO_ENCODE_DPB_BIT_KHR;
		if (static_cast<int32>(usage & ImageUsageFlags::VideoEncodeDst) != 0)
			flags |= VkImageUsageFlagBits::VK_IMAGE_USAGE_VIDEO_ENCODE_DST_BIT_KHR;
		if (static_cast<int32>(usage & ImageUsageFlags::VideoEncodeSrc) != 0)
			flags |= VkImageUsageFlagBits::VK_IMAGE_USAGE_VIDEO_ENCODE_SRC_BIT_KHR;

		return flags;
	}

	constexpr VkFilter ConvertSamplerFilterVulkan(ME::Render::SamplerFilter filter)
	{
		switch (filter)
		{
			case SamplerFilter::None:
				return VK_FILTER_MAX_ENUM;
			case SamplerFilter::Nearest:
				return VK_FILTER_NEAREST;
			case SamplerFilter::Linear:
				return VK_FILTER_LINEAR;
		}
	    return VK_FILTER_MAX_ENUM;
	}

	constexpr ME::Render::ImageLayout ConvertImageLayoutEngine(VkImageLayout layout)
	{
		return ImageLayout::Undefined;
	}

	constexpr ME::Render::LoadOperation ConvertAttachmentLoadOperationEngine(VkAttachmentLoadOp operation)
	{
		return LoadOperation::None;
	}

	constexpr ME::Render::StoreOperation ConvertAttachmentStoreOperationEngine(VkAttachmentStoreOp operation)
	{
		return StoreOperation::None;
	}

	constexpr ME::Render::PipelineBindPoint ConvertPipelineBindPointEngine(VkPipelineBindPoint operation)
	{
		return PipelineBindPoint::None;
	}

	constexpr ME::Render::PipelineStageFlags ConvertPipelineStageFlagsEngine(VkPipelineStageFlags flags)
	{
		return PipelineStageFlags::None;
	}

	constexpr ME::Render::AccessFlags ConvertAccessFlagsEngine(VkAccessFlags flags)
	{
		return AccessFlags::None;
	}

	const asciichar* VulkanMessageTypeToText(const VkDebugUtilsMessageTypeFlagsEXT& messageType)
	{
		switch (messageType)
		{
			case VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT:
				return "General";
			case VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT:
				return "Validation";
			case VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT:
				return "Performance";
			case VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT:
				return "Device address binding";
			default:
				return "Unknown";
		}
	}
}
