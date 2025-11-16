#pragma once

#include <Core.hpp>
#include <Core/Platform/Platform.hpp>

#include "Renderer/RenderCommand.hpp"
#include "Renderer/API/Vulkan/VulkanRenderAPI.hpp"

// ImGui libraries
#include <imgui.h>
#include <backends/imgui_impl_vulkan.h>

// Windows libraries
#ifdef PLATFORM_WINDOWS
	// Window
	#include "Platform/Windows/Win32Window.hpp"
	#include <backends/imgui_impl_win32.h>
	
	// DirectX12 library
	//#include "Renderer/API/DirectX12/DirectX12Renderer.h"
	#include <backends/imgui_impl_dx12.h>

#elif defined(PLATFORM_LINUX)
    // Window
#elif PLATFORM_MAC
    // Window
    #include <backends/imgui_impl_metal.h>
#endif