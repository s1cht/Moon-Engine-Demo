#pragma once

#include <Core/Platform/Platform.hpp>

#ifndef ME_WND_CLASSNAME
	#define ME_WND_CLASSNAME "PEngineWindow"
#endif

#ifdef PLATFORM_WINDOWS
    #include <initguid.h>

    // Graphics APIs
	#include <d3d12.h>
	#include <dxgi1_2.h>

    // Audio API
    #include <xaudio2.h>
    #include <xaudio2fx.h>
    #include <mmdeviceapi.h>

	#define D3D_COMPILE_STANDARD_FILE_INCLUDE ((ID3DInclude*)(UINT_PTR)1)

#endif