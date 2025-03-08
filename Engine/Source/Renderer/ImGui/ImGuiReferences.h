#pragma once

#include "Core.h"
#include "imgui.h"

#include "backends/imgui_impl_vulkan.h"
#include "backends/imgui_impl_opengl3.h"

#ifdef PLATFORM_WINDOWS

	#include "backends/imgui_impl_win32.h"
	
	// Rendering APIs
	#include "backends/imgui_impl_dx11.h"
	#include "backends/imgui_impl_dx12.h"

#elif PLATFORM_MAC

	#include "backends/imgui_impl_metal.h"

#endif