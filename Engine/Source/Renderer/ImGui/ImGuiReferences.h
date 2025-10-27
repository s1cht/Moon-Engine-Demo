#pragma once

#include <Core.hpp>
#include <Core/Platform/Platform.hpp>

#include "Renderer/RenderCommand.h"
#include "Renderer/API/Vulkan/VulkanRenderAPI.hpp"

#include <imgui.h>
#include <backends/imgui_impl_vulkan.h>

#ifdef PLATFORM_WINDOWS
	// Window
	#include "Platform/Windows/Win32Window.h"
	#include <backends/imgui_impl_win32.h>
	
	// Rendering APIs
	//#include "Renderer/API/DirectX12/DirectX12Renderer.h"
	//#include <backends/imgui_impl_dx12.h>

#elif defined(PLATFORM_LINUX)

#elif PLATFORM_MAC

	#include <backends/imgui_impl_metal.h>

#endif