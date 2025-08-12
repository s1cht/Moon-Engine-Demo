#pragma once

#include <Core.hpp>

#ifdef PLATFORM_WINDOWS
	#define VK_USE_PLATFORM_WIN32_KHR
	#include "Platform/Windows/Win32Window.h"
	#include "Platform/Windows/Win32Platform.h"
#endif

#include <vulkan/vulkan.hpp>
#include <vk_mem_alloc.h>

#include "VulkanMacros.hpp"