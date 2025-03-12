#pragma once

#include <Core/Platform/Platform.h>

#include "Renderer/RenderCommand.h"

#include "imgui.h"
#include "backends/imgui_impl_vulkan.h"
#include "backends/imgui_impl_opengl3.h"

#ifdef PLATFORM_WINDOWS

	#include "Platform/Windows/Win32Window.h"
		

	#include <backends/imgui_impl_win32.h>
	
	// Rendering APIs
	#include "Renderer/API/DirectX11/DirectX11Renderer.h"
	#include <backends/imgui_impl_dx11.h>
	//#include "Renderer/API/DirectX12/DirectX12Renderer.h"
	#include <backends/imgui_impl_dx12.h>

#elif PLATFORM_MAC

	#include <backends/imgui_impl_metal.h>

#endif