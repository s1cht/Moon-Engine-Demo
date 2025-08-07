#pragma once

#define ME_VK_RETURN_V(val) (int32(val))
#define ME_VK_FAILED(result) (result != (int32)VkResult::VK_SUCCESS || (int32)ME::Render::VulkanErrors::Success)