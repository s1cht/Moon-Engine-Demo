#pragma once

#define PE_VK_RETURN_V(val) (int32(val))
#define PE_VK_FAILED(result) (result != (int32)VkResult::VK_SUCCESS || (int32)VulkanErrors::Success)