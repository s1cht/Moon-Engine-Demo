#pragma once

#include "Core.h"
#include "imgui.h"

#ifdef PLATFORM_WINDOWS

	#include "backends/imgui_impl_win32.h"
	#include "backends/imgui_impl_dx11.h"
	#include "backends/imgui_impl_dx12.h"
	#include "backends/imgui_impl_vulkan.h"

#else

#endif