#pragma once

#include <Core/Platform/Platform.h>

#ifndef PE_WND_CLASSNAME
	#define PE_WND_CLASSNAME L"PEngineWindow"
#endif

#ifdef PLATFORM_WINDOWS
	#include <d3d11.h>
	#include <d3d12.h>
	#include <dxgi1_2.h>
	//#include <vulkan/vulkan.h>

#endif