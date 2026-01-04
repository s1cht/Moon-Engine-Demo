#pragma once

#define ME_VK_INITIAL_MAX_DESCRIPTOR_SETS 1000
#define ME_VK_MAX_FRAME_COUNT 3
#define ME_VK_RETURN_V(val) (int32(val))
#define ME_VK_FAILED(result) (result != (int32)VkResult::VK_SUCCESS || (int32)ME::Render::VulkanErrors::Success)

#define ME_DRAW_INDIRECT_NECESSITIES VkDrawIndexedIndirectCommand

#define ME_VK_TO_UINT_HANDLE(handle) uint64(handle)

#define ME_VK_LOG_OUTPUT_FORMAT(className, msg) ME_RENDER_LOG_OUTPUT_FORMAT("Vulkan", className, msg)