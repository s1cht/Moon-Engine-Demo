#pragma once

#define ME_VK_INITIAL_MAX_DESCRIPTOR_SETS 1000
#define ME_VK_MAX_FRAME_COUNT 3
#define ME_VK_RETURN_V(val) (int32(val))
#define ME_VK_FAILED(result) (result != (int32)VkResult::VK_SUCCESS || (int32)ME::Render::VulkanErrors::Success)